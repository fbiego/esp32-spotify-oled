#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <OLED_I2C.h>
#include <ArduinoJson.h>

char * WIFI_SSID = "__WIFI_SSID__";
char * WIFI_PASS = "__WIFI_PASSWORD__";

String client_id = "__YOUR_CLIENT_ID__";
String redirect = "http%3A%2F%2Fhttpbin.org%2Fanything";  // redirect link -> http://httpbin.org/anything
String cookie = "__YOUR_COOKIE_HERE__";

long current = 0, duration = 0;
int volume;
String artists = "", title, album, device, type;
bool isActive, playing;

bool isExpired = false;
String token = "";

WiFiMulti wifiMulti;
OLED  myOLED(21, 22); //(SDA, SCL)

extern uint8_t SmallFont[], MediumNumbers[];

void setup() {
  Serial.begin(115200);
  Serial.println("Starting device");
  wifiMulti.addAP(WIFI_SSID, WIFI_PASS);

  if (!myOLED.begin(SSD1306_128X64)) {
    while (1);  // In case the library failed to allocate enough RAM for the display buffer...
  }
  myOLED.setFont(SmallFont);

  Serial.print("Waiting for WiFi to connect...");

  myOLED.clrScr();
  myOLED.print("Connecting...", LEFT, 12);
  myOLED.update();

  while ((wifiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println("Connected");
  myOLED.clrScr();
  myOLED.print("Connected", LEFT, 12);
  myOLED.update();

  getToken();

}

void loop() {

  if (isExpired) {
    getToken();
  }

  getPlayer();
  updateScreen();

}

void updateScreen() {
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print(title, LEFT, 0);
  myOLED.print(artists, LEFT, 12);
  myOLED.print(album, LEFT, 24);
  myOLED.print(device, LEFT, 36);
  myOLED.print(String(volume), RIGHT, 36);
  myOLED.drawRect(0, 48, 127, 52);
  int pr = map(current, 0, duration, 0, 127);
  myOLED.drawRectFill(0, 48, pr, 52);
  myOLED.print(playing ? ">" : "||", LEFT, 56);
  //  int mC = current / 60000;
  //  int sC = (current % 60000) / 1000;
  //  myOLED.print(String(mC) + ":" + (sC < 10 ? "0" : "") + String(sC), 64, 56);
  int m = duration / 60000;
  int s = (duration % 60000) / 1000;
  myOLED.print(String(m) + ":" + (s < 10 ? "0" : "") + String(s), RIGHT, 56);
  myOLED.update();
}



void getToken() {
  HTTPClient http;
  String url = "https://accounts.spotify.com/authorize?response_type=token&redirect_uri=" + redirect + "&client_id=" + client_id + "&scope=user-read-playback-state+user-read-playback-position+user-modify-playback-state&state=cryq3";
  Serial.println(url);
  http.begin(url);
  http.addHeader("Cookie", cookie);
  int httpCode = http.GET();

  String payload = http.getLocation();
  token = payload.substring(payload.indexOf("access_token=") + 13, payload.indexOf("&token_type"));
  Serial.print("Token: ");
  Serial.println(token);

  http.end();
}


void getPlayer() {
  HTTPClient http;
  http.begin("https://api.spotify.com/v1/me/player");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  int httpCode = http.GET();
  String payload = http.getString();
  //Serial.println(payload);
  if (httpCode == HTTP_CODE_OK) {
    DynamicJsonDocument json(20000);
    deserializeJson(json, payload);

    title = json["item"]["name"].as<String>();
    album = json["item"]["album"]["name"].as<String>();
    artists = "";
    JsonArray arr = json["item"]["artists"].as<JsonArray>();
    for (JsonVariant value : arr) {
      artists += value["name"].as<String>() + " ";
    }

    current = json["progress_ms"].as<long>();
    duration = json["item"]["duration_ms"].as<long>();
    playing = json["is_playing"].as<bool>();

    device = json["device"]["name"].as<String>();
    isActive = json["device"]["is_active"].as<bool>();
    type = json["device"]["type"].as<String>();
    volume = json["device"]["volume_percent"].as<int>();


    //    Serial.print("Title: ");
    //    Serial.println(title);
    //    Serial.print("Artist: ");
    //    Serial.println(artists);
    //    Serial.print("Album: ");
    //    Serial.println(album);
    //    Serial.print("Duration: ");
    //    Serial.println(duration);
    //    Serial.print("Current: ");
    //    Serial.println(current);
    //    Serial.print("Playing: ");
    //    Serial.println(playing);
    //    Serial.print("Device: ");
    //    Serial.println(device);
    //    Serial.print("Type: ");
    //    Serial.println(type);
    //    Serial.print("Active: ");
    //    Serial.println(isActive);
    //    Serial.print("Volume: ");
    //    Serial.println(volume);

    isExpired = false;

  } else {
    current = 0, duration = 10;
    volume = 0;
    artists = "", title = "ERROR", album = "Token expired", device = "", type = "";
    isActive = false, playing = false;
    isExpired = true;
  }

  http.end();

}
