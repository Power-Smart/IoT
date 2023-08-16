#include <ESP8266WiFi.h>
//#include <WebSocketClient.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include <Hash.h>

#define USE_SERIAL Serial

const char* ssid     = "SANJEEWA";
const char* password = "680850402";
char path[] = "/";
//IPAddress host(20,246,228,95);/
char host[] = "20.246.228.95";
char wsHost[] = "20.246.228.95";


StaticJsonDocument<200> doc;

int unit_id = 69420;
String initial_data = "{\"realyId\":\"69420\"}";

WebSocketsClient webSocket;



//WebSocketClient webSocketClient;

// Use WiFiClient class to create TCP connections
WiFiClient client;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

      // send message to server when Connected
      webSocket.sendTXT(initial_data);
    }
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[WSc] get text: %s\n", payload);

      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
        case WStype_PING:
            // pong will be send automatically
            USE_SERIAL.printf("[WSc] get ping\n");
            break;
        case WStype_PONG:
            // answer to a ping we send
            USE_SERIAL.printf("[WSc] get pong\n");
            break;
    }
}

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  delay(10);

  for(int t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

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

  webSocket.begin(wsHost,4000, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  webSocket.enableHeartbeat(15000, 3000, 2);

  

// /  Connect to the websocket server
//  if (client.connect(host, 4001)) {
//    Serial.println("Connected");
//  } else {
//    Serial.println("Connection failed.");
//    while(1) {
//      // Hang on failure
//    }
//  }

  // Handshake with the server
//  webSocketClient.path = path;
//  webSocketClient.host = wsHost;
//  if (webSocketClient.handshake(client)) {
//    Serial.println("Handshake successful");
//
//    doc["relayId"]= unit_id;
//
//    serializeJson(doc, initial_data);
//
//    webSocketClient.sendData(initial_data);
//    
//  } else {
//    Serial.println("Handshake failed.");
//    while(1) {
//      // Hang on failure
//    }  
//  }

}


void loop() {
//  String data;

//  if (client.connected()) {
//    
//    webSocketClient.getData(data);
//    if (data.length() > 0) {
//      Serial.print("Received data: ");
//      Serial.println(data);
//    }
//    
//    // capture the value of analog 1, send it along
////    pinMode(1, INPUT);
////    data = String(analogRead(1));
//
//    data = "Success!";
//    
//    webSocketClient.sendData(data);
//    
//  } else {
//    Serial.println("Client disconnected.");
//    while (1) {
//      // Hang on disconnect.
//    }
//  }
  
  // wait to fully let the client disconnect
  webSocket.loop();
  
}
