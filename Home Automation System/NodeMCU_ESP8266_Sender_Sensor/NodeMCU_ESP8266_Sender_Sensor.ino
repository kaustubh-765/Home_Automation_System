#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <ThingSpeak.h>

unsigned long Channel_ID = 2140111;            // Channel ID
const char *WriteAPIKey = "MKVNHS4CCUSL3QNZ";  // Your write API Key
long delayStart = millis();

#define DHTPIN D2          // DHT sensor pin
#define DHTTYPE DHT22      // DHT sensor type
#define LDRPIN A0          // LDR sensor pin
#define smokeSensorPin D7  //Smoke Sensor pin

//Auth code for Blynk App
char auth[] = "DuFkmj-mQZUPJUywZW0u7PsLeLwd1OfT";
//Setting up SSID and Password of Wifi
const char* ssid = "iPhone";
const char* pass = "helloWorld098";
//Providing the IP of the Server
const IPAddress receiver_ip(172, 20, 10, 2);
WiFiClient client;

//Threshold value for sensor to make decisions
const float threshold_low_light = 100;          
const float threshold_high_light = 700;
const float threshold_low_humidity = 30;
const float threshold_high_humidity = 80;
const float threshold_low_temperature = 18;
const float threshold_high_temperature = 40;
const float threshold_smoke_sensor = 1024;

//Activating the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);         //For Serial Connection to Serial Monitor
  WiFi.disconnect();
  delay(2000);
  Serial.print("Start Connection");     //Starting the fresh Wifi Connection
  pinMode(smokeSensorPin, INPUT);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  // Connect to WiFi network
  
  Blynk.begin(auth, ssid, pass);      //Starting the Blynk App Connection
  dht.begin();                        //initializing the DHT sensor for Readings
  ThingSpeak.begin(client);
}

void loop() {

  Blynk.run();
  float humidity = dht.readHumidity();    // Read humidity value from DHT sensor
  float temperature = dht.readTemperature();   // Read temperature value from DHT sensor
  int ldr_value = analogRead(LDRPIN);         // Read LDR sensor value
  int smokeVal = analogRead(smokeSensorPin); //Reading Smoke Sensor Value
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Light Intensity: ");
  Serial.println(ldr_value);
  Serial.print("Smoke Sensor: ");
  Serial.println(smokeVal);

  Blynk.virtualWrite(V0, humidity);            // Send humidity value to Blynk app
  Blynk.virtualWrite(V1, temperature);         // Send temperature value to Blynk app
  Blynk.virtualWrite(V2, ldr_value);           // Send LDR value to Blynk app

   int ledStatus = 0;
  //Conditional Statement for Threshold value
  if((humidity >  threshold_low_humidity && humidity<  threshold_high_humidity ) && (temperature> threshold_low_temperature && temperature < threshold_high_temperature) && (ldr_value > threshold_low_light && ldr_value < threshold_high_light))
    {
      ledStatus = 1; // Value to set the value Window status
    }
    else
    {
      ledStatus = 0; // Value to set the value Window status
    }

    if (smokeVal >= threshold_smoke_sensor){
      ledStatus = 2;
    }

    int data = ledStatus;
    // Sending Data to the NodeMCU
    Serial.println(data);
    
    //Sending data to the nearby node
    if (client.connect(receiver_ip, 80)) {
      client.print(data);  // Send sensor value to receiver NodeMCU
      client.stop();
    }

    //Uploading the Values to the thingspeak Server
    if ((millis() - delayStart) >= 45000) {
    ThingSpeak.writeField(Channel_ID, 1, ldr_value, WriteAPIKey);
    delayStart = millis();
  }
   else if ((millis() - delayStart) >= 30000) {
     ThingSpeak.writeField(Channel_ID, 2, temperature, WriteAPIKey);
  }
   else if ((millis() - delayStart) >= 15000) {
    ThingSpeak.writeField(Channel_ID, 3, humidity, WriteAPIKey);
  }
}
