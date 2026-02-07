const hre = require("hardhat");

async function main() {
    const args = process.argv.slice(2);
    const command = args[0];

    if (!command) {
        console.log("Usage: npx hardhat run scripts/interact.js --network localhost -- <command> [args...]");
        console.log("Commands:");
        console.log("  record <contractAddress> <ipfsHash> <deviceId> <wipeMethod>");
        console.log("  verify <contractAddress> <ipfsHash>");
        return;
    }

    const contractAddress = args[1];
    const WipeRecord = await hre.ethers.getContractFactory("WipeRecord");
    const contract = WipeRecord.attach(contractAddress);

    if (command === "record") {
        const ipfsHash = args[2];
        const deviceId = args[3];
        const wipeMethod = args[4];

        console.log(`Recording wipe for device ${deviceId} with IPFS hash ${ipfsHash}...`);
        const tx = await contract.addRecord(ipfsHash, deviceId, wipeMethod);
        await tx.wait();
        console.log("Wipe recorded successfully!");

    } else if (command === "verify") {
        const ipfsHash = args[2];
        console.log(`Verifying wipe for IPFS hash ${ipfsHash}...`);

        const result = await contract.verifyRecord(ipfsHash);
        if (result[0]) { // bool exists
            console.log("✅ VALID WIPE RECORD FOUND");
            console.log("Device ID:", result[1]);
            console.log("Wipe Method:", result[2]);
            console.log("Timestamp:", new Date(Number(result[3]) * 1000).toISOString());
        } else {
            console.log("❌ NO RECORD FOUND for this hash.");
        }
    } else {
        console.log("Unknown command");
    }
}

main().catch((error) => {
    console.error(error);
    process.exitCode = 1;
});
