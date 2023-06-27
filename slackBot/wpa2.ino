//ライブラリ当然
#include <WiFi.h>      //Wifi library
#include <esp_wpa2.h>  //wpa2 library for connections to Enterprise networks

//IDとかの定義
#define EAP_IDENTITY ""  //if connecting from another corporation, use identity@organisation.domain in Eduroam
#define EAP_USERNAME "tkento1985@keio.jp"
#define EAP_PASSWORD "sireodbrrjnjarcx"                                                                                //your Eduroam password
#define ssid "eduroam"                                                                                                 // (Eduroam) SSID
#define host "https://script.google.com/macros/s/AKfycbym5JwkD5H9O4E-0UX5SxH9WQJUFn4HM9ff4tJYqYjQvJVOQb6PQv4rQA/exec"  //external server domain for HTTP connection after authentification Example"arduino.php5.sk"

void wpa2_init() {
  int counter = 0;
  Serial.println();
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  //WiFi.disconnectはtrueを入れないと切ってくれない
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
                          //WiFi.modeは必須
  WiFi.mode(WIFI_STA);    //init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  //provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
  //provide username --> identity and username is same
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  //provide password
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();  //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config);                  //set config settings to enable function
  WiFi.begin(ssid);                                       //connect to wifi
  delay(100);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
    counter++;
    if (counter >= 300) {  //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP());  //print LAN IP
  configTime( JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
}

void wpa2_connecting_check() {
  int counter = 0;
  if (WiFi.status() == WL_CONNECTED) {  //if we are connected to Eduroam network
    // Serial.println("Wifi is still connected with IP: ");
    // Serial.println(WiFi.localIP());            //inform user about his IP address
  } else if (WiFi.status() != WL_CONNECTED) {  //if we lost connection, retry
    WiFi.begin(ssid);
  }
  while (WiFi.status() != WL_CONNECTED) {  //during lost connection, print dots
    delay(100);
    Serial.print(".");
    counter++;
    if (counter >= 300) {  //30 seconds timeout - reset board
      ESP.restart();
    }
  }
}

bool wpa2_getrequest(String id, bool status) {
  WiFiClient client;
  String options = "?locate=" + String(locate) + "&id=" + String(id) + "&status=" + String(status);
  if (client.connect(host, 80)) {
    String url = "/rele/rele1.txt";
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + options + "\r\n" + "User-Agent: ESP32\r\n" + "Connection: close\r\n\r\n");

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }
    String line = client.readStringUntil('\n');
    Serial.println(line);
    return true;
  } else {
    Serial.println("Connection unsucessful");
    return false;
  }
}