#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <dht11.h>
#include <BH1750.h>
#include <ArduinoJson.h>
#include <CO2Sensor.h>
#include <ESP8266WebServer.h>

#define DHT11_PIN 0

#define BH1750_SDA 4
#define BH1750_SCL 5

#define ANALOG_PIN A0

#define PIR_PIN 13

#define INTERRUPT_BUTTON 2

long currentMillis = 0;

long period = 60000;

//const int led = LED_BUILTIN;

const char* ssidAP = "NodeMCUPostTest";
const char* passwordAP = "12345678";

IPAddress local_ip(192,168,2,1);
IPAddress gateway(192,168,2,1);
IPAddress subnet(255,255,255,0);

// Replace with your network credentials
const char* ssid     = "Redmi 9";
const char* password = "pppppppp";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "https://testing-server-4grg.vercel.app/api";

float lux,hum,temp,co2;
boolean pir_state;
String requestData;
volatile boolean server_mode = false;

//std:: string credentialsArr[2];

ESP8266WebServer server(80);

const String postForms = "<html>\
  <head>\
    <title>Access Point Credentials</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Enter WiFi Access Point Credentials</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
      <lable>Wifi Ssid</lable>\
      <input type=\"text\" name=\"ssid\" value=\"\"><br>\
      <lable>Wifi Password</lable>\
      <input type=\"password\" name=\"pw\" value=\"\"><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </body>\
</html>";

void handleRoot() {
  server.send(200, "text/html", postForms);
}

void handleForm() {
  if (server.method() != HTTP_POST) {
//    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
//    digitalWrite(led, 0);
  } else {
//    digitalWrite(led, 1);
    String message = "POST form was:\n";
    for (uint8_t i = 0; i < server.args(); i++) { 
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
      if(server.argName(i) == "ssid"){
//        credentialsArr[0] = server.arg(i);
          ssid = server.arg(i).c_str();
        }
       if(server.argName(i) == "pw"){
//        credentialsArr[1] = server.arg(i);
          password = server.arg(i).c_str();
        }
      }
    Serial.print(message);
    server.send(200, "text/plain", message);
//    digitalWrite(led, 0);
    WiFi.softAPdisconnect(true);
    server_mode = false;
    delay(100);
    Serial.println("HTTP Server Stopped");
  }
}

void handleNotFound() {
//  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);
//  digitalWrite(led, 0);
}


dht11 DHT11;
BH1750 lightMeter;

CO2Sensor co2Sensor(A0, 0.99, 100);


StaticJsonDocument<200> doc;

ICACHE_RAM_ATTR void setServerMode() {
 if(server_mode == false){
    server_mode = true;
    Serial.println("server mode on");
  }
}


void setup() {
  
  Serial.begin(9600);

  Wire.begin();
  lightMeter.begin();
  co2Sensor.calibrate();

  pinMode(PIR_PIN, INPUT); 
  pinMode(INTERRUPT_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_BUTTON), setServerMode, CHANGE);
  
//  WiFi.begin(ssid, password);
//  Serial.println("Connecting");
//  while(WiFi.status() != WL_CONNECTED) { 
//    delay(500);
//    Serial.print(".");
//  }
//  Serial.println("");
//  Serial.print("Connected to WiFi network with IP Address: ");
//  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.on("/postform/", handleForm);

  server.onNotFound(handleNotFound);

}

void loop() {

  if(server_mode){

    if(WiFi.softAPIP() != local_ip){
      
        WiFi.softAP(ssidAP, passwordAP);
        WiFi.softAPConfig(local_ip, gateway, subnet);
        delay(100);
        server.begin();
        Serial.println("HTTP server started");
        
      }else{
        server.handleClient();
      }

//    Serial.println("Server Mode");
//    delay(5000);
//    server_mode = false;
    
    }else{

       //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){

      delay(10);

  //    int check_dht = ;

      if(DHT11.read(DHT11_PIN)){
        hum = (float)DHT11.humidity, 2;
        temp = (float)DHT11.temperature, 2;
        }
        
      delay(10);

//      if(lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE_2)){
//        
//        }  
       lux = lightMeter.readLightLevel();  

      delay(10);

      co2 = co2Sensor.read();

      delay(10);

      pir_state = digitalRead(PIR_PIN);

      delay(10);

      
      std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

      // Ignore SSL certificate validation
      client->setInsecure();
      
      //create an HTTPClient instance
      HTTPClient https;
      
      // Your Domain name with URL path or IP address with path
      Serial.println(https.begin(*client, serverName));
      
      // Specify content-type header
      https.addHeader("Content-Type", "application/json");
      
      // Prepare your HTTP POST request data
      doc["co2_level"] = co2;
      doc["humudity_level"] = hum;
      doc["temperature"] = temp;
      doc["light_intensity"] = lux;
      doc["pir_reading"] = pir_state;
      
      Serial.print("httpRequestData: ");
      serializeJsonPretty(doc, Serial);

      serializeJson(doc, requestData);
    
      // Send HTTP POST request
      int httpResponseCode = https.POST(requestData);   

          
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = https.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      Serial.println("");
      // Free resources
      https.end();
    }
    else {
//      Serial.println("WiFi Disconnected");
      WiFi.begin(ssid, password);
      Serial.println("Connecting...");
      currentMillis = millis();
      while(millis() - currentMillis < period && WiFi.status() != WL_CONNECTED) { 
        delay(500);
        Serial.print(".");
      }
      if(WiFi.status() == WL_CONNECTED){
        Serial.println("");
        Serial.print("Connected to WiFi network with IP Address: ");
        Serial.println(WiFi.localIP());
      }else{
        Serial.println("Retrying...");
      }
    }
    //Send an HTTP POST request every 30 seconds

    delay(3000);  
        
    }
 
}
