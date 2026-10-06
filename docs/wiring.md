# 🔌 ESP32 RFID Attendance System — Wiring Guide

This document contains the complete hardware wiring for the ESP32 RFID Attendance System.

The project uses:

- ESP32 Dev Module
- RC522 RFID Reader
- SSD1306 128×64 OLED Display
- Buzzer Module
- ESP32 Onboard LED

---

# 🧩 Hardware Overview

```text
                    ┌─────────────────────┐
                    │        ESP32        │
                    │    Dev Module       │
                    │                     │
                    │ GPIO 2  ────────────┼──► Onboard LED
                    │ GPIO 4  ────────────┼──► Buzzer
                    │ GPIO 5  ────────────┼──► RC522 SS/SDA
                    │ GPIO 18 ────────────┼──► RC522 SCK
                    │ GPIO 19 ────────────┼──► RC522 MISO
                    │ GPIO 21 ────────────┼──► OLED SDA
                    │ GPIO 22 ────────────┼──► OLED SCL
                    │ GPIO 23 ────────────┼──► RC522 MOSI
                    │ GPIO 27 ────────────┼──► RC522 RST
                    │                     │
                    │ 3.3V ───────────────┼──► RC522 VCC
                    │ 3.3V ───────────────┼──► OLED VCC
                    │ 3.3V ───────────────┼──► Buzzer VCC
                    │                     │
                    │ GND ────────────────┼──► RC522 GND
                    │ GND ────────────────┼──► OLED GND
                    │ GND ────────────────┼──► Buzzer GND
                    └─────────────────────┘
```

---

# 1. 🔵 RC522 RFID Reader

The RC522 communicates with the ESP32 using the SPI interface.

## RC522 Pin Connections

| RC522 Pin | ESP32 Pin | Function |
|---|---|---|
| SDA / SS | GPIO 5 | SPI Chip Select |
| SCK | GPIO 18 | SPI Clock |
| MOSI | GPIO 23 | SPI Data Out |
| MISO | GPIO 19 | SPI Data In |
| RST | GPIO 27 | Reset |
| IRQ | Not Connected | Interrupt |
| GND | GND | Ground |
| 3.3V | 3.3V | Power |

## RC522 Wiring

```text
RC522                         ESP32
──────────────────────────────────────

SDA / SS  ──────────────────► GPIO 5

SCK       ──────────────────► GPIO 18

MOSI      ──────────────────► GPIO 23

MISO      ──────────────────► GPIO 19

RST       ──────────────────► GPIO 27

IRQ       ──────────────────► Not Connected

GND       ──────────────────► GND

3.3V      ──────────────────► 3.3V
```

### ⚠️ Important

The RC522 should be powered from **3.3V**.

```text
RC522 VCC → ESP32 3.3V
```

Do not power the RC522 from 5V.

---

# 2. 🖥️ SSD1306 OLED Display

The project uses a **128×64 SSD1306 OLED display** using I2C communication.

## OLED Pin Connections

| OLED Pin | ESP32 Pin | Function |
|---|---|---|
| VCC | 3.3V | Power |
| GND | GND | Ground |
| SDA | GPIO 21 | I2C Data |
| SCL | GPIO 22 | I2C Clock |

## OLED Wiring

```text
OLED                          ESP32
──────────────────────────────────────

VCC       ──────────────────► 3.3V

GND       ──────────────────► GND

SDA       ──────────────────► GPIO 21

SCL       ──────────────────► GPIO 22
```

### OLED I2C Address

The project uses:

```text
0x3C
```

### I2C Pins

```text
SDA → GPIO 21
SCL → GPIO 22
```

---

# 3. 🔊 Buzzer Module

The buzzer provides audio feedback during RFID scans.

## Buzzer Pin Connections

| Buzzer Pin | ESP32 Pin | Function |
|---|---|---|
| VCC | 3.3V | Power |
| GND | GND | Ground |
| I/O | GPIO 4 | Buzzer Signal |

## Buzzer Wiring

```text
Buzzer                        ESP32
──────────────────────────────────────

VCC       ──────────────────► 3.3V

GND       ──────────────────► GND

I/O       ──────────────────► GPIO 4
```

