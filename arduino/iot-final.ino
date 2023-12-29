// #include <Arduino.h>
// #include <WiFi.h>
// #include <Firebase_ESP_Client.h>
// #include <NTPClient.h>
// #include <WiFiUdp.h>
// #include <Adafruit_Fingerprint.h>
// #include <HardwareSerial.h>

// #include "addons/TokenHelper.h"
// //Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"

// // Replace these with your Wi-Fi credentials
// #define WIFI_SSID "dinod"
// #define WIFI_PASSWORD "12345678"

// // Replace these with your Firebase credentials
// #define API_KEY "AIzaSyAjb8mJgpmENdYg_IP_uVlhGcr_W0dzxa8"
// #define DATABASE_URL "https://new-iot-8c3f9-default-rtdb.firebaseio.com/"

// // Define the pin for the light
// #define LIGHT_PIN 5  // Change this to the appropriate pin for your setup
// #define LOCK_PIN 2
// #define ENROLL_PIN 4


// uint8_t id;

// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig config;

// unsigned long fetchDataPrevMillis = 50;
// unsigned long fingerPrintIntervel = 50;

// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
// void setup() {
//   Serial.begin(57600);
//   Serial2.begin(115200);
//   pinMode(LIGHT_PIN, OUTPUT);
//   pinMode(LOCK_PIN, OUTPUT);
//   pinMode(ENROLL_PIN, OUTPUT);



//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   Serial.print("Connecting to Wi-Fi");
//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(300);
//   }
//   Serial.println();
//   Serial.print("Connected with IP: ");
//   Serial.println(WiFi.localIP());
//   Serial.println();

//   config.api_key = API_KEY;
//   config.database_url = DATABASE_URL;

//   if (Firebase.signUp(&config, &auth, "", "")) {
//     Serial.println("Sign up successful");
//   } else {
//     Serial.printf("Sign up failed: %s\n", config.signer.signupError.message.c_str());
//   }

//   config.token_status_callback = tokenStatusCallback;
//   Firebase.begin(&config, &auth);
//   Firebase.reconnectWiFi(true);

//   //Fringer print part
//   while (!Serial)
//     ;  // For Yun/Leo/Micro/Zero/...
//   delay(100);
//   Serial.println("\n\nAdafruit finger detect test");

//   // set the data rate for the sensor serial port
//   finger.begin(57600);
//   delay(5);
//   if (finger.verifyPassword()) {
//     Serial.println("Found fingerprint sensor!");
//   } else {
//     Serial.println("Did not find fingerprint sensor :(");
//     while (1) { delay(1); }
//   }

//   Serial.println(F("Reading sensor parameters"));
//   finger.getParameters();
//   Serial.print(F("Status: 0x"));
//   Serial.println(finger.status_reg, HEX);
//   Serial.print(F("Sys ID: 0x"));
//   Serial.println(finger.system_id, HEX);
//   Serial.print(F("Capacity: "));
//   Serial.println(finger.capacity);
//   Serial.print(F("Security level: "));
//   Serial.println(finger.security_level);
//   Serial.print(F("Device address: "));
//   Serial.println(finger.device_addr, HEX);
//   Serial.print(F("Packet len: "));
//   Serial.println(finger.packet_len);
//   Serial.print(F("Baud rate: "));
//   Serial.println(finger.baud_rate);

//   finger.getTemplateCount();

//   if (finger.templateCount == 0) {
//     Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
//   } else {
//     Serial.println("Waiting for valid finger...");
//     Serial.print("Sensor contains ");
//     Serial.print(finger.templateCount);
//     Serial.println(" templates");
//   }
//   timeClient.begin();
// }

// bool enroll = false;

// void fetchData() {
//   // Fetch Light Status from Firebase
//   Firebase.RTDB.getInt(&fbdo, "/Fetch/light");

//   if (fbdo.dataType() == "int") {
//     int lightStatus = fbdo.intData();
//     Serial.print(fbdo.intData());
//     if (lightStatus == 1) {
//       digitalWrite(LIGHT_PIN, HIGH);  // Turn on the light
//     } else {
//       digitalWrite(LIGHT_PIN, LOW);  // Turn off the light
//     }
//   } else {
//     Serial.println("Failed to Read Light Status from Firebase");
//     Serial.println("REASON: " + fbdo.errorReason());
//   }

