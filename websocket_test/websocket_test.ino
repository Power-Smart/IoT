#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
#include <ArduinoJson.h>

const char* ssid     = "SANJEEWA";
const char* password = "680850402";
char path[] = "/";
//IPAddress host(20,246,228,95);/
char host[] = "20.246.228.95";
char wsHost[] = "ws://20.246.228.95";


StaticJsonDocument<200> doc;
int unit_id = 69420;
String initial_data;

WebSocketClient webSocketClient;

// Use WiFiClient class to create TCP connections
WiFiClient client;

void setup() {
  Serial.begin(9600);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(5000);
  

// /  Connect to the websocket server
  if (client.connect(host, 4001)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    while(1) {
      // Hang on failure
    }
  }

  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = wsHost;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");

    doc["relayId"]= unit_id;

    serializeJson(doc, initial_data);

    webSocketClient.sendData(initial_data);
    
  } else {
    Serial.println("Handshake failed.");
    while(1) {
      // Hang on failure
    }  
  }

}


void loop() {
  String data;

  if (client.connected()) {
    
    webSocketClient.getData(data);
    if (data.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(data);
    }
    
    // capture the value of analog 1, send it along
//    pinMode(1, INPUT);
//    data = String(analogRead(1));

    data = "Success!";
    
    webSocketClient.sendData(data);
    
  } else {
    Serial.println("Client disconnected.");
    while (1) {
      // Hang on disconnect.
    }
  }
  
  // wait to fully let the client disconnect
  delay(3000);
  
}
