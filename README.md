# Dual Axis Solar Tracker
**🏆 Third Place in Micro Computers competition!**

A two-axis (pan/tilt) solar tracking system built on a PIC16F1829, developed as a team final project for Micro Computer course. The tracker automatically orients a solar panel toward the brightest available light source using quadrant photoresistor sensing and closes the loop through hardware PWM servo control, while charging a LiPo battery from the panel.


## Overview
 
- **MCU:** Microchip PIC16F1829
- **IDE:** MPLAB X IDE with the XC8 compiler
- **Sensing:** Four photoresistors in a quadrant layout 
- **Actuation:** Two MG995 servos, driven by hardware PWM
- **Display:** SSD1306 OLED showing live pan/tilt angle and system status
- **Power:** Panel charged LiPo battery with a shift-register LED bar gauge and push-button readout
- **Mechanical:** Custom 3D printed  gimbal assembly designed in Fusion 360

<img width="600" height="600" alt="solar" src="https://github.com/user-attachments/assets/8d63bffe-3840-49da-93a4-bbd6e65211d2" />

<img width="540" height="540" alt="solar1" src="https://github.com/user-attachments/assets/3bb0d55f-6e80-4521-b8fb-6805366ce187" />
