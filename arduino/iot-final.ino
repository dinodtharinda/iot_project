#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "dinod"
#define WIFI_PASSWORD "12345678"


#define API_KEY ""
#define DATABASE_URL "https://new-iot-8c3f9-default-rtdb.firebaseio.com/"


#define LIGHT_PIN 5
#define LOCK_PIN 2
#define ENROLL_PIN 4


uint8_t id;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long fetchDataPrevMillis = 50;
unsigned long fingerPrintIntervel = 50;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
bool enroll = false;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);



void setup() {
  Serial.begin(57600);
  Serial2.begin(115200);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(LOCK_PIN, OUTPUT);
  pinMode(ENROLL_PIN, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Sign up successful");
  } else {
    Serial.printf("Sign up failed: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);


  while (!Serial)
    ;
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  Serial.println(F("Reading sensor"));


  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains ");
    Serial.print(finger.templateCount);
    Serial.println(" templates");
  }
  timeClient.begin();
}



void fetchData() {

  int lightStatus = getIntFromFirebase("/Fetch/light");
  if (lightStatus == 1) {
    digitalWrite(LIGHT_PIN, HIGH);
  } else {
    digitalWrite(LIGHT_PIN, LOW);
  }

  int lock = getIntFromFirebase("/Fetch/lock");
  if (lock == 1) {
    digitalWrite(LOCK_PIN, HIGH);
  } else {
    digitalWrite(LOCK_PIN, LOW);
  }

  int en = getIntFromFirebase("/Fetch/enroll");
  if (en == 1) {
    enroll = true;
  } else {
    enroll = false;
  }
  setStringToFirebase("/Fetch/message" ,"No Message..");
}

void loop() {
  if (millis() - fingerPrintIntervel > 100 || fingerPrintIntervel == 0) {
    fingerPrintIntervel = millis();
    if (!enroll) {
      getFingerprintID();
      digitalWrite(ENROLL_PIN, LOW);

    } else {
      digitalWrite(ENROLL_PIN, HIGH);
      getFingerprintEnroll();
    }
  }
  if (Firebase.ready() && (millis() - fetchDataPrevMillis > 1000 || fetchDataPrevMillis == 0)) {
    fetchDataPrevMillis = millis();
    fetchData();
  }
}



uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    timeClient.update();
    String currentDateTime = getSLDateTime();
    logData(currentDateTime, "-", 0);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);

  String currentDateTime = getSLDateTime();
  logData(currentDateTime, String(finger.fingerID), 1);
  digitalWrite(LOCK_PIN, HIGH);
  delay(3000);
  digitalWrite(LOCK_PIN, LOW);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);

  return finger.fingerID;
}


int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  // found a match!
  Serial.print("Found ID #");

  Serial.print(finger.fingerID);
  digitalWrite(LOCK_PIN, HIGH);
  delay(3000);
  digitalWrite(LOCK_PIN, LOW);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}

void logData(const String& timestamp, const String& logId, int logStatus) {

  String idPath = "/logs/" + timestamp + "/id";
  String statusPath = "/logs/" + timestamp + "/status";
  String datePath = "/logs/" + timestamp + "/date";

  setIntToFirebase(idPath, logId.toInt());
  setIntToFirebase(statusPath, logStatus);
  setStringToFirebase(datePath, getSLDateTime());
}

void setStringToFirebase(String path, String data) {
  if (Firebase.RTDB.set(&fbdo, path, data)) {
    Serial.println("ID pushed to Firebase");
  } else {
    Serial.println("Error pushing ID to Firebase");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

void setIntToFirebase(String path, int data) {
  if (Firebase.RTDB.setInt(&fbdo, path, data)) {
    Serial.println("ID pushed to Firebase");
  } else {
    Serial.println("Error pushing ID to Firebase");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}



int getIntFromFirebase(String path) {
  Firebase.RTDB.getInt(&fbdo, path);
  if (fbdo.dataType() == "int") {
    int data = fbdo.intData();
    Serial.print(fbdo.intData());
    return data;
  } else {
    Serial.println("Failed to Read Light Status from Firebase");
    Serial.println("REASON: " + fbdo.errorReason());
  }
  return -1;
}




String getSLDateTime() {
  const long utcOffsetSeconds = 19800;
  time_t localTime = timeClient.getEpochTime() + utcOffsetSeconds;
  struct tm timeInfo;
  gmtime_r(&localTime, &timeInfo);

  char formattedTime[30];
  snprintf(formattedTime, sizeof(formattedTime), "%04d-%02d-%02d %02d:%02d:%02d",
           timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
           (timeInfo.tm_hour % 12 == 0) ? 12 : timeInfo.tm_hour % 12,
           timeInfo.tm_min, timeInfo.tm_sec, micros());
  String ampm = (timeInfo.tm_hour < 12) ? "AM" : "PM";
  return String(formattedTime) + ampm;
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        if (enroll) {
          Serial.println(".");
          fetchData();
        } else {
          return 0;
        }
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  setStringToFirebase("/Fetch/message" ,"Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  setStringToFirebase("/Fetch/message" ,"Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");

  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
        setStringToFirebase("/Fetch/message" ,"Prints matched!");

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
 
 
    int printid = getIntFromFirebase("/Fetch/finalPrintId");
    printid++;
    Serial.print("ID ");
    Serial.println(printid);
    p = finger.storeModel(printid);
    setIntToFirebase("Fetch/finalPrintId",printid);

  if (p == FINGERPRINT_OK) {
    enroll = false;
    setIntToFirebase("Fetch/enroll", 0);
    Serial.println("Stored!");
    setStringToFirebase("/Fetch/message" ,"Stored!");

    return 0;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
