#include<WiFi.h>
#include <WiFiManager.h> 
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      webSocket.sendTXT(num, "Connected");
    }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      Serial.println((char *) payload);
      // lcd.clear();
      // lcd.print((char *) payload);

      if(payload[0] =='1') { // Compare the value of payload with a character
        // we get RGB data
        // uint16_t color = (uint16_t) strtol((const char *) &payload[1], NULL, 16);
        digitalWrite(19, HIGH); // Replace digitawirite with digitalWrite
        digitalWrite(18, HIGH);
        lcd.clear();
        lcd.print("Position False");
      }
      else if(payload[0] =='0') {
        digitalWrite(19, LOW); // Replace digitawirite with digitalWrite
        digitalWrite(18, LOW);
        lcd.clear();
        lcd.print("Position True");
      }
      else
      {
        lcd.clear();
        lcd.print((char *) payload);
      }
      break;
  }
}
void setup() {
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.print("Waiting...");
  Serial.begin(9600);
  pinMode(19, OUTPUT);
  pinMode(18, OUTPUT);
  digitalWrite(19, LOW);
  digitalWrite(18, LOW);
  WiFi.mode(WIFI_AP_STA);
  WiFiManager wifiManager;
  if (!wifiManager.startConfigPortal("MotionDetectorAP")) {
    Serial.println("NO");
    Serial.println("Restarting...");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  IPAddress localIP = WiFi.localIP(); // Add this line to declare the localIP variable
  lcd.clear();
  lcd.print(localIP);
  // delay(10000);
  Serial.println(localIP);
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Not found");
  });
  server.begin();
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

}

void loop() { 
  // lcd.clear();
  // lcd.print("Undetected");
  webSocket.loop();
}
