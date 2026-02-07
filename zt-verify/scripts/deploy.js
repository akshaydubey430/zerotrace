const hre = require("hardhat");

async function main() {
    const WipeRecord = await hre.ethers.getContractFactory("WipeRecord");
    const wipeRecord = await WipeRecord.deploy();

    await wipeRecord.waitForDeployment();

    console.log(
        `WipeRecord deployed to ${await wipeRecord.getAddress()}`
    );
}

// We recommend this pattern to be able to use async/await everywhere
// and properly handle errors.
main().catch((error) => {
    console.error(error);
    process.exitCode = 1;
});
