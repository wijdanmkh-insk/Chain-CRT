# Chain-CRT
## _Your Alive Bag Companion Ever_

Do you ever wanted a cute, interactive keychain-like a Tamagotchi-that feels alive? Or do you want that robot responds to your car motions like Dasai Mochi? Maybe you'd love to have one hanging on your bag, but can't afford the real one? Don't worry! Chain-CRT here to give you small, cheap DIY project using an ESP32 to create a living keychain that responds to your physically active body. 

## Requirements
There are few requirements to meet your companion for the first shot. Here are the lists of your requirements. 
### Software
- Arduino IDE / PlatformIO (Depending on the board you're using)
- Required libraries :
-- **MPU6050**
-- **SSD1306**
-- **SD CARD**
-- **SPI**
-- _See the full list in the source code_

### Hardware
- **Small-sized ESP32**
Recommended : ESP32 - C3 or ESP32-C6 for compact size
- **SD CARD MODULE + microSD card**
Used to store expression files that won't fit in flash memory + data logging for debugging
- **MPU6050** 
To detect motions like shaking, falling, tilting, petting, etc.
- **2 x 10k ohm Resistor**
As voltage divider circuit to read battery voltage, attached to ESP32 ADC pin. Helps monitor the battery level and trigger automatic shutdown when the power is low, depending on the system configuration
- **1 x 3.3V Zener Diode**
As a current protector from reverse current. Attach the zener diode on the bottom of the resistor divider, attached to ESP32 GND pin.
- **3.3V LiPo Battery** 
As power source.
-- For **DFRobot** or **Xiao ESP32** boards : connect to **BAT** pin for positive and **Gnd** for negative. Xiao have BAT+ and BAT-, so you can attach the battery directly into the BAT pin. 
-- For other boards : use a **TP4056** charging module to enable safe charging.
- **SSD1304**
To show expressions.

### Features
- Responsive facial expressions triggered by motion
- Battery monitoring and automatic shutdown
- Customized Wi-Fi configuration to help you stay connected and updated to time
- Auto Wi-Fi connect/disconnect after syncing time 
- Detects environment temps
- Automatic deep sleep when there's no motion detected and turns on when there's a motion detected. 


⚙️ This project is on progress, so stay tuned for the new features!