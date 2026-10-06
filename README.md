# ESP32 RFID Attendance System

> A smart, automated RFID attendance system built with ESP32, RC522 RFID, OLED display, buzzer, Wi-Fi, Make.com, and Google Sheets.


## 📌 Project Overview

The **ESP32 RFID Attendance System** is an IoT-based attendance solution that uses an RFID card or tag to record attendance automatically.

When an RFID card is scanned, the ESP32 identifies the card's UID and determines whether the event should be:

- `CLOCK_IN`
- `CLOCK_OUT`

The attendance event is then sent over Wi-Fi to a **Make.com Custom Webhook**, which processes the data and stores it in **Google Sheets**.

Every scan creates a new attendance record.

The system supports multiple RFID cards, and each card maintains its own independent CLOCK IN / CLOCK OUT state.

---

## ✨ Features

- 🔐 RFID-based attendance system
- 📡 ESP32 Wi-Fi connectivity
- 🪪 Supports any RFID card/tag
- 👥 Supports multiple RFID cards
- 🔄 Automatic CLOCK IN / CLOCK OUT alternation
- ♾️ Unlimited scans during device operation
- 📊 Google Sheets attendance logging
- ⚡ Make.com webhook automation
- 🖥️ 128×64 SSD1306 OLED display
- 🔊 Buzzer feedback
- 💡 LED feedback
- 🎵 Buzzer and LED beat demonstration
- 📦 JSON-based communication
- 🚫 No hardcoded RFID UID registration required
- 🚫 No duplicate filtering in Make.com
- 📝 Every successful scan creates a new Google Sheets row

---

# 🏗️ System Architecture

```text
                    ┌──────────────────┐
                    │    RFID Card     │
                    │    / RFID Tag    │
                    └────────┬─────────┘
                             │
                             ▼
                    ┌──────────────────┐
                    │      RC522       │
                    │   RFID Reader    │
                    └────────┬─────────┘
                             │ SPI
                             ▼
                    ┌──────────────────┐
                    │      ESP32       │
                    │   Controller     │
                    └────────┬─────────┘
                             │
             ┌───────────────┼───────────────┐
             │               │               │
             ▼               ▼               ▼
        ┌─────────┐     ┌─────────┐     ┌─────────┐
        │  OLED   │     │ Buzzer  │     │   LED   │
        │ SSD1306 │     │ GPIO 4  │     │ GPIO 2  │
        └─────────┘     └─────────┘     └─────────┘
                             │
                             │ Wi-Fi
                             ▼
                    ┌──────────────────┐
                    │     Make.com     │
                    │ Custom Webhook   │
                    └────────┬─────────┘
                             │
                             ▼
                    ┌──────────────────┐
                    │  Google Sheets   │
                    │ Attendance Data  │
                    └──────────────────┘
```

---

# 🔄 Attendance Workflow

The attendance logic works independently for each RFID card.

### First scan

```text
RFID Card
    ↓
Read UID
    ↓
CLOCK IN
    ↓
1 Beep
1 LED Blink
    ↓
Send Data to Make.com
    ↓
Google Sheets
```

### Second scan

```text
RFID Card
    ↓
Read UID
    ↓
CLOCK OUT
    ↓
2 Beeps
2 LED Blinks
    ↓
Send Data to Make.com
    ↓
Google Sheets
```

The same card continues alternating:

```text
CLOCK IN
     ↓
CLOCK OUT
     ↓
CLOCK IN
     ↓
CLOCK OUT
     ↓
CLOCK IN
     ↓
CLOCK OUT
     ↓
...
```

---

# 👥 Multiple RFID Cards

Each RFID UID maintains its own state.

For example:

```text
Card A → CLOCK IN
Card B → CLOCK IN
Card A → CLOCK OUT
Card C → CLOCK IN
Card B → CLOCK OUT
Card A → CLOCK IN
```

The cards do not interfere with each other.

---

# 📡 Make.com Integration

The ESP32 sends attendance information to a **Make.com Custom Webhook** using an HTTP POST request.

The payload is sent as JSON.

