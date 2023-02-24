#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Set your network credentials
const char* ssid = "SIREN";
const char* password = "12345678";
unsigned long lastOnTime=0;
unsigned long currentTime=0;
int autoOffMinutes = 1 ; //set minutes for auto off

// Create an instance of the server
ESP8266WebServer server(80);

// Set the pin number for controlling
const int pinD7 = D7;

void setup() {
  Serial.begin(9600);
  // Initialize the pin as an output
  pinMode(pinD7, OUTPUT);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print the IP address
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
}

void loop() {
  server.handleClient();
  //turn off after autoOffMinutes minute  
  if(digitalRead(pinD7) == HIGH){
    currentTime=millis();
    if(currentTime-lastOnTime>=60000*autoOffMinutes){
      digitalWrite(pinD7, LOW);
    } 
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html><html lang='en'><head>    <meta charset='UTF-8'>    <meta http-equiv='X-UA-Compatible' content='IE=edge'>    <meta name='viewport' content='width=device-width, initial-scale=1.0'>    <title>Control Siren</title></head><body>    <link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css' rel='stylesheet'        crossorigin='anonymous'>    <div class='container mt-5 text-center'>        <h4>Control Siren</h4>        <p class='my-5'>Current status: ";
  if (digitalRead(pinD7) == HIGH) {
    html += "ON</p>";
  } else {
    html += "OFF</p>";
  }
  html += "<a class='btn btn-outline-primary px-5' href='on'>Turn ON</a><br><a class='btn btn-outline-primary px-5 mt-4' href='off'>Turn OFF</a><p id='lastUpdate' class='text-muted text-small mt-5'></p><p>Note: Siren will be auto turned off after 1 minute</p><b>Developer: satishkushwahdigital@gmail.com</b>";
  html+= "</div><script>document.getElementById('lastUpdate').innerText = 'Last update: ' + new Date().toLocaleTimeString();</script></body></html>";

  server.send(200, "text/html", html);
}

void handleOn() {
  digitalWrite(pinD7, HIGH);
  lastOnTime=millis();
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleOff() {
  digitalWrite(pinD7, LOW);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
} 
