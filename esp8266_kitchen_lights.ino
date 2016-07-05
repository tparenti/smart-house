/*
 *  To control and monitor the kitchen cabinet lights
 */

#include <ESP8266WiFi.h>

const char* ssid = "N900";
const char* password = "Inter1!2";

int ledPin = 0;      // choose the pin for the LED
int inputPin = 2;    // choose the input pin (for PIR sensor)
int motion = 0;      // variable for reading the pin status
int led = 0;         // led status variable

// Create an instance of the server with port 80
WiFiServer server(80);

void setup() {
  //Serial.begin(115200);
  delay(10);

  //set pwm pulse
  analogWriteFreq(200);
  
  // prepare led pin
  pinMode(ledPin, OUTPUT);

  // prepare motion sensor pin
  pinMode(inputPin, INPUT);
  
  // Connect to WiFi network
  //Serial.println();
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.hostname("KitchenLights");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  //Serial.println("Server started");

  // Print the IP address
  //Serial.println(WiFi.localIP());
}

void loop() {
  delay(2000);
  motion = digitalRead(inputPin);  // read input value for motion
  if (motion == HIGH) {            // check if the input is HIGH

    //fade in led strip
    if (led == 0) {
        for (int fadeValue = 1 ; fadeValue <= 1023; fadeValue += 5) {
        analogWrite(ledPin,fadeValue);
        //Serial.println("Trying to fade in");
        //Serial.println(ledPin);
        //Serial.println(fadeValue);
        // wait for 15 milliseconds to see the dimming effect
        delay(15);  
        }
    }
    analogWrite(ledPin, 1023);
    led = 1;  //  led status on
    //Serial.println("Human located");
  } else {
    if (led == 1) {
        for (int fadeValue = 1023 ; fadeValue >= 0; fadeValue -= 5) {
        analogWrite(ledPin,fadeValue);
        //Serial.println("Trying to fade out");
       // Serial.println(ledPin);
        //Serial.println(fadeValue);
        // wait for 15 milliseconds to see the dimming effect
        delay(15);  
        }
    }
    analogWrite(ledPin, 0);
    led = 0; //  led status off
    //Serial.println("Nothing Here");
  }
    
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  //Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  //Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/lightoff") != -1)
    val = 0;
  else if (req.indexOf("/lighton") != -1)
    val = 1;
  else {
    //Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(ledPin, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val)?"high":"low";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  //Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