### Example CLOCK IN payload

```json
{
  "uid": "A1B2C3D4",
  "name": "RFID User",
  "event_type": "CLOCK_IN",
  "device": "ESP32-RFID-01"
}
```

### Example CLOCK OUT payload

```json
{
  "uid": "A1B2C3D4",
  "name": "RFID User",
  "event_type": "CLOCK_OUT",
  "device": "ESP32-RFID-01"
}
```

---

# 🔗 Make.com Scenario

The recommended Make.com automation is:

```text
Custom Webhook
       │
       ▼
Set Date / Time
       │
       ▼
    Router
    /     \
   /       \
  ▼         ▼
CLOCK IN   CLOCK OUT
  │         │
  ▼         ▼
Google     Google
Sheets     Sheets
  │         │
  ▼         ▼
 Gmail     Gmail
```

The system is designed so that:

> **Every RFID scan = one new attendance event.**

There is no duplicate filtering or Data Store requirement.

---

# 📊 Google Sheets

Attendance data can be stored using the following columns:

| Column | Description |
|---|---|
| Timestamp | Complete date and time |
| Date | Attendance date |
| Time | Attendance time |
| UID | RFID card UID |
| Name | RFID user name |
| Event Type | CLOCK_IN / CLOCK_OUT |
| Device | ESP32 device name |
| Status | SUCCESS / FAILED |

### Example

| Timestamp | Date | Time | UID | Name | Event Type | Device | Status |
|---|---|---|---|---|---|---|---|
| 2026-10-06 09:10:25 | 2026-10-06 | 09:10:25 | A1B2C3D4 | RFID User | CLOCK_IN | ESP32-RFID-01 | SUCCESS |
| 2026-10-06 17:05:11 | 2026-10-06 | 17:05:11 | A1B2C3D4 | RFID User | CLOCK_OUT | ESP32-RFID-01 | SUCCESS |

---

# 🔌 Hardware Components

| Component | Quantity |
|---|---:|
| ESP32 Dev Module | 1 |
| RC522 RFID Reader | 1 |
| RFID Card / Tag | 1 or more |
| SSD1306 OLED 128×64 | 1 |
| Buzzer Module | 1 |
| Jumper Wires | As required |
| USB Cable | 1 |

---

# 📍 Pin Configuration

## RC522 → ESP32

| RC522 Pin | ESP32 GPIO |
|---|---:|
| SDA / SS | GPIO 5 |
| RST | GPIO 27 |
| SCK | GPIO 18 |
| MISO | GPIO 19 |
| MOSI | GPIO 23 |
| VCC | 3.3V |
| GND | GND |
| IRQ | Not Connected |

### SPI Configuration

```text
RC522
  │
  ├── SDA / SS ─── GPIO 5
  ├── SCK ──────── GPIO 18
  ├── MISO ─────── GPIO 19
  ├── MOSI ─────── GPIO 23
  └── RST ──────── GPIO 27
```

---

# 🖥️ OLED → ESP32

The project uses an SSD1306 128×64 OLED display with I2C communication.

| OLED Pin | ESP32 |
|---|---:|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### OLED I2C Address

```text
0x3C
```

### I2C Configuration

```text
OLED
  │
  ├── SDA ─── GPIO 21
  └── SCL ─── GPIO 22
```

---

# 🔊 Buzzer → ESP32

| Buzzer Pin | ESP32 |
|---|---:|
| VCC | 3.3V |
| GND | GND |
| I/O | GPIO 4 |

The buzzer provides audio feedback during RFID attendance events.

---

# 💡 LED

The ESP32 onboard LED is used for visual feedback.

```text
Onboard LED → GPIO 2
```

No external LED is required.

---

# 🔔 Attendance Feedback

## CLOCK IN

```text
1 × Buzzer Beep
1 × LED Blink
```

## CLOCK OUT

```text
2 × Buzzer Beep
2 × LED Blinks
```

The OLED also displays the attendance status.

---

# 🎵 Buzzer Beat Demonstration

The project also contains a buzzer and LED beat demonstration.

