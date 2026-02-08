const { expect } = require("chai");

describe("WipeRecord", function () {
    it("Should record and verify a wipe", async function () {
        const WipeRecord = await ethers.getContractFactory("WipeRecord");
        const wipeRecord = await WipeRecord.deploy();

        // Test Data
        const ipfsHash = "QmSimulatedTestHash123";
        const deviceId = "DEVICE-001";
        const wipeMethod = "ATA_SECURE_ERASE";

        // Add Record
        await wipeRecord.addRecord(ipfsHash, deviceId, wipeMethod);

        // Verify Record
        const result = await wipeRecord.verifyRecord(ipfsHash);

        expect(result[0]).to.equal(true); // Exists
        expect(result[1]).to.equal(deviceId);
        expect(result[2]).to.equal(wipeMethod);
        // Timestamp should be valid (greater than 0)
        expect(result[3]).to.be.gt(0);

        // Verify non-existent record
        const badResult = await wipeRecord.verifyRecord("QmNonExistent");
        expect(badResult[0]).to.equal(false);
    });
});