//   Firebase.RTDB.getInt(&fbdo, "/Fetch/lock");

//   if (fbdo.dataType() == "int") {
//     int lightStatus = fbdo.intData();
//     Serial.print(fbdo.intData());
//     if (lightStatus == 1) {
//       digitalWrite(LOCK_PIN, HIGH);  // Turn on the light
//     } else {
//       digitalWrite(LOCK_PIN, LOW);  // Turn off the light
//     }
//   } else {
//     Serial.println("Failed to Read Light Status from Firebase");
//     Serial.println("REASON: " + fbdo.errorReason());
//   }


//   Firebase.RTDB.getInt(&fbdo, "/Fetch/enroll");

//   if (fbdo.dataType() == "int") {
//     int lightStatus = fbdo.intData();
//     Serial.print(fbdo.intData());
//     if (lightStatus == 1) {
//       enroll = true;
//     } else {
//       enroll = false;
//     }
//   } else {
//     Serial.println("Failed to Read Light Status from Firebase");
//     Serial.println("REASON: " + fbdo.errorReason());
//   }
// }
// void loop() {
//   if (millis() - fingerPrintIntervel > 100 || fingerPrintIntervel == 0) {
//     fingerPrintIntervel = millis();
//     if (!enroll) {
//       getFingerprintID();
//       digitalWrite(ENROLL_PIN, LOW);  // Turn off the light

//     } else {
//       digitalWrite(ENROLL_PIN, HIGH);  // Turn off the light
//       getFingerprintEnroll();
     
//     }
//   }

//    if (Firebase.ready() && (millis() - fetchDataPrevMillis > 1000 || fetchDataPrevMillis == 0)) {
//     fetchDataPrevMillis = millis();
//     fetchData();
//   }
  
// }



// uint8_t getFingerprintID() {
//   uint8_t p = finger.getImage();
//   switch (p) {
//     case FINGERPRINT_OK:
//       Serial.println("Image taken");
//       break;
//     case FINGERPRINT_NOFINGER:
//       Serial.println("No finger detected");
//       return p;
//     case FINGERPRINT_PACKETRECIEVEERR:
//       Serial.println("Communication error");
//       return p;
//     case FINGERPRINT_IMAGEFAIL:
//       Serial.println("Imaging error");
//       return p;
//     default:
//       Serial.println("Unknown error");
//       return p;
//   }

//   // OK success!

//   p = finger.image2Tz();
//   switch (p) {
//     case FINGERPRINT_OK:
//       Serial.println("Image converted");
//       break;
//     case FINGERPRINT_IMAGEMESS:
//       Serial.println("Image too messy");
//       return p;
//     case FINGERPRINT_PACKETRECIEVEERR:
//       Serial.println("Communication error");
//       return p;
//     case FINGERPRINT_FEATUREFAIL:
//       Serial.println("Could not find fingerprint features");
//       return p;
//     case FINGERPRINT_INVALIDIMAGE:
//       Serial.println("Could not find fingerprint features");
//       return p;
//     default:
//       Serial.println("Unknown error");
//       return p;
//   }

//   // OK converted!
//   p = finger.fingerSearch();
//   if (p == FINGERPRINT_OK) {
//     Serial.println("Found a print match!");
//   } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//     Serial.println("Communication error");
//     return p;
//   } else if (p == FINGERPRINT_NOTFOUND) {
//     Serial.println("Did not find a match");
//     timeClient.update();
//     String currentDateTime = getSLDateTime();
//     logData(currentDateTime, "-", 0);
//     return p;
//   } else {
//     Serial.println("Unknown error");
//     return p;
//   }

//   // found a match!
//   Serial.print("Found ID #");
//   Serial.print(finger.fingerID);

//   String currentDateTime = getSLDateTime();
//   logData(currentDateTime, String(finger.fingerID), 1);
//   digitalWrite(LOCK_PIN, HIGH);
//   delay(3000);
//   digitalWrite(LOCK_PIN, LOW);
//   Serial.print(" with confidence of ");
//   Serial.println(finger.confidence);

//   return finger.fingerID;
// }