---

# 4. 💡 ESP32 Onboard LED

The project uses the ESP32's onboard LED.

No external LED is required.

```text
ESP32 GPIO 2
      │
      ▼
Onboard LED
```

### LED Pin

```text
GPIO 2
```

The LED is used for visual feedback during attendance events.

---

# 5. 🔌 Complete Pin Configuration

| Component | Pin | ESP32 GPIO |
|---|---|---:|
| ESP32 LED | LED | GPIO 2 |
| Buzzer | I/O | GPIO 4 |
| RC522 | SDA / SS | GPIO 5 |
| OLED | SDA | GPIO 21 |
| OLED | SCL | GPIO 22 |
| RC522 | SCK | GPIO 18 |
| RC522 | MISO | GPIO 19 |
| RC522 | MOSI | GPIO 23 |
| RC522 | RST | GPIO 27 |

---

# 6. ⚡ Power Connections

The modules share the ESP32 ground.

```text
ESP32 3.3V
    │
    ├──────────► RC522 VCC
    │
    ├──────────► OLED VCC
    │
    └──────────► Buzzer VCC


ESP32 GND
    │
    ├──────────► RC522 GND
    │
    ├──────────► OLED GND
    │
    └──────────► Buzzer GND
```

### Common Ground

All modules must share a common ground:

```text
RC522 GND
OLED GND
Buzzer GND
      │
      ▼
ESP32 GND
```

---

# 7. 🔗 Complete Wiring Table

| Module | Module Pin | ESP32 |
|---|---|---|
| RC522 | SDA / SS | GPIO 5 |
| RC522 | SCK | GPIO 18 |
| RC522 | MOSI | GPIO 23 |
| RC522 | MISO | GPIO 19 |
| RC522 | RST | GPIO 27 |
| RC522 | 3.3V | 3.3V |
| RC522 | GND | GND |
| OLED | SDA | GPIO 21 |
| OLED | SCL | GPIO 22 |
| OLED | VCC | 3.3V |
| OLED | GND | GND |
| Buzzer | I/O | GPIO 4 |
| Buzzer | VCC | 3.3V |
| Buzzer | GND | GND |
| ESP32 | Onboard LED | GPIO 2 |

---

# 8. 🧠 Communication Interfaces

The project uses two communication interfaces.

## SPI — RC522

The RC522 uses SPI.

```text
SPI
│
├── SCK  → GPIO 18
├── MISO → GPIO 19
├── MOSI → GPIO 23
└── SS   → GPIO 5
```

Reset:

```text
RST → GPIO 27
```

---

## I2C — OLED

The OLED uses I2C.

```text
I2C
│
├── SDA → GPIO 21
└── SCL → GPIO 22
```

OLED address:

```text
0x3C
```

---

# 9. 🔄 Attendance Hardware Flow

```text
               RFID CARD
                   │
                   ▼
            ┌─────────────┐
            │    RC522    │
            │ RFID Reader │
            └──────┬──────┘
                   │
                   │ SPI
                   ▼
            ┌─────────────┐
            │    ESP32    │
            │ Controller  │
            └──────┬──────┘
                   │
          ┌────────┼─────────┐
          │        │         │
          ▼        ▼         ▼
       ┌─────┐  ┌──────┐  ┌─────┐
       │OLED │  │Buzzer│  │ LED │
       └─────┘  └──────┘  └─────┘
                   │
                   │ Wi-Fi
                   ▼
             ┌───────────┐
             │ Make.com  │
             └─────┬─────┘
                   │
                   ▼
             ┌───────────┐
             │  Google   │
             │  Sheets   │
             └───────────┘
```

---

# 10. 🔔 CLOCK IN Feedback

When an RFID card is scanned and its next state is `CLOCK_IN`:

```text
RFID Scan
    │
    ▼
CLOCK IN
    │
    ├──► OLED displays attendance status
    │
    ├──► 1 Buzzer Beep
    │
    ├──► 1 LED Blink
    │
    └──► Send event to Make.com
```

---

# 11. 🔔 CLOCK OUT Feedback

