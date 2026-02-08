const hre = require("hardhat");

async function main() {
    const Registry = await hre.ethers.getContractFactory("DeviceWipeRegistry");
    const registry = await Registry.deploy();

    await registry.waitForDeployment();

    const address = await registry.getAddress();
    console.log(
        `WipeRecord deployed to ${address}`
    );
    
    const fs = require("fs");
    fs.writeFileSync("contract_address.txt", address);
}

// We recommend this pattern to be able to use async/await everywhere
// and properly handle errors.
main().catch((error) => {
    console.error(error);
    process.exitCode = 1;
});
