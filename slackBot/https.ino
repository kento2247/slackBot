#include <HTTPClient.h>
#include <HTTPSRedirect.h>
#include <WiFi.h>
#include "time.h"

#define JST     3600* 9

#define https_ssid "aterm-059a4d-g"  // your network https_ssid (name of wifi network)
#define https_password "ooya1192"    // your network https_password

HTTPSRedirect* client = nullptr;

void https_init() {
  Serial.print("Attempting to connect to https_ssid: ");
  Serial.println(https_ssid);
  WiFi.begin(https_ssid, https_password);

  byte faild_counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
    faild_counter++;
    if (faild_counter == 10) {
      ESP.restart();
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to ");
    Serial.println(https_ssid);
  }
  configTime( JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
}

void https_gas_get(String id, bool status) {
  String url =
    "https://script.google.com/macros/s/AKfycbxDjC2MepWJTXRChr7BGkMBtSGdZ7e8hMasMsRJkdiMkyucLPoTrijZvBlI1sZtevEwpQ/exec?locate=" + String(locate) + "&id=" + String(id) + "&status=" + String(status);
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  http.end();
}

String https_gas_get_redirect(String id, bool status) {
  String url =
    "/macros/s/AKfycbxDjC2MepWJTXRChr7BGkMBtSGdZ7e8hMasMsRJkdiMkyucLPoTrijZvBlI1sZtevEwpQ/exec?locate=" + String(locate) + "&id=" + String(id) + "&status=" + String(status);
  const char* host = "script.google.com";
  client = new HTTPSRedirect(443);
  String body;
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  client->connect(host, 443);
  client->GET(url, host);
  body = client->getResponseBody();
  delete client;
  client = nullptr;
  Serial.println("end");
  return body;
}

byte https_get_day(){
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  return timeinfo.tm_mday;
}