The buzzer uses different frequencies to create musical beats.

Example frequencies used by the project include:

```text
LOW   = 330 Hz
MID   = 392 Hz
HIGH  = 494 Hz
HIGH2 = 587 Hz
HIGH3 = 659 Hz
```

The LED synchronizes with the buzzer beats.

---

# 💻 Software

The project is developed using:

- Arduino IDE
- C++
- ESP32 Arduino Core
- Make.com
- Google Sheets

---

# 📦 Required Arduino Libraries

Install the following libraries from the Arduino Library Manager:

### Adafruit GFX Library

Used for graphics rendering on the OLED.

### Adafruit SSD1306

Used to control the SSD1306 OLED display.

### Adafruit BusIO

Dependency for Adafruit display libraries.

### MFRC522

Used to communicate with the RC522 RFID reader.

### ArduinoJson

Used to create JSON payloads for Make.com.

---

# 📚 ESP32 Built-in Libraries

The following libraries are provided through the ESP32 Arduino package:

```text
WiFi
HTTPClient
SPI
Wire
```

---

# ⚙️ Configuration

Before uploading the firmware, configure:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MAKE_WEBHOOK_URL = "YOUR_MAKE_WEBHOOK_URL";
```

Replace these values with your own credentials locally.

---

# 🔐 Security

**Never commit secrets to a public GitHub repository.**

Do not publish:

```text
Wi-Fi passwords
Make.com webhook URLs
API keys
Access tokens
Private credentials
```

For local development, use a separate configuration file such as:

```text
config.h
```

and add it to `.gitignore`.

Example:

```cpp
#pragma once

const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
const char* MAKE_WEBHOOK_URL = "YOUR_WEBHOOK";
```

---

# 💾 RFID State Storage

The current implementation keeps RFID card attendance states in ESP32 RAM.

This means the state exists while the ESP32 is powered and running.

For example:

```text
Card A → CLOCK IN
Card A → CLOCK OUT
Card A → CLOCK IN
```

After restarting the ESP32, the in-memory states are reset.

Persistent state storage can be added in a future version using:

- ESP32 Preferences
- NVS
- EEPROM
- Database storage

---

# 🚫 No Duplicate Filtering

This project intentionally does not use duplicate filtering.

If the same RFID card is scanned six times:

```text
Scan 1 → CLOCK IN
Scan 2 → CLOCK OUT
Scan 3 → CLOCK IN
Scan 4 → CLOCK OUT
Scan 5 → CLOCK IN
Scan 6 → CLOCK OUT
```

All six events are sent to Make.com.

All six events should create separate Google Sheets rows.

---

# 🌐 Data Flow

```text
RFID Card
    │
    ▼
RC522
    │
    ▼
ESP32
    │
    ├── Determine UID
    │
    ├── Determine CLOCK IN / OUT
    │
    ├── Update OLED
    │
    ├── Activate Buzzer
    │
    ├── Blink LED
    │
    ▼
Wi-Fi
    │
    ▼
Make.com Webhook
    │
    ▼
Google Sheets
    │
    ▼
