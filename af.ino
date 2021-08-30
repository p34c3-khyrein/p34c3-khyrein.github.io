/*
   Automatic Feeder (duduk)
   HTTP Client POST Request
   Code by Jefri Herdi Triyanto (082214252455)
   Copyright (c) 2021, BPBAT
   All rights reserved.
*/

// microcontroller ? processor, ram, internal, non os, executable, 
// microprocessor  ? processor, ram, internal, os, non executable, 

//=======================================================================
//                                  Library
//=======================================================================
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Servo.h>

Servo servo_katup;

//=======================================================================
//                                   Define
//=======================================================================

//== EEPROM CONFIGURE ==//
int address_execute = 0;
int address_putaran = 1;

//== SERVO CONFIGURE ==//
int pin_servo_katup = D0;
int sudut_servo_katup_buka = 30; //ubah (cari max)
int sudut_servo_katup_tutup = 5; //ubah (cari 0)

/* Set these to your desired credentials. */
// pake password
//const char* ssid = "Hotspot Arsip"; // ubah
//const char* password = "arsip2020"; // ubah
// non password
const char* ssid = "Pangasius hostpot"; // ubah

// yang membedakan antara robot yang lain
const char *model = "automatic-feeder-duduk"; // klasifikasi robot

const char *api = "hachry-patin-02"; // rubah ? api tiap robot berbeda
String postData = "password=asuberkelas"; // ubah / tidak juga boleh

//Web/Server address to read/write from (JANGAN RUBAH INI)
int refresh_rate = 200; //ms
String protocol = "http";     // CORS
String path = "/robotic";     // routes

// public
//String host = "202.65.125.102"; // server
//String port = "4000";           // port
// local
String host = "192.168.22.5"; // server
String port = "80";           // port

//=======================================================================
//                               Power on setup
//=======================================================================

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);

  // setup servo
  servo_katup.attach(pin_servo_katup);
  // sudut awal servo
  servo_katup.write(sudut_servo_katup_tutup);

  Serial.begin(115200);
  EEPROM.begin(512);

  //Prevents reconnection issue (taking too long to connect)
  WiFi.mode(WIFI_OFF);
  delay(1000);
  //This line hides the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);

  //Connect to your WiFi router (mungkin rubah jika definisi password diatas dirubah)
  //  WiFi.begin(ssid, password); // pake password
  WiFi.begin(ssid); // non password
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    digitalWrite(LED_BUILTIN, LOW); // hidup
    Serial.print(".");
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  // Setup clear
  // kedip 3x
  kedip_lampu(3, 300);
  Serial.println("Listening Server...");
}

//=======================================================================
//                             Main Program Loop
//=======================================================================
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(refresh_rate);  // Post Data at every refresh rate value (ms)

  // Get Data
  String response = requestData(postData, "ping");
  StaticJsonDocument<500> doc;
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.println(response);
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  String success = doc["success"]; // check validasi
  String message = doc["message"]; // message di setiap tempat PHP wajib ada
  if (success == "true") {
    int execute = doc["execute"];
    int test_buka = doc["test_buka"];
    int speed = doc["speed"];
    int putaran = doc["putaran"];
    int delay_putaran = doc["delay"];

    if (execute == 1 && bacaEEPROM(address_execute) == 0) {
      tulisEEPROM(address_execute, 1);
      jalankan(speed, putaran, delay_putaran);
    } else if (bacaEEPROM(address_execute) == 1) {
      hentikanExecute("exit");
    } else {
      if (test_buka == 1) {
        buka_katup(speed);
        hentikanExecute("exit-test");
      } else {
        // idle
      }
    }

    //    Serial.println(message); // debug monitoring
  } else {
    Serial.println(message);
  }
}

//=======================================================================
//                             Main Function
//=======================================================================
void jalankan(int speed, int putaran, int delay_putaran) {
  Serial.println();
  Serial.println("Jalankan!");

  for (int i = 0; i <= putaran - 1; i++) { // satu putaran pakan
    Serial.println(); // space

    String counting = "pakan ke: " + String(i + 1);
    Serial.println(counting); // counting

    buka_katup(speed);
    delay(delay_putaran); // delay next putaran

    // lapor karena udah sebar pakan
    laporan();
  }

  hentikanExecute("exit");

  Serial.println();
  Serial.println("Listening Server...");
}

void buka_katup(int speed) {
  // buka servo
  digitalWrite(LED_BUILTIN, LOW); // hidup
  Serial.println("buka servo");
  servo_katup.write(sudut_servo_katup_buka);

  delay(speed); // atur quantity

  // tutup servo
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("tutup servo");
  servo_katup.write(sudut_servo_katup_tutup);
}

void laporan() {
  // Get Data
  String response = requestData(postData, "lapor-ndan");
  StaticJsonDocument<500> doc;
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.println(response);
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  String success = doc["success"]; // check validasi
  String message = doc["message"]; // message di setiap tempat PHP wajib ada
  if (success == "true") {
    Serial.println(message);
  }
}

void hentikanExecute(String perihal) {
  Serial.println();
  Serial.println("Hentikan Execute!");
  Serial.println();

  // Get Data
  String response = requestData(postData, perihal);
  StaticJsonDocument<500> doc;
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.println(response);
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  String success = doc["success"]; // check validasi
  String message = doc["message"]; // message di setiap tempat PHP wajib ada
  if (success == "true") {
    if (perihal == "exit") {
      tulisEEPROM(address_execute, 0);

      // kedip 2x
      kedip_lampu(2, 300);
    } else {
      Serial.println(message);
    }
  } else {
    Serial.println(message);
  }
}

//=======================================================================
//                             Extra Function
//=======================================================================
String requestData(String postData, String perihal) {
  //Declare object of class HTTPClient
  HTTPClient http;

  http.begin(protocol + "://" + host + ":" + port + path + "/" + model + "/" + api + "/" + perihal);
  //Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload

  http.end();  //Close connection

  if (httpCode == 200) {
    return payload; // Print request response payload for debug
  } else {
    return "Error: " + httpCode;
  }
}

void kedip_lampu(int kali, int speed) {
  for (int i = 0; i <= kali - 1; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(speed);
    digitalWrite(LED_BUILTIN, LOW); // hidup
    delay(speed);
  }
  digitalWrite(LED_BUILTIN, HIGH);
}

void tulisEEPROM(int address, int value) {
  EEPROM.write(address, value);
  EEPROM.commit();
}

int bacaEEPROM(int address) {
  return EEPROM.read(address);
}
