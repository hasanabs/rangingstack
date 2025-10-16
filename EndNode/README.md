# End Node

This folder contains the **End Node** component of the ranging stack.  
It is divided into two main parts:

- **Hardware**  
- **Firmware**

---

## 🛠️ Hardware

The hardware consists of a **microcontroller** paired with an **SX1280 transceiver**.  

In our reference design, we created a custom PCB for compactness that integrates:

- **nRF52832 MCU**  
- **SX1280 chip**

### Options
- You may use other microcontrollers, but you must update the pin configuration inside `Settings.h` in the firmware.  
- To reproduce our hardware, you can directly print the **Gerber files** in the `hardware` folder.  

**Recommended PCB specs:**
- **Dual layer**  
- **1.6 mm FR4**  

> ⚠️ Note: The PCB includes an antenna design, which may not be fully optimized. Feel free to adapt it as needed.  
> To modify the design, use the provided **EasyEDA** project files.

---

## 💻 Firmware

Once the hardware is ready, upload the firmware to the node. 
If you use our design, the upload and debug protocol uses CMSIS-DAP with SWD connection. Connect the CMSIS-DAP to the board using a 1.27 mm pogopin to the board with the order of CLK-DIO-VCC-GND from left to right.

### Toolchain
We recommend using **PlatformIO** (VS Code extension) for building and flashing the firmware. 

### Configuration
Before flashing, configure your node by editing **`Settings.h`**:

- `Node_address` — unique identifier for the node  
- `NetworkID` — unique identifier for the network  

After configuration, build and upload the firmware using PlatformIO.

---

### ⚙️ Advanced Configuration

You can modify **LoRa parameters** such as:

- **Spreading Factor (SF)**  
- **Bandwidth (BW)**  
- Other communication/ranging parameters  

> ⚠️ Adjusting these may also require tuning related parameters (e.g., **timeouts**) since different SF/BW values affect **time on air**.

---

## 📜 Licensing

Refer to the `LICENSE` file(s) inside the subfolders for specific licensing details.
