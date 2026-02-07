// SPDX-License-Identifier: MIT
pragma solidity ^0.8.19;

contract WipeRecord {
    struct Record {
        string ipfsHash;
        string deviceId;
        string wipeMethod;
        uint256 timestamp;
        address verifier;
    }

    // Mapping from IPFS Hash to Record
    mapping(string => Record) public records;
    
    // Mapping from Device ID to list of IPFS hashes (history)
    mapping(string => string[]) public deviceHistory;

    event WipeRecorded(string indexed deviceId, string ipfsHash, uint256 timestamp);

    function addRecord(string memory _ipfsHash, string memory _deviceId, string memory _wipeMethod) public {
        require(bytes(records[_ipfsHash].ipfsHash).length == 0, "Record already exists");

        Record memory newRecord = Record({
            ipfsHash: _ipfsHash,
            deviceId: _deviceId,
            wipeMethod: _wipeMethod,
            timestamp: block.timestamp,
            verifier: msg.sender
        });

        records[_ipfsHash] = newRecord;
        deviceHistory[_deviceId].push(_ipfsHash);

        emit WipeRecorded(_deviceId, _ipfsHash, block.timestamp);
    }

    function verifyRecord(string memory _ipfsHash) public view returns (bool, string memory, string memory, uint256) {
        Record memory r = records[_ipfsHash];
        if (bytes(r.ipfsHash).length == 0) {
            return (false, "", "", 0);
        }
        return (true, r.deviceId, r.wipeMethod, r.timestamp);
    }

    function getDeviceWipes(string memory _deviceId) public view returns (string[] memory) {
        return deviceHistory[_deviceId];
    }
}
