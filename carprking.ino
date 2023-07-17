#include <SPI.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
char ssid[] = "your ssid";
char password[] = " your Password";
const char* server = "your server address";
const int port = 80;                        
const int numSpaces = 4;
bool occupied[numSpaces];
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Smart Parking");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.print("Connected to");
  lcd.setCursor(0, 1);
  lcd.print("Wi-Fi");
  delay(2000);
  for (int i = 0; i < numSpaces; i++) {
    occupied[i] = false;
  }
}
void loop() {
  for (int i = 0; i < numSpaces; i++) {
    int sensorValue = digitalRead(i); 
    bool isOccupied = (sensorValue == HIGH);
    if (isOccupied != occupied[i]) {
      occupied[i] = isOccupied;
      updateServer(i, occupied[i]);
    }
  }
  delay(1000);
}
void updateServer(int space, bool isOccupied) {
  WiFiClient client;
  if (!client.connect(server, port)) {
    Serial.println("Connection to server failed");
    return;
  }
  String url = "/update?space=" + String(space) + "&occupied=" + String(isOccupied);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  client.stop();
}
