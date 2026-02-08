const express = require('express');
const { ethers } = require('ethers');
const dotenv = require('dotenv');
dotenv.config();
const fs = require('fs');
const app = express();
app.use(express.json());

// CONFIG
const RPC_URL = "http://127.0.0.1:8545";
const PRIVATE_KEY = process.env.PRIVATE_KEY ? process.env.PRIVATE_KEY.trim() : null;
const CONTRACT_ADDR = "0xe7f1725E7734CE288F8367e1Bb143E90bb3F0512";

if (!PRIVATE_KEY) {
  throw new Error("PRIVATE_KEY not set");
}

// Load ABI synchronously
const artifact = JSON.parse(
  fs.readFileSync("DeviceWipeRegistry.json", "utf8")
);
const ABI = artifact.abi;

const provider = new ethers.JsonRpcProvider(RPC_URL);
const wallet = new ethers.Wallet(PRIVATE_KEY, provider);
const contract = new ethers.Contract(CONTRACT_ADDR, ABI, wallet);

app.post("/record-wipe", async (req, res) => {
  try {
    const { cert_hash, device_hash, wipe_method } = req.body;

    const WIPE_METHODS = [
      "Plain Overwrite",
      "Encrypted Overwrite", 
      "Firmware Erase",
      "ATA Secure Erase"
    ];

    const methodString = WIPE_METHODS[wipe_method] || "Unknown";

    const tx = await contract.recordCertificate(
      device_hash, // deviceId
      cert_hash,   // certHash
      methodString // wipeMethod
    );

    await tx.wait();

    res.json({
      status: "ok",
      tx_hash: tx.hash
    });
  } catch (e) {
    res.status(400).json({
      status: "error",
      message: e.reason || e.message
    });
  }
});



app.post("/verify-wipe", async (req, res) => {
  try {
    const { device_hash, cert_hash } = req.body;

    // Returns: [valid (bool), timestamp (uint256), issuer (address)]
    const result = await contract.verifyCertificate(
      device_hash,
      cert_hash
    );

    // Result decoding
    const verified = result[0];
    const timestamp = Number(result[1]);
    const issuer = result[2];

    res.json({
      status: "ok",
      verified,
      timestamp,
      issuer
    });
  } catch (e) {
    res.status(400).json({
      status: "error",
      message: e.reason || e.message
    });
  }
});

app.listen(8080, () =>
  console.log("ZT chain helper listening on :8080")
);
