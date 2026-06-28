# 🚀 ESP32-S3-POE-ETH-8DI-8DO_template

ตัวอย่าง firmware & library สำหรับบอร์ด **Waveshare ESP32-S3-POE-ETH-8DI-8DO** ที่พัฒนาโดย **Tenergy Innovation**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
![Platform: ESP32-S3](https://img.shields.io/badge/Platform-ESP32--S3-blue)
![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-00599C)

---

## 📋 ลักษณะเด่น

### Hardware Support
- 🔌 **8 Digital Input (DI)** - Optocoupler isolated
- 🔌 **8 Digital Output (DO)** - Relay controlled via TCA9554 I2C expander
- 🔗 **RS485 Interface** - UART1 (GPIO17/TX, GPIO18/RX)
- 🌐 **Ethernet + WiFi** - POE powered
- 📡 **Bluetooth 5.0**
- 💡 **RGB LED** - WS2812B NeoPixel (GPIO38)
- 🔊 **Buzzer** - PWM controlled (GPIO46, LEDC channel 0)
- 🎮 **CAN 2.0B** - For industrial communication

### Software Features
- 📝 **Interactive Serial Command Interface**
- 🎨 **RGB LED Control** - Red, Green, Blue, Yellow, Purple, Orange, White
- 🔄 **Status Query** - Read DI/DO states in real-time
- 🎵 **Buzzer Beep** - Configurable beep sequences
- 🔐 **MQTT Support** - Cloud connectivity (optional)
- 🌐 **OTA Firmware Update** - ArduinoOTA enabled
- 📊 **Google Sheets Integration** - Data logging

---

## 🛠️ ติดตั้ง

### 1. Clone Repository
```bash
git clone https://github.com/yourusername/ESP32-S3-POE-ETH-8DI-8DO_template.git
cd ESP32-S3-POE-ETH-8DI-8DO_template
```

### 2. ติดตั้ง PlatformIO
```bash
# macOS / Linux
brew install platformio

# หรือใช้ VSCode Extension
# - Install "PlatformIO IDE" from VSCode Marketplace
```

### 3. Build & Upload
```bash
# Build firmware
platformio run

# Upload ไปที่บอร์ด
platformio run --target upload

# เปิด Serial Monitor
platformio device monitor
```

---

## 💻 คำสั่ง (Commands)

เมื่อเชื่อมต่อ Serial Monitor ที่ 115200 baud:

### 1️⃣ LED Control
```
LED <color> <seconds>

Colors: red, green, blue, yellow, purple, orange, white
Seconds: 0.1 to 10.0 (or -1 to turn OFF)

Examples:
  LED red 2         → แสดงสีแดง 2 วินาที
  LED green 5       → แสดงสีเขียว 5 วินาที
  LED blue -1       → ดับ LED ทันที
```

### 2️⃣ Buzzer Control
```
BUZZER <times>

Times: 1 to 10 beeps

Examples:
  BUZZER 1          → Beep 1 ครั้ง
  BUZZER 3          → Beep 3 ครั้ง
```

### 3️⃣ Digital Output (Relay)
```
OUTPUT <channel> <state>

Channel: 1-8 or DO1-DO8
State: ON/OFF (case-insensitive), 1/0, or ? (read status)

Examples:
  OUTPUT 1 ON       → เปิด relay DO1
  OUTPUT 1 on       → เปิด relay DO1 (lowercase ได้)
  OUTPUT DO5 OFF    → ปิด relay DO5
  OUTPUT 3 1        → เปิด relay DO3 (numeric format)
  OUTPUT 2 0        → ปิด relay DO2 (numeric format)
  OUTPUT 4 ?        → อ่านสถานะ relay DO4
```

### 4️⃣ Digital Input (Sensor Read)
```
INPUT <channel>

Channel: 1-8 or DI1-DI8

Examples:
  INPUT 1           → อ่านสถานะ DI1
  INPUT DI3         → อ่านสถานะ DI3
```

### 5️⃣ System
```
RESET               → รีเซ็ตบอร์ด
HELP                → แสดงรายการคำสั่ง
```

---

## 📦 โครงสร้าง Project

```
ESP32-S3-POE-ETH-8DI-8DO_template/
├── src/
│   └── main.cpp                 ← Interactive serial handler
├── lib/
│   └── ESP32-S3-POE-ETH-8DI-8DO/
│       ├── src/
│       │   ├── ESP32S3_8DI8DO.h      ← Library header
│       │   └── ESP32S3_8DI8DO.cpp    ← Implementation
│       └── examples/
│           ├── BasicDigitalIO/
│           ├── EthernetDemo/
│           └── RS485Modbus/
├── include/
├── platformio.ini
├── README.md
├── LICENSE
├── CHANGELOG.md
├── CONTRIBUTING.md
└── extras/
    ├── PRD.md                   ← Product Requirements Doc
    ├── ai_prompt.md             ← Development notes
    └── (other docs & backups)
```

---

## 📡 RS485 Communication

บอร์ดรองรับการสื่อสาร RS485 ผ่าน **UART1**:
- **TX**: GPIO17
- **RX**: GPIO18
- **RTS**: GPIO21
- **Baud**: 9600 (configurable)

ตัวอย่าง:
```cpp
// ส่งคำสั่ง AT Command ไป LoRa DTU
board.getRS485().write("AT+VER\r\n");

// รับข้อมูลกลับ
String response = board.getRS485().readUntil('\n', 100);
```

---

## 🔌 Pinout Reference

| Component | GPIO | Function |
|-----------|------|----------|
| **RGB LED** | 38 | WS2812B NeoPixel |
| **Buzzer** | 46 | PWM (LEDC ch.0, 1kHz) |
| **RS485 TX** | 17 | UART1 TX |
| **RS485 RX** | 18 | UART1 RX |
| **RS485 RTS** | 21 | Request to Send |
| **DI1-DI8** | 4-11 | Digital Input |
| **DO Expander** | 41(SCL), 42(SDA) | TCA9554 I2C (addr 0x20) |
| **I2C SCL** | 41 | I2C Clock |
| **I2C SDA** | 42 | I2C Data |

---

## 🐛 Troubleshooting

### ❌ "LED red -1" ไม่ดับ
```
✅ Fixed in v1.0.0 - LEDOff() now properly stops animation
```

### ❌ "OUTPUT 5 ON" ติดทุกตัว
```
✅ Fixed in v1.0.0 - State synchronization issue resolved
```

### ❌ Serial Monitor ไม่ได้รับข้อมูล
```
1. ตรวจสอบ baud rate = 115200
2. ตรวจสอบ USB cable
3. กดปุ่ม RESET บนบอร์ด
4. ดู Device Manager / /dev/tty* สำหรับ port
```

---

## 📚 Documentation

- 📖 [ESP32-S3 Official Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- 🌐 [Waveshare Board Wiki](https://www.waveshare.com/wiki/ESP32-S3-POE-ETH-8DI-8DO)
- 📡 [Modbus RTU Protocol](https://en.wikipedia.org/wiki/Modbus)

---

## 🤝 Contributing

ปรับปรุงโปรเจคนี้ได้เสมอ! 👋

ดูรายละเอียดใน [CONTRIBUTING.md](CONTRIBUTING.md)

---

## 📝 License

MIT License - ดู [LICENSE](LICENSE) สำหรับรายละเอียด

---

## 👨‍💻 Author

**Tenergy Innovation**
- 🌐 Website: [tenergy.co.th](https://tenergy.co.th)
- 📧 Email: info@tenergy.co.th
- 📍 Thailand

---

## ❓ FAQ

**Q: ต้องลงทะเบียน WiFi ไหม?**  
A: ไม่ต้อง - ใช้ได้ผ่าน Serial Command แบบ offline

**Q: สามารถขยายขึ้นไปอีก 8 relay ได้ไหม?**  
A: ได้ - เพิ่ม TCA9554 ตัวใหม่ที่ I2C address อื่น

**Q: เทคนิคการโปรแกรมหรือใช้งาน?**  
A: เปิด Issues หรือสอบถามใน Discussion

---

**Happy Coding!** 🎉 If you found this helpful, give us a ⭐ on GitHub!
