#include <WiFi.h>
#include <HTTPClient.h>

#define WIFISSID "LeoWifi"
#define WIFIPSW "wifi12345"


String translateEncryptionType(wifi_auth_mode_t encryptionType) {

  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}

void scanNetworks() {

  int numberOfNetworks = WiFi.scanNetworks();

  Serial.print("Number of networks found: ");
  Serial.println(numberOfNetworks);

  for (int i = 0; i < numberOfNetworks; i++) {

    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));

    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));

    Serial.print("MAC address: ");
    Serial.println(WiFi.BSSIDstr(i));

    Serial.print("Encryption type: ");
    String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
    Serial.println(encryptionTypeDescription);
    Serial.println("-----------------------");

  }
}

void connectToNetwork() {
  WiFi.begin(WIFISSID, WIFIPSW);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }

  Serial.println("Connected to network");

}


void ServerCom(String m, String host, String header = "") {
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    Serial.println("WIFI Ready!");


    if (m == "GET") {
      HTTPClient http;
      http.begin(host); //Specify the URL
      int httpCode = http.GET();                                        //Make the request
      Serial.println(String(httpCode));

      if (httpCode > 0) { //Check for the returning code
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      }
      else {
        Serial.println("Error on GET request");
      }
      http.end(); //Free the resources
    }

    if (m == "POST") {
      HTTPClient http;
      http.begin(host);  //Specify destination for HTTP request
      http.addHeader("Content-Type", "text/plain");             //Specify content-type header
      int httpResponseCode = http.POST("POSTING from ESP32");   //Send the actual POST request
      
      if (httpResponseCode > 0) {
        String response = http.getString();                       //Get the response to the request
        Serial.println(httpResponseCode);   //Print return code
        Serial.println(response);           //Print request answer
      } 
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();  //Free resources
    }
  }
  else {
    Serial.println("Wifi errorr! Request is not sent!");
  }

}


void setup() {

  Serial.begin(115200);
  delay(4000);   //Delay needed before calling the WiFi.begin

  scanNetworks();
  connectToNetwork();
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());

}

void loop() {

  ServerCom("GET", "http://jsonplaceholder.typicode.com/comments?id=10", "");

  ServerCom("POST", "http://jsonplaceholder.typicode.com/posts", "");

  delay(5000);
}
