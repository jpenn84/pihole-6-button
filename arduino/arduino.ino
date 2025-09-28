#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials (Change these to match your own)
const char* ssid = "ssid";
const char* password = "password";

// API endpoints (Change these to match your own)
const char* authUrl = "http://pi.hole/api/auth";
const char* dnsUrl = "http://pi.hole/api/dns";

// PiHole API password (Change this to match your own)
const char* apiAuthPayload = "{\"password\": \"password\"}";

// LEDs and buttons
const int ledEnabled = A0;
const int ledDisabled = A1;
const int btnEnable = 2;
const int btnDisable = 3;

// Session ID
String sessionId;
const int sidMaxAge = 1800; // PiHole default, in seconds
// TODO: Timer for SID

// Blocking timer, in seconds
int blockTimer = 300;

// Block buttons while performing operations
bool blockButtons = false;

void setup() {
  // Setup HW pins & Serial
  pinMode(ledEnabled, OUTPUT);
  pinMode(ledDisabled, OUTPUT);
  pinMode(btnEnable, INPUT_PULLUP);
  pinMode(btnDisable, INPUT_PULLUP);
  Serial.begin(115200);

  // Connect to Wi-Fi
  delay(1000);
  Serial.print("Connecting to Wi-Fi.");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // TODO: Check session ID age and update if necessary

  // TODO: Blink LEDs per status

  // Get Status & set LEDs
  digitalWrite(ledEnabled, isEnabled());
  digitalWrite(ledDisabled, !isEnabled());

  // Read hardware buttons if not blocked
  if (!blockButtons) {
    if (!digitalRead(btnEnable))
    {
      blockButtons = true;
      Serial.println("Button press: 'Enable Blocking'");
      setBlocking(true);
    } else if (!digitalRead(btnDisable)) {
      blockButtons = true;
      Serial.println("Button press: 'Disable Blocking'");
      setBlocking(false);
    }
  }
  delay(50);
}

void setBlocking(bool blocking) {
  String payload = "{\"blocking\":" + String(blocking) + ",\"timer\":" + String(blockTimer) + "}";
  HTTPClient http;
  http.begin(dnsUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("sid", sessionId);
  int httpResponseCode = http.POST(payload);

  blockButtons = false;
}

bool isEnabled() {
  String responsePayload;
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(dnsUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("sid", sessionId);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      responsePayload = http.getString();
      Serial.println("Server response:");
      Serial.println(responsePayload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();

    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, responsePayload);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return "";
    }
    const char* blocking = doc["blocking"];
    return blocking == "enabled";
  }
}

void updateSessionId() {
  String responsePayload;

  // TODO: Delete old session

  // Get new Session ID
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(authUrl);
    http.addHeader("Content-Type", "application/json");
    Serial.println("Sending POST request with payload:");
    Serial.println(apiAuthPayload);
    int httpResponseCode = http.POST(apiAuthPayload);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      responsePayload = http.getString();
      Serial.println("Server response:");
      Serial.println(responsePayload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();

    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, responsePayload);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
    }
    const char* sid = doc["session"]["sid"];
    sessionId = sid;
  }

}
