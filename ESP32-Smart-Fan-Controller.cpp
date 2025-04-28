#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Definitions
#define ONE_WIRE_BUS 4       // DS18B20 temperature sensor data pin
#define FAN_PWM_PIN 16       // PWM pin for fan control
#define FAN_PWM_CHANNEL 0    // ESP32 PWM channel

// Temperature settings
#define TEMP_LIMIT 28.0      // Temperature limit in degrees Celsius
#define TEMP_HYSTERESIS 2.0  // Hysteresis to prevent fan oscillation
#define TEMP_MIN 20.0        // Minimum temperature where fan starts running
#define TEMP_CHECK_INTERVAL 2000 // Temperature check interval in milliseconds

// Fan settings
#define PWM_FREQUENCY 25000  // PWM frequency for fan (25kHz)
#define PWM_RESOLUTION 8     // 8-bit resolution (0-255)
#define FAN_MIN_DUTY 50      // Minimum fan duty cycle (0-255)
#define FAN_MAX_DUTY 255     // Maximum fan duty cycle (0-255)

// Initialize OneWire and DallasTemperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

unsigned long lastTempCheckTime = 0;
float currentTemperature = 0.0;

void setup() {
  Serial.begin(115200);
  
  // Initialize temperature sensor
  sensors.begin();
  
  // Configure PWM for fan control
  ledcSetup(FAN_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(FAN_PWM_PIN, FAN_PWM_CHANNEL);
  
  // Start with fan off
  ledcWrite(FAN_PWM_CHANNEL, 0);
  
  Serial.println("ESP32 Fan Temperature Controller");
  Serial.print("Temperature Limit: ");
  Serial.print(TEMP_LIMIT);
  Serial.println(" °C");
}

void loop() {
  // Check temperature at regular intervals
  if (millis() - lastTempCheckTime >= TEMP_CHECK_INTERVAL) {
    lastTempCheckTime = millis();
    
    // Read temperature
    sensors.requestTemperatures();
    currentTemperature = sensors.getTempCByIndex(0);
    
    // Check if temperature reading is valid
    if (currentTemperature != DEVICE_DISCONNECTED_C) {
      Serial.print("Current Temperature: ");
      Serial.print(currentTemperature);
      Serial.println(" °C");
      
      // Control fan speed based on temperature
      adjustFanSpeed(currentTemperature);
    } else {
      Serial.println("Error reading temperature sensor!");
    }
  }
}

void adjustFanSpeed(float temperature) {
  int fanSpeed = 0;
  
  if (temperature < TEMP_MIN) {
    // Below minimum temperature - fan off
    fanSpeed = 0;
    Serial.println("Fan OFF - Temperature below minimum");
  } 
  else if (temperature > TEMP_LIMIT) {
    // Above temperature limit - fan at maximum
    fanSpeed = FAN_MAX_DUTY;
    Serial.println("Fan at MAXIMUM - Temperature above limit!");
  } 
  else {
    // Map temperature to fan speed between minimum and maximum
    float tempRange = TEMP_LIMIT - TEMP_MIN;
    float tempRatio = (temperature - TEMP_MIN) / tempRange;
    fanSpeed = FAN_MIN_DUTY + tempRatio * (FAN_MAX_DUTY - FAN_MIN_DUTY);
    
    Serial.print("Fan Speed: ");
    Serial.print(fanSpeed);
    Serial.print("/");
    Serial.println(FAN_MAX_DUTY);
  }
  
  // Set fan speed
  ledcWrite(FAN_PWM_CHANNEL, fanSpeed);
}