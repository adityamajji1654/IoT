// Define your Blynk template and authentication token
#define BLYNK_TEMPLATE_ID "TMPL3zm7IwGEy"
#define BLYNK_TEMPLATE_NAME "IoT Fan Controller"
#define BLYNK_AUTH_TOKEN "3TYeiALzpLZ9mx2jXT-nRJsGack-VcH5"

// Include necessary libraries
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Set the Blynk debugging output to the Serial monitor
#define BLYNK_PRINT Serial

// Define the pin for the OneWire sensor
#define ONE_WIRE_BUS 12

// Define the fan control pin, initial duty cycle, and temperature variables
int fanPin = 16;
int dutyCycle = 0;
float temp = 0;
int threshold = 30;

// Create an instance of the OneWire sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Create a widget for controlling the fan via Blynk app
WidgetLED FAN(V0);

// Define Wi-Fi credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Srinivas";
char pass[] = "srinu123";

// Setup function
void setup()
{
  // Initialize the Serial communication for debugging
  Serial.begin(115200);

  // Initialize the OneWire sensor and fan control pin
  sensors.begin();
  pinMode(fanPin, OUTPUT);

  // Set the range and frequency for PWM fan control
  analogWriteRange(100);
  analogWriteFreq(10000);

  // Connect to the Blynk server
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

// Blynk virtual write handler for changing the temperature threshold
BLYNK_WRITE(V7)
{
  threshold = param.asInt();
  Serial.print(" The Threshold value is: ");
  Serial.println(threshold);
  Serial.println();
}

// Function to control the fan speed based on a percentage
void controlFanSpeed(int fanSpeedPercent)
{
  analogWrite(fanPin, fanSpeedPercent);

  Serial.print("Fan Speed: ");
  Serial.print(fanSpeedPercent);
  Serial.println("%");
}

// Main loop
void loop()
{
  // Run the Blynk service
  Blynk.run();

  // Request temperature from the Dallas sensor
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("*C    ");
  Serial.print("Threshold: ");
  Serial.print(threshold);
  Serial.println("*C    ");
  // Send temperature data to Blynk app
  Blynk.virtualWrite(V3, temp);

  // Control the fan based on temperature and threshold
  if (temp >= threshold)
  {
    if(FAN.getValue()==0)
    {
      FAN.on();
      Blynk.logEvent("threshold_alert", String("Fan is On"));
    }
    int fanSpeedPercent = map(temp, threshold, 55, 10, 100);
    controlFanSpeed(fanSpeedPercent);
    Blynk.virtualWrite(V4, fanSpeedPercent);
  }
  else if (temp < threshold)
  {
    if(FAN.getValue()==255)
    {
      FAN.off();
      Blynk.logEvent("threshold_alert", String("Fan is Off"));
      int fanSpeedPercent = 0;
      controlFanSpeed(fanSpeedPercent);
      Blynk.virtualWrite(V4, fanSpeedPercent);
    }
  }

  // Add a delay to prevent rapid updates
  delay(1000);
}