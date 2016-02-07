#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

// Change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int sdCardChipSelectPin = 10;

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

File logFile;

bool pressureSensorPresent = false; 
bool sdCardPresent = false;

//Vairables for log count 
unsigned int logCountMaximum = 42;
unsigned int currentLogCount = 1;

void setup() {

  Serial.begin(19200);
  Serial.println("Pressure Sensor Logger");
  Serial.println("");

  // Initialize the BMP-180 pressure sensor
  if (!bmp.begin()) {
    Serial.print("No BMP-180 pressure sensor was detected!");
  } else {    
    Serial.print("BMP-180 pressure sensor was found");
    pressureSensorPresent = true;  
  }
  Serial.println("");

  Serial.println("Initializing SD card...");
  pinMode(sdCardChipSelectPin, OUTPUT);   
  if (!SD.begin(sdCardChipSelectPin)) {
    Serial.println("NO SD card was detected!");
    Serial.println("");
  } else {
    Serial.println("SD card was found.");
    Serial.println("");
    sdCardPresent = true;  

    if (SD.exists("pressure.log")) {
      Serial.println("File pressure.log will be removed!");    
      SD.remove("pressure.log");
      Serial.println("DONE");
      Serial.println("");      
    }
    
    logFile = SD.open("pressure.log", FILE_WRITE);
  }  
}

void loop() {
  if (pressureSensorPresent) {        

    //Log until we reach the maximum log count number
    if (currentLogCount <= logCountMaximum) {
      retrieveAndLogSensorData();  
    }    

    //Wait for 2 seconds
    delay(2000);    
  }
}

void retrieveAndLogSensorData() {

  sensors_event_t event;
  bmp.getEvent(&event);

  if (event.pressure) {
    float pressure;
    float temperature;
    float altitude;

    // Retrieve the pressure in milliBar
    pressure = event.pressure;
    
    // Retrieve the temperature and altitude in celsius and meter
    bmp.getTemperature(&temperature);
    altitude = bmp.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA, pressure);

    Serial.println("Sensor data:");
    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println(" milliBar");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" celsius");

    Serial.print("Altitude: ");
    Serial.print(altitude);
    Serial.println(" meter");      
    Serial.println("");

    if (sdCardPresent && logFile) {
      
      Serial.print("Log #");
      Serial.print(currentLogCount);
      Serial.println(" to the SD card:");

      logFile.print(currentLogCount, 10);  
      logFile.print(", "); 
      logFile.print(pressure, 10);  
      logFile.print(", ");  
      logFile.print(temperature, 10);  
      logFile.print(", ");  
      logFile.println(altitude, 10);  
      logFile.flush();    
      Serial.println("DONE");
      Serial.println("");      

      //Close the log file when we reach the log count maximum
      if (currentLogCount == logCountMaximum) {
        logFile.close();
        Serial.println("The pressure.log file was closed.");
      }

      //Increase the log count by one
      currentLogCount++;
    }
  }
}
