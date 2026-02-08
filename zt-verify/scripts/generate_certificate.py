import json
import hashlib
import time
import argparse
import os

def generate_certificate(device_id, model, size, method, output_file="certificate.json"):
    """
    Generates a wipe certificate and simulates IPFS upload.
    In a real scenario, this would upload the JSON to an IPFS node.
    """
    certificate_data = {
        "device_id": device_id,
        "device_model": model,
        "device_size_bytes": size,
        "wipe_method": method,
        "timestamp": int(time.time()),
        "status": "SUCCESS",
        "issuer": "ZeroTrace Verification System"
    }

    # Convert to JSON string
    json_str = json.dumps(certificate_data, indent=4)

    # Save to file
    with open(output_file, "w") as f:
        f.write(json_str)
    
    print(f"Certificate saved to {output_file}")

    # Simulate IPFS Hash (SHA-256 of the content)
    # In reality, this would be the CID returned by the IPFS node
    ipfs_hash = hashlib.sha256(json_str.encode()).hexdigest()
    
    # Store the Mock IPFS CID format (often starts with Qm...)
    # We'll just use the hex hash for this simulation or prefix it
    mock_ipfs_cid = f"QmSimulated{ipfs_hash[:40]}"

    print(f"Simulated IPFS Upload Complete.")
    print(f"IPFS Hash (CID): {mock_ipfs_cid}")

    with open("last_cid.txt", "w") as f:
        f.write(mock_ipfs_cid)

    return mock_ipfs_cid

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate ZeroTrace Wipe Certificate")
    parser.add_argument("--device", required=True, help="Device ID or Serial")
    parser.add_argument("--model", required=True, help="Device Model")
    parser.add_argument("--size", required=True, type=int, help="Device Size in Bytes")
    parser.add_argument("--method", required=True, help="Wipe Method Used")
    parser.add_argument("--output", default="certificate.json", help="Output JSON file path")

    args = parser.parse_args()

    cid = generate_certificate(args.device, args.model, args.size, args.method, args.output)
    
    # Output solely the CID on the last line for easier parsing by other scripts
    # or just print it clearly.
    # print(cid) 