// // returns -1 if failed, otherwise returns ID #
// int getFingerprintIDez() {
//   uint8_t p = finger.getImage();
//   if (p != FINGERPRINT_OK) return -1;

//   p = finger.image2Tz();
//   if (p != FINGERPRINT_OK) return -1;

//   p = finger.fingerFastSearch();
//   if (p != FINGERPRINT_OK) return -1;

//   // found a match!
//   Serial.print("Found ID #");

//   Serial.print(finger.fingerID);
//   digitalWrite(LOCK_PIN, HIGH);
//   delay(3000);
//   digitalWrite(LOCK_PIN, LOW);
//   Serial.print(" with confidence of ");
//   Serial.println(finger.confidence);
//   return finger.fingerID;
// }

// void logData(const String& timestamp, const String& logId, int logStatus) {
//   // Generate paths for each attribute
//   String idPath = "/logs/" + timestamp + "/id";
//   String statusPath = "/logs/" + timestamp + "/status";
//   String datePath = "/logs/" + timestamp + "/date";


//   // Store each attribute separately
//   if (Firebase.RTDB.setInt(&fbdo, idPath, logId.toInt())) {
//     Serial.println("ID pushed to Firebase");
//   } else {
//     Serial.println("Error pushing ID to Firebase");
//     Serial.println("REASON: " + fbdo.errorReason());
//   }

//   if (Firebase.RTDB.setInt(&fbdo, statusPath, logStatus)) {
//     Serial.println("Status pushed to Firebase");
//   } else {
//     Serial.println("Error pushing status to Firebase");
//     Serial.println("REASON: " + fbdo.errorReason());
//   }

//   if (Firebase.RTDB.set(&fbdo, datePath, getSLDateTime())) {
//     Serial.println("Status pushed to Firebase");
//   } else {
//     Serial.println("Error pushing status to Firebase");
//     Serial.println("REASON: " + fbdo.errorReason());
//   }
// }

// String getSLDateTime() {
//   // UTC offset for Sri Lanka (in seconds)
//   const long utcOffsetSeconds = 19800;  // 5 hours and 30 minutes

//   // Calculate local time
//   time_t localTime = timeClient.getEpochTime() + utcOffsetSeconds;

//   // Format the local time as a string with microseconds
//   struct tm timeInfo;
//   gmtime_r(&localTime, &timeInfo);

//   char formattedTime[30];  // Increased the size to accommodate microseconds
//   snprintf(formattedTime, sizeof(formattedTime), "%04d-%02d-%02d %02d:%02d:%02d",
//            timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
//            (timeInfo.tm_hour % 12 == 0) ? 12 : timeInfo.tm_hour % 12,
//            timeInfo.tm_min, timeInfo.tm_sec, micros());

//   // Determine AM or PM
//   String ampm = (timeInfo.tm_hour < 12) ? "AM" : "PM";

//   return String(formattedTime) + ampm;
// }

// uint8_t readnumber(void) {
//   uint8_t num = 0;

//   while (num == 0) {
//     while (!Serial.available())
//       ;
//     num = Serial.parseInt();
//   }
//   return num;
// }

// uint8_t getFingerprintEnroll() {

//   int p = -1;
//   Serial.print("Waiting for valid finger to enroll as #");
//   Serial.println(id);
//   while (p != FINGERPRINT_OK) {
//     p = finger.getImage();
//     switch (p) {
//       case FINGERPRINT_OK:
//         Serial.println("Image taken");
//         break;
//       case FINGERPRINT_NOFINGER:
//         if (enroll) {
//           Serial.println(".");
//           fetchData();
//         } else {
//           return 0;
//         }

//         break;
//       case FINGERPRINT_PACKETRECIEVEERR:
//         Serial.println("Communication error");
//         break;
//       case FINGERPRINT_IMAGEFAIL:
//         Serial.println("Imaging error");
//         break;
//       default:
//         Serial.println("Unknown error");
//         break;
//     }
//   }

//   // OK success!

