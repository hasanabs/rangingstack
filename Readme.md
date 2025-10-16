# Ranging Stack Framework

## 📄 Overview

A modular, reproducible framework for localization using LoRa ranging-capable transceiver, designed for **scalability** and **energy efficiency**.

The stack is composed of:
- **Network Server** — containerized service coordinating nodes and gateways
- **Gateway** — bridges nodes to the Network Server (hardware + firmware)
- **End Node** — LoRa ranging-capable device (hardware + firmware)

---

## 📁 Repository Structure

- **`NetworkServer/`**  
  Demo Network Server packaged for Docker deployment.  
  → See `NetworkServer/README.md` for usage, notes, and licensing.

- **`Gateway/`**  
  - `hardware:` — ESP32-S3 + SX1280 reference design (Gerbers, EasyEDA)  
  - `firmware:` — PlatformIO project and configuration notes  
  → See `Gateway/README.md`.

- **`EndNode/`**  
  - `hardware:` — nRF52832 + SX1280 compact PCB (Gerbers, EasyEDA)  
  - `firmware:` — PlatformIO project and configuration notes  
  → See `EndNode/README.md`.

> Each subcomponent includes its own license file. Please review it.

---

## 📫 Contact

For questions or collaboration inquiries:

- **Hasan Albinsaid** — `mail@hasanabs.com`
