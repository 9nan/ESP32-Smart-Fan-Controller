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

// Error handling
#define MAX_TEMP_READ_ERRORS 5
#define SENSOR_TIMEOUT 5000

// Initialize OneWire and DallasTemperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// State variables
unsigned long lastTempCheckTime = 0;
float currentTemperature = 0.0;
float previousTemperature = 0.0;
int currentFanSpeed = 0;
int errorCount = 0;
bool fanState = false;

// Moving average filter for temperature smoothing
#define FILTER_SIZE 5
float tempReadings[FILTER_SIZE] = {0};
int readingIndex = 0;
bool filterInitialized = false;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n=================================");
  Serial.println("ESP32 Fan Temperature Controller");
  Serial.println("=================================");
  
  // Initialize temperature sensor
  sensors.begin();
  
  // Check if sensor is connected
  if (sensors.getDeviceCount() == 0) {
    Serial.println("ERROR: No DS18B20 sensors found!");
    Serial.println("Please check wiring and connections.");
  } else {
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount());
    Serial.println(" temperature sensor(s)");
  }
  
  // Set sensor resolution for faster readings
  sensors.setResolution(10); // 10-bit resolution (0.25°C accuracy)
  
  // Configure PWM for fan control
  ledcSetup(FAN_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(FAN_PWM_PIN, FAN_PWM_CHANNEL);
  
  // Start with fan off
  setFanSpeed(0);
  
  // Display configuration
  Serial.println("\nConfiguration:");
  Serial.print("  Min Temp: ");
  Serial.print(TEMP_MIN);
  Serial.println("°C");
  Serial.print("  Max Temp: ");
  Serial.print(TEMP_LIMIT);
  Serial.println("°C");
  Serial.print("  Hysteresis: ");
  Serial.print(TEMP_HYSTERESIS);
  Serial.println("°C");
  Serial.print("  Check Interval: ");
  Serial.print(TEMP_CHECK_INTERVAL);
  Serial.println("ms");
  Serial.println("=================================\n");
}

void loop() {
  // Check temperature at regular intervals
  if (millis() - lastTempCheckTime >= TEMP_CHECK_INTERVAL) {
    lastTempCheckTime = millis();
    
    // Read and process temperature
    if (readTemperature()) {
      // Apply smoothing filter
      float smoothedTemp = applyTemperatureFilter(currentTemperature);
      
      // Control fan speed based on smoothed temperature
      adjustFanSpeed(smoothedTemp);
      
      // Reset error count on successful read
      errorCount = 0;
    } else {
      handleSensorError();
    }
  }
}

bool readTemperature() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  
  // Validate temperature reading
  if (temp != DEVICE_DISCONNECTED_C && temp > -50 && temp < 125) {
    previousTemperature = currentTemperature;
    currentTemperature = temp;
    return true;
  }
  
  return false;
}

float applyTemperatureFilter(float newTemp) {
  // Add new reading to circular buffer
  tempReadings[readingIndex] = newTemp;
  readingIndex = (readingIndex + 1) % FILTER_SIZE;
  
  // Mark filter as initialized after first full cycle
  if (readingIndex == 0) {
    filterInitialized = true;
  }
  
  // Calculate average
  float sum = 0;
  int count = filterInitialized ? FILTER_SIZE : readingIndex;
  
  for (int i = 0; i < count; i++) {
    sum += tempReadings[i];
  }
  
  return sum / count;
}

void adjustFanSpeed(float temperature) {
  int targetFanSpeed = 0;
  String status = "";
  
  if (temperature < TEMP_MIN) {
    // Below minimum temperature - fan off
    targetFanSpeed = 0;
    status = "OFF (below minimum)";
  } 
  else if (temperature >= TEMP_LIMIT + TEMP_HYSTERESIS) {
    // Above temperature limit with hysteresis - fan at maximum
    targetFanSpeed = FAN_MAX_DUTY;
    status = "MAXIMUM (above limit!)";
  } 
  else if (temperature > TEMP_LIMIT && fanState) {
    // Apply hysteresis when cooling down
    if (temperature < TEMP_LIMIT - TEMP_HYSTERESIS) {
      // Temperature dropped below hysteresis threshold
      float tempRange = TEMP_LIMIT - TEMP_MIN;
      float tempRatio = (temperature - TEMP_MIN) / tempRange;
      targetFanSpeed = FAN_MIN_DUTY + tempRatio * (FAN_MAX_DUTY - FAN_MIN_DUTY);
      status = "Variable";
    } else {
      targetFanSpeed = FAN_MAX_DUTY;
      status = "MAXIMUM (hysteresis)";
    }
  }
  else {
    // Map temperature to fan speed between minimum and maximum
    float tempRange = TEMP_LIMIT - TEMP_MIN;
    float tempRatio = (temperature - TEMP_MIN) / tempRange;
    targetFanSpeed = FAN_MIN_DUTY + tempRatio * (FAN_MAX_DUTY - FAN_MIN_DUTY);
    
    status = "Variable";
  }
  
  // Only update fan if speed changed significantly (reduce PWM noise)
  if (abs(targetFanSpeed - currentFanSpeed) > 5 || 
      (targetFanSpeed == 0 && currentFanSpeed != 0) ||
      (targetFanSpeed > 0 && currentFanSpeed == 0)) {
    
    setFanSpeed(targetFanSpeed);
    currentFanSpeed = targetFanSpeed;
    fanState = (targetFanSpeed > 0);
    
    // Print status update
    Serial.print("Temp: ");
    Serial.print(temperature, 1);
    Serial.print("°C | Fan: ");
    Serial.print(status);
    Serial.print(" | Speed: ");
    Serial.print(currentFanSpeed);
    Serial.print("/");
    Serial.print(FAN_MAX_DUTY);
    Serial.print(" (");
    Serial.print((currentFanSpeed * 100) / FAN_MAX_DUTY);
    Serial.println("%)");
  }
}

void setFanSpeed(int speed) {
  // Constrain speed to valid range
  speed = constrain(speed, 0, FAN_MAX_DUTY);
  ledcWrite(FAN_PWM_CHANNEL, speed);
}

void handleSensorError() {
  errorCount++;
  
  Serial.print("ERROR: Failed to read temperature sensor! (Error count: ");
  Serial.print(errorCount);
  Serial.println(")");
  
  if (errorCount >= MAX_TEMP_READ_ERRORS) {
    Serial.println("WARNING: Multiple sensor read failures detected!");
    Serial.println("Setting fan to MAXIMUM speed as safety precaution.");
    setFanSpeed(FAN_MAX_DUTY);
    currentFanSpeed = FAN_MAX_DUTY;
    fanState = true;
  }
}
