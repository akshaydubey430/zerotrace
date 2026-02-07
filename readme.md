# ZeroTrace

**Secure Data Wiping for Trustworthy IT Asset Recycling**

ZeroTrace is a high-performance, bootable USB utility designed to securely sanitize storage devices for safe reuse or recycling. It operates below the operating system level and complies with **NIST SP 800-88** media sanitization standards, ensuring data is permanently unrecoverable and **cryptographically verifiable via blockchain**.

---

## Problem Statement

- Over **1.75 million tonnes of e-waste** are generated annually in India.
- ₹50,000+ crore worth of unused devices remain idle due to **data privacy fears**.
- Users lack:
  - Reliable data destruction methods
  - Tamper-proof proof of sanitization
  - Trust in recycling vendors

---

## Solution Overview

ZeroTrace provides a **low-level, OS-independent disk sanitization tool** that:

- Securely wipes storage devices using industry-approved methods
- Generates **tamper-proof Certificates of Destruction**
- Stores verification records on **Ethereum blockchain**
- Enables public, third-party verification of wipe status

---

## Key Features

### 🔐 NIST-Compliant Data Sanitization
- Firmware-level erase (ATA Secure Erase / NVMe Format)
- Cryptographic erase for self-encrypted drives
- Multi-pass overwrite for non-encrypted drives

### 💻 Cross-Platform Bootable Utility
- Works on **Windows, Linux, Android**
- Compatible with SATA, NVMe, USB, eMMC
- Bypasses OS-level and filesystem restrictions

### ⛓️ Blockchain-Verified Certification
- Certificates stored on **IPFS**
- Verification hashes anchored on **Ethereum**
- Immutable, tamper-proof wipe proof

### 🎯 Designed for All Users
- One-click, intuitive interface
- Free for individuals, scalable for enterprises

---

## Technical Architecture

### Step 1 — Boot & Device Discovery
- Boot from a **Rust-based USB utility**
- Enumerate connected storage devices
- Display device specifications and supported erase methods

---

### Step 2 — Wipe Method Selection
ZeroTrace prioritizes **device-native erase methods**:
1. Firmware Secure Erase (Preferred)
2. Cryptographic Erase
3. Data Overwrite (Fallback)

---

### Step 3 — Secure Key Destruction (Self-Encrypted Devices)

For **Self-Encrypted Drives (SEDs)**, ZeroTrace destroys the internal encryption keys instead of re-encrypting data.

#### Key Destruction Workflow
- Detect self-encrypted storage
- Trigger firmware-level key purge
- Securely erase wrapped encryption keys from device memory
- Flush internal caches and metadata

#### Example Commands (Device-Specific)
```bash
# ATA Secure Erase (SED)
hdparm --user-master u --security-set-pass NULL /dev/sdX
hdparm --user-master u --security-erase NULL /dev/sdX

# NVMe Secure Format
nvme format /dev/nvme0n1 --ses=1