When the same RFID card is scanned again:

```text
RFID Scan
    │
    ▼
CLOCK OUT
    │
    ├──► OLED displays attendance status
    │
    ├──► 2 Buzzer Beeps
    │
    ├──► 2 LED Blinks
    │
    └──► Send event to Make.com
```

---

# 12. 🔄 Multiple RFID Cards

Each RFID card maintains its own state.

Example:

```text
Card A → CLOCK IN
Card B → CLOCK IN
Card A → CLOCK OUT
Card C → CLOCK IN
Card B → CLOCK OUT
Card A → CLOCK IN
```

The state of one card does not affect another card.

---

# 13. ♾️ Repeated Scanning

The same card can be scanned repeatedly.

Example:

```text
Scan 1 → CLOCK IN
Scan 2 → CLOCK OUT
Scan 3 → CLOCK IN
Scan 4 → CLOCK OUT
Scan 5 → CLOCK IN
Scan 6 → CLOCK OUT
```

Every scan is treated as a new attendance event.

---

# 14. 📡 Internet Data Flow

After an RFID scan:

```text
RFID
  │
  ▼
ESP32
  │
  ▼
Wi-Fi
  │
  ▼
Make.com Webhook
  │
  ▼
Google Sheets
```

The ESP32 sends data using an HTTP POST request.

Example JSON:

```json
{
  "uid": "A1B2C3D4",
  "name": "RFID User",
  "event_type": "CLOCK_IN",
  "device": "ESP32-RFID-01"
}
```

---

# 15. 🛡️ Wiring Safety Checklist

Before powering the project, verify:

- [ ] RC522 VCC is connected to 3.3V
- [ ] RC522 GND is connected to GND
- [ ] RC522 SS/SDA is connected to GPIO 5
- [ ] RC522 SCK is connected to GPIO 18
- [ ] RC522 MISO is connected to GPIO 19
- [ ] RC522 MOSI is connected to GPIO 23
- [ ] RC522 RST is connected to GPIO 27
- [ ] OLED VCC is connected correctly
- [ ] OLED GND is connected to GND
- [ ] OLED SDA is connected to GPIO 21
- [ ] OLED SCL is connected to GPIO 22
- [ ] Buzzer signal is connected to GPIO 4
- [ ] Buzzer GND is connected to GND
- [ ] All modules share a common ground

---

# 16. ⚠️ Important Notes

### RC522 Voltage

The RC522 used in this project is connected to:

```text
3.3V
```

Do not connect its VCC to 5V.

### Common Ground

Make sure all modules have a common ground with the ESP32.

### SPI Pins

Do not accidentally swap:

```text
MOSI
MISO
SCK
SS
```

### OLED Pins

Make sure:

```text
SDA → GPIO 21
SCL → GPIO 22
```

---

# 17. 📋 Quick Reference

```text
┌────────────────────────────────────────┐
│         ESP32 RFID ATTENDANCE          │
├────────────────────────────────────────┤
│                                        │
│ LED             → GPIO 2               │
│ Buzzer          → GPIO 4               │
│ RC522 SS        → GPIO 5               │
│ RC522 SCK       → GPIO 18              │
│ RC522 MISO      → GPIO 19              │
│ OLED SDA        → GPIO 21              │
│ OLED SCL        → GPIO 22              │
│ RC522 MOSI      → GPIO 23              │
│ RC522 RST       → GPIO 27              │
│                                        │
│ RC522 VCC       → 3.3V                 │
│ OLED VCC        → 3.3V                 │
│ Buzzer VCC      → 3.3V                 │
│                                        │
│ All GND         → ESP32 GND            │
│                                        │
└────────────────────────────────────────┘
```

---

# 👨‍💻 Author

**Sumit Suresh Jagtap**

GitHub:

https://github.com/Sumit-tech01

---

# 📁 Related Files

Main firmware:

```text
src/esp32-rfid-attendance.ino
```

Project documentation:

```text
README.md
```

Wiring documentation:

```text
docs/wiring.md
```

---

> **ESP32 RFID Attendance System**
>
> **Scan → Detect → Record → Automate**