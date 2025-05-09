# 🚗 Smart Parking Lot System

This project implements a **smart parking lot** system using a **PIC18F45K22 microcontroller**, programmed in **C** with **MPLAB**, and validated using **Proteus simulation**. The system monitors three parking spots and automates gate control, displays availability, and ensures real-time interaction with cars using sensors.

---

## 📌 Overview

- **Microcontroller**: PIC18F45K22
- **Sensors**: E18-D80NK Infrared Proximity Sensors (Entrance + Exit)
- **Actuator**: Servo Motor (Gate control via PWM)
- **Display**: LCD (Real-time spot availability)
- **Programming**: C language via MPLAB
- **Simulation**: Validated using Proteus

---

## 🧠 Features

- Automatic gate control (open/close based on car detection)
- Live count of available parking spots
- Entry denied when lot is full
- Buzzer and LED indicators
- EEPROM usage to store parking count
- PWM generation for motor control

---

## 🗂️ File Structure

Smart-Parking-System/
├── src/
│   └── main.c                   # Main embedded C file
├── docs/
│   └── hassan-melissa-report.docx   # Project report with explanation, diagrams
├── simulation/
│   └── parkingLot.pdsprj       # Proteus project file

---

## 🚀 How to Run

### 🔧 MPLAB Setup

1. Open `main.c` in MPLAB X IDE
2. Select target device: `PIC18F45K22`
3. Set clock to `8 MHz`
4. Build and upload to MCU (if using real hardware)

### 🧪 Proteus Simulation

1. Open `parkingLot.pdsprj` in Proteus 8
2. Run simulation to test:

   * Start button
   * Car entry (entrance sensor)
   * Car exit (exit sensor)
   * LCD response
   * Motor control (angle change)

---

## 👨‍🎓 Authors

* **Hassan Mroueh**
* **Melissa Naffah**
  *Submitted for CPEN 305 - Microcontrollers Lab at University of Balamand*

---

## 📝 License

For academic use only.


