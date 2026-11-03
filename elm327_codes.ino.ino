#include <Arduino.h>
#include "ELMduino.h"

// Use HardwareSerial 2 on the ESP32-S3
HardwareSerial ELMSerial(2); 
ELM327 myELM327;

// Variables to store sensor values
float rpm = 0;
int32_t mphOrKphSpeed = 0;
float coolantTemp = 0;

void setup() 
{
  // Initialize native USB Serial for debugging monitor output
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor to open

  Serial.println("Starting ESP32-S3 OBD2 Controller...");

  // Initialize Serial2 for communication with the ELM327 device
  // Pins: RX = GPIO 18, TX = GPIO 17 (Adjust if using different pins)
  ELMSerial.begin(38400, SERIAL_8N1, 18, 17);

  // Attempt to establish communication with ELM327 chip
  Serial.println("Connecting to ELM327 chip...");
  
  if (!myELM327.begin(ELMSerial, true, 2000)) 
  {
    Serial.println("Error: Couldn't connect to ELM327. Check wiring/baud rate.");
    while (1); // Halt execution if connection fails
  }
  
  Serial.println("Connected to ELM327 successfully.");
}

void loop() 
{
  // 1. Read Engine RPM (OBD PID: 01 0C)
  float tempRPM = myELM327.rpm();
  if (myELM327.nb_rx_state == ELM_SUCCESS) 
  {
    rpm = tempRPM;
    Serial.print("RPM: ");
    Serial.println(rpm);
  } 
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG) 
  {
    myELM327.printError();
  }

  delay(100); // Small stability delay between queries

  // 2. Read Vehicle Speed (OBD PID: 01 0D)
  int32_t tempSpeed = myELM327.kph();
  if (myELM327.nb_rx_state == ELM_SUCCESS) 
  {
    mphOrKphSpeed = tempSpeed;
    Serial.print("Speed: ");
    Serial.print(mphOrKphSpeed);
    Serial.println(" km/h");
  } 
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG) 
  {
    myELM327.printError();
  }

  delay(100);

  // 3. Read Engine Coolant Temperature (OBD PID: 01 05)
  float tempECT = myELM327.engineCoolantTemp();
  if (myELM327.nb_rx_state == ELM_SUCCESS) 
  {
    coolantTemp = tempECT;
    Serial.print("Coolant Temp: ");
    Serial.print(coolantTemp);
    Serial.println(" C");
  } 
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG) 
  {
    myELM327.printError();
  }

  Serial.println("---------------------------");
  delay(1000); // Query the bike once every second
}
