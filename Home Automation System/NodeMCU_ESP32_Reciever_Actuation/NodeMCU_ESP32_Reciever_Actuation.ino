#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

//Setting up SSID and Password of Wifi
const char* ssid = "iPhone";
const char* password = "helloWorld098";
//Auth code for Blynk App
char auth[] = "DuFkmj-mQZUPJUywZW0u7PsLeLwd1OfT";

//Led is connected to PIN 19
const int ledPin_1 = 19;
const int ledPin_2 = 23;
const int ledPin_3 = 18;
const int ledPin_4 = 5;
const int buzzerPin = 22;
const int motorPin = 15;
int buttonState;

//Object for Creating a Wifi - Server
WiFiServer server(80);
Servo mt;
int testing_smoke = 0;

void setup() {
  Serial.begin(9600);                //For Serial Connection to serial Monitor
  pinMode(ledPin_1, OUTPUT);           //Setting PIN as output
  pinMode(ledPin_2, OUTPUT);           //Setting PIN as output
  pinMode(ledPin_3, OUTPUT);           //Setting PIN as output
  pinMode(ledPin_4, OUTPUT);           //Setting PIN as output 
  
  pinMode(buzzerPin, OUTPUT);           //Setting PIN as output

  mt.attach(motorPin);

  Serial.print("Connecting to ");     //Connecting to Wifi
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");      //Showing IP Address of Connection
  Serial.println(WiFi.localIP());       

  server.begin();                       //Starting the Server

  Blynk.begin(auth, ssid, password);    //Starting the Blynk App Connection
  Blynk.syncVirtual(V3, V4, V5);            //Syncing Virtual Pins
}

BLYNK_CONNECTED() {
    Blynk.syncAll();
}

BLYNK_WRITE(V3) // this function gets called whenever the button widget is turned on or off
{
  buttonState = param.asInt();
}
BLYNK_WRITE(V5) // this function gets called whenever the button widget is turned on or off
{
  testing_smoke = param.asInt();
}

void loop() {

  
  //Checking if any client is available on network
  WiFiClient client = server.available();
  if (client) {
    Blynk.run();                  //Running the Blynk App Client
    int ledStatus;
    while (client.connected()) {  //Checking for client Connection
      if (client.available()) {
        int c = client.read();    //Reading the Data Sent by Client
          ledStatus = c;          //Stroing Value for Automatic Window Operation            
      }
    }
    
    Serial.println(ledStatus); // Print received sensor value to serial monitor
    Serial.println(buttonState);
    Serial.println(testing_smoke);

    //Performing the Actuation - Decision Making
    if (buttonState == 1) // if the button is turned on
    {
    if (ledStatus == 49){ //Checking the Value for 
      noTone(buzzerPin);
      digitalWrite(ledPin_1, LOW); // Open the Window 
      Blynk.virtualWrite(V4, "Window Open");           // Send Window Status to Blynk app
        mt.write(180);    // rotate the servo to 0 degrees
        delay(1000);
        mt.write(0);         // wait for 1 second
    }
    else if (ledStatus == 48) {
      digitalWrite(ledPin_1, HIGH); // Close the Window
      Blynk.virtualWrite(V4, "Window Close");           // Send Window Status to Blynk app
      noTone(buzzerPin);
        mt.write(90);    // rotate the servo to 0 degrees
        delay(1000);         // wait for 1 second
    }
    }
    else // if the button is turned off
    {
      Blynk.virtualWrite(V4, "Window Close");           // Send Window Status to Blynk app
      digitalWrite(ledPin_1, HIGH); // Close the Window
    }

    //if (ledStatus == 50){
    if (testing_smoke == 1){
      digitalWrite(ledPin_2, HIGH); // Open the Window
      digitalWrite(ledPin_3, HIGH); // Open the Window
      digitalWrite(ledPin_4, HIGH); // Open the Window 
      Blynk.virtualWrite(V4, "Emergency Window Open");           // Send Window Status to Blynk app
      tone(buzzerPin, 1000);
      delay(100);
        mt.write(180);    // rotate the servo to 0 degrees
        delay(1000);         // wait for 1 second
    }
    else{
      digitalWrite(ledPin_2, LOW); // Open the Window
      digitalWrite(ledPin_3, LOW); // Open the Window 
      digitalWrite(ledPin_4, LOW); // Open the Window 

      //Blynk.virtualWrite(V4, "Auto Window Open Activate");           // Send Window Status to Blynk app
      //mt.write(90);
      noTone(buzzerPin);
    }
    
    //Closing the Connections
    client.stop();
  }
}