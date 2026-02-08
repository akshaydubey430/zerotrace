import os
import subprocess
import time
import json
import requests
import signal
import sys
import shutil

def run_command(command, cwd=None, background=False, env=None):
    print(f"Running: {command}")
    
    # Merge current environment with passed env
    cmd_env = os.environ.copy()
    if env:
        cmd_env.update(env)

    if background:
        return subprocess.Popen(command, shell=True, cwd=cwd, env=cmd_env, preexec_fn=os.setsid)
    else:
        result = subprocess.run(command, shell=True, cwd=cwd, env=cmd_env, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"Error running command: {command}")
            print(result.stderr)
            sys.exit(1)
        return result

def main():
    print("=== ZeroTrace Demo/Test Script ===")
    
    root_dir = os.getcwd()
    verify_dir = os.path.join(root_dir, "zt-verify")
    chain_dir = os.path.join(root_dir, "zt-chain")

    # 1. Install Dependencies
    print("\n--- Installing Dependencies ---")
    run_command("npm install", cwd=verify_dir)
    run_command("npm install", cwd=chain_dir)

    # 2. Start Hardhat Node
    print("\n--- Starting Hardhat Node ---")
    # Redirect output to prevent clutter
    node_process = run_command("npx hardhat node", cwd=verify_dir, background=True)
    time.sleep(5) # Wait for node to startup

    try:
        # 3. Deploy Contract
        print("\n--- Deploying Contract ---")
        run_command("npx hardhat run scripts/deploy.js --network localhost", cwd=verify_dir)

        # 4. Get Contract Address
        addr_file = os.path.join(verify_dir, "contract_address.txt")
        if not os.path.exists(addr_file):
            print("Error: contract_address.txt not found.")
            sys.exit(1)
        
        with open(addr_file, "r") as f:
            contract_address = f.read().strip()
        print(f"Contract deployed at: {contract_address}")

        # 5. Setup zt-chain
        print("\n--- Setting up Middleware (zt-chain) ---")
        
        # Copy artifact
        artifact_src = os.path.join(verify_dir, "artifacts", "contracts", "DeviceWipeRegistry.sol", "DeviceWipeRegistry.json")
        artifact_dst = os.path.join(chain_dir, "DeviceWipeRegistry.json")
        shutil.copy(artifact_src, artifact_dst)

        # Update address in index.js
        index_js_path = os.path.join(chain_dir, "index.js")
        with open(index_js_path, "r") as f:
            content = f.read()
        
        # Regex or simple string replace for the const definition
        # We look for: const CONTRACT_ADDR = "..."
        import re
        new_content = re.sub(
            r'const CONTRACT_ADDR = ".*";',
            f'const CONTRACT_ADDR = "{contract_address}";',
            content
        )
        
        with open(index_js_path, "w") as f:
            f.write(new_content)

        # 6. Start zt-chain
        print("\n--- Starting Middleware Server ---")
        # Default Hardhat Account #0 Private Key
        env_vars = {
            "PRIVATE_KEY": "0xac0974bec39a17e36ba4a6b4d238ff944bacb478cbed5efcae784d7bf4f2ff80"
        }
        chain_process = run_command("node index.js", cwd=chain_dir, background=True, env=env_vars)
        time.sleep(3) # Wait for server

        # 7. Simulate Client
        print("\n--- Simulating Client Wipe Record ---")
        
        # Dummy data matching zt-client logic
        cert_hash = "0x" + "a" * 64
        device_hash = "0x" + "b" * 64
        wipe_method = 0 # Plain Overwrite

        payload = {
            "cert_hash": cert_hash,
            "device_hash": device_hash,
            "wipe_method": wipe_method
        }

        try:
            r = requests.post("http://127.0.0.1:8080/record-wipe", json=payload)
            print(f"Record Response: {r.status_code} {r.text}")
            if r.status_code != 200:
                print("Failed to record wipe.")
                sys.exit(1)
        except Exception as e:
            print(f"Connection failed: {e}")
            sys.exit(1)

        print("\n--- Verifying Wipe on Blockchain ---")
        verify_payload = {
            "device_hash": device_hash,
            "cert_hash": cert_hash
        }

        try:
            r = requests.post("http://127.0.0.1:8080/verify-wipe", json=verify_payload)
            print(f"Verify Response: {r.status_code} {r.text}")
            data = r.json()
            if data.get("verified") == True:
                print("\nSUCCESS: Wipe verified on blockchain!")
            else:
                print("\nFAILURE: Wipe not verified.")
                sys.exit(1)
        except Exception as e:
             print(f"Connection failed: {e}")
             sys.exit(1)

    finally:
        print("\n--- Cleaning Up ---")
        if 'node_process' in locals():
            os.killpg(os.getpgid(node_process.pid), signal.SIGTERM)
        if 'chain_process' in locals():
            os.killpg(os.getpgid(chain_process.pid), signal.SIGTERM)

if __name__ == "__main__":
    main()
