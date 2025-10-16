# Gateway

This folder contains the **Gateway** component of the ranging stack, which bridges the **Node** and the **Network Server**.  
It is divided into two main parts:

- **Hardware**  
- **Firmware**

---

## 🛠️ Hardware

The hardware consists of a **microcontroller with Wi-Fi capability** (ESP32-S3, a dual-core MCU) connected to the **SX1280** transceiver.  

In our reference design, we use:

- **Adafruit ESP32-S3 QT Py**  
- **Ebyte SX1280 module**

### Options
- If you reproduce our design (PCB provided), you can **directly upload the firmware** without modification.  
- You may also use a different ESP32-S3 development board, but in that case, update the pin configuration inside `Settings.h` in the firmware.

### PCB
- To fabricate the PCB, use the **Gerber files** included in the `hardware` folder.  
- You can also modify the design using **EasyEDA** PCB design software.

---

## 💻 Firmware

Once the hardware is ready, the next step is to upload the firmware.

### Toolchain
- We recommend using **PlatformIO** (VS Code extension) for building and flashing the firmware.

### Configuration
Before flashing, configure your device by editing **`identifier.cpp`**:

- `NetworkID` — unique identifier for the network  
- `SSID` and `Password` — Wi-Fi credentials  
- `MQTT Server` and `Port` — address of the MQTT broker (used by the NS)  
- `MQTT Topics` — topics used for communication with the NS

Once configured, build and upload the firmware with PlatformIO to your ESP32-S3.

---

## 📜 Licensing

Refer to the `LICENSE` file(s) inside the subfolders for specific licensing details.
