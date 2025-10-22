# ESP32 Smart Fan Temperature Controller

An intelligent temperature-based fan controller using **ESP32**, **DS18B20** sensor, and **PWM control** for optimal thermal management. Features smooth speed transitions, temperature filtering, and fail-safe operation.

## ✨ Key Features

- **Accurate Temperature Sensing**: DS18B20 digital sensor with 0.25°C precision
- **Smooth PWM Control**: 25kHz frequency for silent operation
- **Temperature Filtering**: Moving average filter eliminates noise and prevents erratic fan behavior
- **Hysteresis Control**: Prevents rapid fan on/off cycling
- **Fail-Safe Mode**: Automatically runs fan at maximum if sensor fails
- **Real-Time Monitoring**: Detailed serial output with temperature, fan speed, and status
- **Efficient Updates**: Only adjusts fan speed when necessary to reduce PWM switching

## 🔧 Hardware Requirements

| Component | Description |
|-----------|-------------|
| **ESP32 Development Board** | Any ESP32 board with GPIO support |
| **DS18B20 Temperature Sensor** | Digital one-wire temperature sensor |
| **4.7kΩ Pull-up Resistor** | Required for DS18B20 data line |
| **PWM-Compatible DC Fan** | 12V or 5V DC fan (3-pin or 4-pin) |
| **MOSFET/Transistor** | For fan control (e.g., IRLZ44N) |
| **Power Supply** | Appropriate for your fan voltage |
| **Connecting Wires** | Jumper wires for connections |

## 📐 Wiring Diagram

### DS18B20 Temperature Sensor
```
DS18B20          ESP32
---------        -----
VCC      →       3.3V
GND      →       GND
DATA     →       GPIO 4 (with 4.7kΩ pull-up to 3.3V)
```

### Fan Control (via MOSFET)
```
ESP32 GPIO 16  →  MOSFET Gate (through 220Ω resistor)
MOSFET Drain   →  Fan Negative (-)
MOSFET Source  →  GND
Fan Positive   →  Power Supply (+)
```

## 📊 Configuration Settings

| Parameter | Default | Description |
|-----------|---------|-------------|
| `TEMP_MIN` | 20°C | Fan starts running above this temperature |
| `TEMP_LIMIT` | 28°C | Fan runs at full speed above this temperature |
| `TEMP_HYSTERESIS` | 2°C | Prevents rapid on/off cycling |
| `FAN_MIN_DUTY` | 50 (20%) | Minimum fan speed when active |
| `FAN_MAX_DUTY` | 255 (100%) | Maximum fan speed |
| `TEMP_CHECK_INTERVAL` | 2000ms | Temperature measurement frequency |
| `PWM_FREQUENCY` | 25kHz | PWM frequency for silent operation |

## 📚 Required Libraries

Install via Arduino IDE Library Manager:

1. **OneWire** by Paul Stoffregen
   - `Sketch → Include Library → Manage Libraries → Search "OneWire"`

2. **DallasTemperature** by Miles Burton
   - `Sketch → Include Library → Manage Libraries → Search "DallasTemperature"`

## 🚀 Installation

1. **Install Arduino IDE** (or PlatformIO)
2. **Install ESP32 Board Support**:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" and install

3. **Install Required Libraries** (see above)

4. **Wire Your Hardware** according to the wiring diagram

5. **Upload the Code**:
   - Select your ESP32 board: Tools → Board → ESP32 Dev Module
   - Select the correct port: Tools → Port
   - Click Upload

6. **Open Serial Monitor**: Tools → Serial Monitor (115200 baud)

## 💡 How It Works

### Temperature-to-Speed Mapping

- **Below 20°C**: Fan OFF
- **20°C - 28°C**: Fan speed increases linearly from 20% to 100%
- **Above 28°C**: Fan at 100% (maximum cooling)

### Smart Features

1. **Moving Average Filter**: Smooths temperature readings over 5 samples to eliminate sensor noise
2. **Hysteresis Control**: Prevents fan from rapidly turning on/off near threshold temperatures
3. **Fail-Safe Mode**: If sensor fails after 5 consecutive errors, fan runs at maximum speed
4. **Efficient PWM**: Only updates fan speed when change is significant (>2% difference)

## 📈 Serial Monitor Output

```
=================================
ESP32 Fan Temperature Controller
=================================
Found 1 temperature sensor(s)

Configuration:
  Min Temp: 20.0°C
  Max Temp: 28.0°C
  Hysteresis: 2.0°C
  Check Interval: 2000ms
=================================

Temp: 22.3°C | Fan: Variable | Speed: 88/255 (34%)
Temp: 23.1°C | Fan: Variable | Speed: 108/255 (42%)
Temp: 28.5°C | Fan: MAXIMUM (above limit!) | Speed: 255/255 (100%)
```

## 🎯 Use Cases

- **Electronics Cooling**: Raspberry Pi, NAS drives, amplifiers
- **3D Printer Enclosures**: Temperature-controlled ventilation
- **Small Server Racks**: Quiet, efficient cooling
- **Incubators**: Precise temperature maintenance
- **DIY Climate Control**: Greenhouses, pet habitats
- **PC/Laptop Cooling**: External cooling pad controller

## 🔄 Customization Tips

### Change Temperature Range
```cpp
#define TEMP_MIN 25.0    // Start fan at 25°C
#define TEMP_LIMIT 35.0  // Max speed at 35°C
```

### Adjust Fan Speed Range
```cpp
#define FAN_MIN_DUTY 100  // Higher minimum speed (40%)
#define FAN_MAX_DUTY 230  // Lower maximum speed (90%)
```

### More Aggressive Cooling
```cpp
#define TEMP_HYSTERESIS 1.0  // Faster response
#define TEMP_CHECK_INTERVAL 1000  // Check every second
```

## 🐛 Troubleshooting

| Problem | Solution |
|---------|----------|
| "No DS18B20 sensors found" | Check wiring, verify 4.7kΩ pull-up resistor is connected |
| Fan doesn't spin | Verify MOSFET connections, check fan power supply |
| Temperature reads -127°C | Sensor disconnected or faulty wiring |
| Fan stutters | Ensure PWM_FREQUENCY is 25kHz, check power supply capacity |
| Erratic fan behavior | Normal - the filter will stabilize after 5 readings (~10 seconds) |

## 📄 License

This project is released under the **MIT License**.

```
MIT License - Free to use, modify, and distribute
```

## 🤝 Contributing

Improvements and suggestions are welcome! Feel free to fork and submit pull requests.

## 📧 Support

For issues and questions, please open an issue on the project repository.

---

**Made with ❤️ for the ESP32 Community**
