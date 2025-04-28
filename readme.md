# ESP32 Fan Temperature Controller

This project uses an **ESP32**, a **DS18B20** temperature sensor, and a **PWM-controlled fan** to maintain optimal device or environment temperatures.  
The fan speed is dynamically adjusted based on the measured temperature, ranging from off at low temperatures to full speed when the temperature exceeds a defined limit.

## Features
- Accurate temperature sensing with the DS18B20 sensor.
- Smooth fan speed control via PWM (25kHz, 8-bit resolution).
- Adjustable temperature thresholds, hysteresis, and fan speed limits.
- Serial monitor output for real-time temperature and fan status updates.

## Hardware Requirements
- ESP32 Development Board
- DS18B20 Temperature Sensor
- 4.7kΩ Pull-up Resistor (for DS18B20 data line)
- PWM-Compatible DC Fan
- Connecting wires

## Pin Configuration
| Device        | ESP32 Pin |
| ------------- | --------- |
| DS18B20 Data  | GPIO 4     |
| Fan PWM Control | GPIO 16  |

## Configuration Settings
- `TEMP_MIN` : Fan starts when temperature exceeds 20°C.
- `TEMP_LIMIT` : Fan runs at full speed above 28°C.
- `TEMP_HYSTERESIS` : 2°C hysteresis to avoid frequent fan toggling.
- `FAN_MIN_DUTY` : Minimum fan speed duty cycle (20% duty).
- `FAN_MAX_DUTY` : Maximum fan speed (100% duty).

## Libraries Used
- [OneWire](https://www.arduino.cc/reference/en/libraries/onewire/)
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library)

## Installation
1. Install the required libraries via Arduino IDE Library Manager.
2. Connect the hardware components as per the pin configuration.
3. Upload the provided code to the ESP32 board.
4. Open the Serial Monitor to observe temperature and fan status.

## Usage
- The system automatically reads the temperature every 2 seconds.
- Fan speed dynamically changes based on the current temperature.
- Useful for temperature-based fan control in projects like electronics cooling, incubators, and small environmental chambers.

## License
This project is released under the MIT License.

