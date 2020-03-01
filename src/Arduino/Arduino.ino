/********************************************************************/
// First we include the libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Libraries/Button.h"
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 3
#define buttonPin 9
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
/********************************************************************/
Button button;
int mode = 0; // modes 0-battery status, 1-sampling, 2-hive statuses.
static long millisBetweenSamples = 5000;
unsigned long currentMillis, lastTempsMillis;
float maxTemps[5] = {0,0,0,0,0};
float minTemps[5] = {200,200,200,200,200};
static float maxBroodTempF = 96;
static float minBroodTempF = 89.5;
bool statusLEDs[5] = {0,0,0,0,0}; // 0 is red, 1 is green. TODO

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("One Wire Hive temp sensors");
  // Start up the library
  sensors.begin();
  sensors.setResolution(9);

  button.setup(buttonPin, [](int state) {
    if (state == 1) {
      Serial.print(millis());
      Serial.println(", button pressed");
    }
  });
}
void loop(void)
{
  currentMillis = millis();
  if ((currentMillis - lastTempsMillis) >= millisBetweenSamples){
    currentMillis = currentMillis-(currentMillis%100); //prevents any ms creep.
    sensors.requestTemperatures(); // Send the command to get temperature readings
    /********************************************************************/
    Serial.print(currentMillis/1000);
    Serial.print(", ");
    Serial.print(sensors.getTempFByIndex(0),1); //
    float temp;
    for (int i = 1; i <= 2; i++) {
      Serial.print(", ");
      temp = sensors.getTempFByIndex(i);
      if (temp < minTemps[i]) minTemps[i] = temp;
      if (temp > maxTemps[i]) maxTemps[i] = temp;
      if (temp == DEVICE_DISCONNECTED_F){
        Serial.print("NA");
      } else{
        Serial.print(temp,1); //
      }
    }
    Serial.println();
    lastTempsMillis = currentMillis;
  }

  button.update();
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  /********************************************************************/

}
