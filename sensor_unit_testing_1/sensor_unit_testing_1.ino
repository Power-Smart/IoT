#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <dht11.h>
#include <BH1750.h>
#include <ArduinoJson.h>
#include <CO2Sensor.h>

#define DHT11_PIN 2

#define BH1750_SDA 4
#define BH1750_SCL 5

#define ANALOG_PIN A0

#define PIR_PIN 13

//#define INTERRUPT_BUTTON 0


// Replace with your network credentials
const char* ssid     = "SANJEEWA";
const char* password = "680850402";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "https://testing-server-4grg.vercel.app/api";

float lux,hum,temp,co2;
boolean pir_state;
//volatile boolean server_mode = false;



dht11 DHT11;
BH1750 lightMeter;

CO2Sensor co2Sensor(A0, 0.99, 100);


StaticJsonDocument<200> doc;

//ICACHE_RAM_ATTR void setServerMode() {
// if(server_mode == false){
//    server_mode = true;
//  }
//}


void setup() {
  
  Serial.begin(9600);

  Wire.begin();
  lightMeter.begin();
  co2Sensor.calibrate();

  pinMode(PIR_PIN, INPUT); 
//  pinMode(INTERRUPT_BUTTON, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(INTERRUPT_BUTTON), setServerMode, CHANGE);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}

void loop() {

       //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){

      delay(10);

  //    int check_dht = ;

      if(DHT11.read(DHT11_PIN)){
        hum = (float)DHT11.humidity, 2;
        temp = (float)DHT11.temperature, 2;
        }
        
      delay(10);

      if(lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE_2)){
        lux = lightMeter.readLightLevel();
        }    

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
      doc["temp"] = temp;
      doc["hum"] = hum;
      doc["lux"] = lux;
      doc["co2"] = co2;
      doc["pir"] = pir_state;
      
      Serial.print("httpRequestData: ");
      serializeJsonPretty(doc, Serial);
      String requestData;
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
      Serial.println("WiFi Disconnected");
    }
    //Send an HTTP POST request every 30 seconds

    delay(3000);   
}