//   p = finger.image2Tz(1);
//   switch (p) {
//     case FINGERPRINT_OK:
//       Serial.println("Image converted");
//       break;
//     case FINGERPRINT_IMAGEMESS:
//       Serial.println("Image too messy");
//       return p;
//     case FINGERPRINT_PACKETRECIEVEERR:
//       Serial.println("Communication error");
//       return p;
//     case FINGERPRINT_FEATUREFAIL:
//       Serial.println("Could not find fingerprint features");
//       return p;
//     case FINGERPRINT_INVALIDIMAGE:
//       Serial.println("Could not find fingerprint features");
//       return p;
//     default:
//       Serial.println("Unknown error");
//       return p;
//   }

//   Serial.println("Remove finger");
//   delay(2000);
//   p = 0;
//   while (p != FINGERPRINT_NOFINGER) {
//     p = finger.getImage();
//   }
//   Serial.print("ID ");
//   Serial.println(id);
//   p = -1;
//   Serial.println("Place same finger again");
//   while (p != FINGERPRINT_OK) {
//     p = finger.getImage();
//     switch (p) {
//       case FINGERPRINT_OK:
//         Serial.println("Image taken");
//         break;
//       case FINGERPRINT_NOFINGER:
//         Serial.print(".");
//         break;
//       case FINGERPRINT_PACKETRECIEVEERR:
//         Serial.println("Communication error");
//         break;
//       case FINGERPRINT_IMAGEFAIL:
//         Serial.println("Imaging error");
//         break;
//       default:
//         Serial.println("Unknown error");
//         break;
//     }
//   }

//   // OK success!

//   p = finger.image2Tz(2);
//   switch (p) {
//     case FINGERPRINT_OK:
//       Serial.println("Image converted");
//       break;
//     case FINGERPRINT_IMAGEMESS:
//       Serial.println("Image too messy");
//       return p;
//     case FINGERPRINT_PACKETRECIEVEERR:
//       Serial.println("Communication error");
//       return p;
//     case FINGERPRINT_FEATUREFAIL:
//       Serial.println("Could not find fingerprint features");
//       return p;
//     case FINGERPRINT_INVALIDIMAGE:
//       Serial.println("Could not find fingerprint features");
//       return p;
//     default:
//       Serial.println("Unknown error");
//       return p;
//   }

//   // OK converted!
//   Serial.print("Creating model for #");

//   Serial.println(id);

//   p = finger.createModel();
//   if (p == FINGERPRINT_OK) {
//     Serial.println("Prints matched!");
//   } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//     Serial.println("Communication error");
//     return p;
//   } else if (p == FINGERPRINT_ENROLLMISMATCH) {
//     Serial.println("Fingerprints did not match");
//     return p;
//   } else {
//     Serial.println("Unknown error");
//     return p;
//   }
//   Firebase.RTDB.getInt(&fbdo, "/Fetch/finalPrintId");

//   if (fbdo.dataType() == "int") {
//     int printid = fbdo.intData();
//     printid++;
//     Serial.print(fbdo.intData());
//     Serial.print("ID ");
//     Serial.println(printid);
//     p = finger.storeModel(printid);
//        if (Firebase.RTDB.setInt(&fbdo, "Fetch/finalPrintId", printid)) {
//       Serial.println("ID pushed to Firebase");
//     } else {
//       Serial.println("Error pushing ID to Firebase");
//       Serial.println("REASON: " + fbdo.errorReason());
//     }
//   } else {
//     Serial.println("Failed to Read Light Status from Firebase");
//     Serial.println("REASON: " + fbdo.errorReason());
//   }



//   if (p == FINGERPRINT_OK) {
//     enroll = false;
//     if (Firebase.RTDB.setInt(&fbdo, "Fetch/enroll", 0)) {
//       Serial.println("ID pushed to Firebase");
//     } else {
//       Serial.println("Error pushing ID to Firebase");
//       Serial.println("REASON: " + fbdo.errorReason());
//     }

//     Serial.println("Stored!");
//     return 0;
//   } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//     Serial.println("Communication error");
//     return p;
//   } else if (p == FINGERPRINT_BADLOCATION) {
//     Serial.println("Could not store in that location");
//     return p;
//   } else if (p == FINGERPRINT_FLASHERR) {
//     Serial.println("Error writing to flash");
//     return p;
//   } else {
//     Serial.println("Unknown error");
//     return p;
//   }

//   return true;
// }