Attendance Record
```

---

# 🛠️ Installation

## 1. Install Arduino IDE

Download and install Arduino IDE.

## 2. Install ESP32 Board Support

Add ESP32 board support to Arduino IDE.

## 3. Install Required Libraries

Install:

```text
Adafruit GFX Library
Adafruit SSD1306
Adafruit BusIO
MFRC522
ArduinoJson
```

## 4. Connect the Hardware

Follow the wiring configuration described in:

```text
docs/wiring.md
```

## 5. Configure Wi-Fi

Update the local Wi-Fi configuration.

## 6. Configure Make.com

Create a Make.com Custom Webhook and use its URL in the local configuration.

## 7. Select ESP32 Board

In Arduino IDE select the appropriate ESP32 board, for example:

```text
ESP32 Dev Module
```

## 8. Select Port

Select the USB serial port connected to the ESP32.

## 9. Upload

Compile and upload the firmware.

## 10. Test RFID

Scan an RFID card.

The first scan should generate:

```text
CLOCK IN
```

The next scan should generate:

```text
CLOCK OUT
```

---

# 🧪 Testing

A basic test sequence is:

```text
1. Power ON ESP32
2. Connect to Wi-Fi
3. Scan RFID Card
4. Verify CLOCK IN
5. Check OLED
6. Check buzzer
7. Check LED
8. Check Make.com webhook
9. Check Google Sheets
10. Scan the same card again
11. Verify CLOCK OUT
12. Check Google Sheets again
```

---

# 🐛 Troubleshooting

## RFID is not detected

Check:

```text
RC522 VCC → 3.3V
RC522 GND → GND
SDA / SS → GPIO 5
SCK → GPIO 18
MISO → GPIO 19
MOSI → GPIO 23
RST → GPIO 27
```

---

## OLED is blank

Check:

```text
OLED VCC → 3.3V
OLED GND → GND
OLED SDA → GPIO 21
OLED SCL → GPIO 22
```

Also verify the OLED I2C address:

```text
0x3C
```

---

## Buzzer does not work

Check:

```text
Buzzer VCC → 3.3V
Buzzer GND → GND
Buzzer I/O → GPIO 4
```

---

## ESP32 does not connect to Make.com

Check:

```text
Wi-Fi connection
Webhook URL
Internet connection
Make.com scenario status
Webhook configuration
```

---

## Google Sheets is not updated

Verify the Make.com scenario:

```text
Custom Webhook
       ↓
Data Processing
       ↓
Google Sheets
```

Make sure the Google Sheets module is configured correctly.

---

# 📁 Project Structure

```text
esp32-rfid-attendance/
│
├── src/
│   └── esp32-rfid-attendance.ino
│
├── docs/
│   └── wiring.md
│
├── README.md
│
└── .gitignore
```

---

# 🔮 Future Improvements

Possible future versions can include:

- 👤 Student name registration
- 🪪 RFID card registration system
- 🗄️ Persistent RFID state storage
- 📊 Attendance dashboard
- 📱 Mobile application
- 🌐 Web-based admin dashboard
- 📈 Attendance analytics
- 📧 Automated attendance reports
- 📲 WhatsApp notifications
- 🤖 AI-based attendance analytics
- ☁️ Cloud database
- 🏫 Multi-classroom support
- 📡 Multiple ESP32 devices
- 🔄 Offline attendance queue
- ⏱️ NTP-based time synchronization
- 🔒 Admin authentication
- 👨‍🎓 Student profile management

---

# 📸 Project Demo

### Live attendance data in Google Sheets

![TagIT - Live attendance data in Google Sheets](docs/images/tagit-google-sheets-showcase.png)

### Make.com automation workflow

![TagIT - Make.com automation workflow](docs/images/tagit-make-workflow-showcase.png)

---

# 🎥 Demo Video

Add your project demonstration video here.

```text
Coming soon...
```

---

# 📜 Project Status

```text
🟢 Active Development
```

Current version includes:

```text
✔ ESP32
✔ RC522 RFID
✔ OLED Display
✔ Buzzer
✔ LED Feedback
✔ Wi-Fi
✔ Make.com
✔ Google Sheets
✔ Multi-card support
✔ CLOCK IN / CLOCK OUT
✔ Unlimited scan events
```

---

# 👨‍💻 Author

## Sumit Suresh Jagtap

Computer Science & Engineering student and AI/ML developer interested in:

- Artificial Intelligence
- Machine Learning
- Generative AI
- IoT
- Full-Stack Development
- Automation
- AI Agents

### GitHub

https://github.com/Sumit-tech01

---

# ⭐ Support

If you find this project useful, consider giving the repository a ⭐ on GitHub.

---

# 📄 License

This project is intended for educational, experimental, and development purposes.

You are free to modify and improve the project according to your requirements.

---

## 🚀 Built With

```text
ESP32
+
RC522 RFID
+
SSD1306 OLED
+
Arduino
+
C++
+
Wi-Fi
+
Make.com
+
Google Sheets
```

> **ESP32 RFID Attendance System — Scan. Record. Automate.**