/*_____________________________________________________________________________________________________________________________________________*/
/*    -This program is to config Esp32 connected Wifi through Web. This is to help change the Wifi for ESP32 without reprograming              */
/*    -Just use it as reference as it takes quite a great amount of memory____                                                                 */
/*_____________________________________________________________________________________________________________________________________________*/
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences preferences; //Save the input Wifi info to NvM
WebServer server(80);

const char* apSSID = "ESP32_AP";       // Name of the Access Point
const char* apPassword = "12345678";  // Password for the Access Point (must be at least 8 characters)
long connectTimeout = 15000; //15s to connect before unsuccessfullly connect annouce

void findingWifi(){
  preferences.begin("wifi-cred", false);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");

  if (ssid.isEmpty() || !connectToWiFi(ssid.c_str(), password.c_str()) ) {
    startAccessPoint();
  }

  // Web page to input SSID and Password
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", R"rawliteral(
      <form action="/setwifi" method="post">
        SSID: <input type="text" name="ssid"><br>
        Password: <input type="password" name="password"><br>
        <input type="submit" value="Save">
      </form>
    )rawliteral");
  });

  server.on("/setwifi", HTTP_POST, []() {
    if (server.hasArg("ssid") && server.hasArg("password")) {
      String ssid = server.arg("ssid");
      String password = server.arg("password");

      preferences.putString("ssid", ssid);
      preferences.putString("password", password);

      server.send(200, "text/plain", "Credentials saved. Reboot to connect.");
    } else {
      server.send(400, "text/plain", "Invalid input.");
    }
  });
  server.begin();
}

//Creaate a AP, print ip to terminal for web access
void startAccessPoint() {
  WiFi.softAP(apSSID, apPassword);  // Start Access Point with password
  Serial.println("Access Point started");
  Serial.print("AP SSID: ");
  Serial.println(apSSID);
  Serial.print("AP Password: ");
  Serial.println(apPassword);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());
}

//Start connect to given Wifi
bool connectToWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  long startConnectTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    if (millis() - startConnectTime > connectTimeout){
      Serial.println("Cannot connect to Wifi!");
      return false;
    }
    delay(1000);
  }
  Serial.println("Connected!");
  Serial.print("Current host IP:");
  Serial.println(WiFi.localIP());
  return true;
}


void setup() {
  Serial.begin(115200);
  findingWifi();
}
void loop() {
 server.handleClient();
}


