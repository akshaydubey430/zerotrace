
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.19;

contract DeviceWipeRegistry {

    struct WipeCertificate {
        bytes32 certHash;      // Hash of certificate file
        string deviceId;       // Serial / UUID / disk WWN
        string wipeMethod;     // e.g. "NIST 800-88 Clear"
        uint256 timestamp;     // Block timestamp at submission
        address issuer;        // Address that submitted
    }

    // deviceId => certificate
    mapping(string => WipeCertificate) private certificates;

    event CertificateRecorded(
        string indexed deviceId,
        bytes32 certHash,
        string wipeMethod,
        uint256 timestamp,
        address indexed issuer
    );

    /// Record a wipe certificate hash
    function recordCertificate(
        string calldata deviceId,
        bytes32 certHash,
        string calldata wipeMethod
    ) external {
        require(certificates[deviceId].timestamp == 0,
            "Certificate already exists for device");

        certificates[deviceId] = WipeCertificate({
            certHash: certHash,
            deviceId: deviceId,
            wipeMethod: wipeMethod,
            timestamp: block.timestamp,
            issuer: msg.sender
        });

        emit CertificateRecorded(
            deviceId,
            certHash,
            wipeMethod,
            block.timestamp,
            msg.sender
        );
    }

    /// Verify a certificate hash against on-chain record
    function verifyCertificate(
        string calldata deviceId,
        bytes32 certHash
    ) external view returns (bool valid, uint256 timestamp, address issuer) {
        WipeCertificate memory cert = certificates[deviceId];

        if (cert.timestamp == 0) {
            return (false, 0, address(0));
        }

        return (
            cert.certHash == certHash,
            cert.timestamp,
            cert.issuer
        );
    }

    /// Fetch full metadata (optional)
    function getCertificate(
        string calldata deviceId
    ) external view returns (
        bytes32 certHash,
        string memory wipeMethod,
        uint256 timestamp,
        address issuer
    ) {
        WipeCertificate memory cert = certificates[deviceId];
        require(cert.timestamp != 0, "No certificate found");

        return (
            cert.certHash,
            cert.wipeMethod,
            cert.timestamp,
            cert.issuer
        );
    }
}
