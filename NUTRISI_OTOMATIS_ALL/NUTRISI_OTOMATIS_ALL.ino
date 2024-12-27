#include <ArduinoOTA.h>
#include <Ticker.h>
//#include <WiFi.h>      //esp32
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>  
//#include <HTTPClient.h>  //esp32
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
//#include <LiquidCrystal_I2C.h>
//#include <RTClib.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
#include "fuzzy_function.h"

// Password untuk autentikasi OTA
const char* ota_password = "123";
void setupOTA();
String OTA_msg = "modif yield()";

// Google script Web_App_URL.
String Web_App_URL = "https://script.google.com/macros/s/AKfycbwVtR04NDLnKFMrjlx2EHCEvLHktAMAuHcxUE4rBzrnD1UwdEL0UoegfaXF4j328ZQHUQ/exec";
void spreadsheet();
void readSpreadsheetData();
String status;
int dataSpreadsheet;
String warning;

//MQTT
const char* mqtt_server = "broker.mqtt-dashboard.com";

const char* SUBTOPIC_SETPH = "greenhouse/input/setph";
const char* SUBTOPIC_SETPPM = "greenhouse/input/setppm";
const char* SUBTOPIC_PENYIRAM = "greenhouse/input/penyiram";
const char* SUBTOPIC_TEST = "greenhouse/input/test";

const char* PUBTOPIC_PH = "greenhouse/output/ph";
const char* PUBTOPIC_PPM = "greenhouse/output/ppm";
const char* PUBTOPIC_TINGGI = "greenhouse/output/tinggi";
const char* PUBTOPIC_SUHU = "greenhouse/output/suhu";
const char* PUBTOPIC_WAKTU = "greenhouse/output/waktu";

const char* PUBTOPIC_POMPA1 = "greenhouse/output/pompa1";
const char* PUBTOPIC_POMPA2 = "greenhouse/output/pompa2";
const char* PUBTOPIC_POMPA3 = "greenhouse/output/pompa3";
const char* PUBTOPIC_POMPA4 = "greenhouse/output/pompa4";
const char* PUBTOPIC_POMPA5 = "greenhouse/output/pompa5";
const char* PUBTOPIC_POMPA6 = "greenhouse/output/pompa6";
const char* PUBTOPIC_POMPA7 = "greenhouse/output/pompa7";
const char* PUBTOPIC_POMPA8 = "greenhouse/output/pompa8";
const char* PUBTOPIC_OTA = "greenhouse/output/OTA";
const char* PUBTOPIC_WARNING = "greenhouse/output/warning";
const char* PUBTOPIC_STATUS_SISTEM = "greenhouse/output/status";

String pompa1 = "Padam", pompa2 = "Padam", pompa3 = "Padam", pompa4 = "Padam", pompa5 = "Padam", pompa6 = "Padam", pompa7 = "Padam", pompa8 = "Padam";

//const char* ssid ="bebas";
//const char* password = "akunulisaja";
const char* ssid ="Prastzy.net";
const char* password = "123456781";

//Deklarasi Pin

//#define pin_pH D4//39  //analog pin
//#define pin_EC D4//36  //analog pin
//#define PIN_TRIG D4//2
//#define PIN_ECHO D4//15

#define relay_pHup D4//17   //D4
#define relay_pHdn D4//12  //D5
#define relay_nutrisi D4//26  //D6
#define relay_ambilSampel D4//5  //D3
#define relay_buangSampel D4//18  //D2
#define relay_penyiram D4//14  //D8
#define relay_tangki D4//19  //D1
#define relay_mixer D4//27  //D2


//#define ONE_WIRE_BUS 0
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
WiFiClient espClient;
PubSubClient client(espClient);
//LiquidCrystal_I2C lcd(0x27,20,4);
//RTC_DS1307 rtc;
//int setWaktu1 [] = {8,0,0}; //jam, menit, detik
int menit;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

float set_jarak = 100;
float pH, PPM, selisih_pH, selisih_PPM, set_pH = 7, set_PPM = 800;
int duration;
float suhu;
float tinggi_cm, jarak_cm, jarak_inci;
float p_inputf, n_inputf, p_hasil, n_hasil;
bool kondisi = false;

void mainTask();
void baca_pH();
void baca_PPM();
void baca_jarak();
void baca_suhu();
void kontrol_pH();
void kontrol_PPM();
void kontrol_tinggi();
void baca_RTC();
void outputFuzzy();

void setup_wifi();
void reconnect();
void callback(char *topic, byte *payload, unsigned int length);

Ticker task;
void monitoring ();

void setup() {
  Serial.begin(115200);
  //sensors.begin();
  
  //pinMode(pin_pH, INPUT);
  //pinMode(pin_EC, INPUT);
  //pinMode(PIN_ECHO, INPUT);
  
  //pinMode(PIN_TRIG, OUTPUT);
  pinMode(relay_pHup, OUTPUT);
  pinMode(relay_pHdn, OUTPUT);
  pinMode(relay_nutrisi, OUTPUT);
  //pinMode(relay_nutrisiB, OUTPUT);
  pinMode(relay_mixer, OUTPUT);
  pinMode(relay_penyiram, OUTPUT);
  pinMode(relay_tangki, OUTPUT);
  pinMode(relay_ambilSampel, OUTPUT);
  pinMode(relay_buangSampel, OUTPUT);
  
  setup_wifi();
  
//  lcd.init();   
  /*                  
  lcd.backlight();
  lcd.setCursor(3,1);
  lcd.print("START  PROGRAM");
  lcd.setCursor(3,2);
  lcd.print("==============");
  delay(1000);
  lcd.clear();
  */
  setupOTA();  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  /*
   if (! rtc.begin()) {
    Serial.println("RTC tidak ditemukan");
    while (1);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC tidak berjalan, setting waktu...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set waktu sesuai waktu kompilasi
  }
  */
  task.attach(3, monitoring);
}

void loop() { 
  if (!client.connected()) {
          reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
  mainTask ();
}

//=========================================================
void mainTask (){
    
    //DateTime now = rtc.now();
    //if (now.hour() == setWaktu1[0] && now.minute() == setWaktu1[1] && now.second() == setWaktu1[2] ){
    //if (now.minute() == menit && now.second() == 1){
    if (kondisi == true){ //pengganti rtc 
      Serial.println("==================================");
      Serial.println("kondisi didalam mainTask ");
      Serial.println("==================================");

      Serial.println("menjalankan kontrol_tinggi ");
      kontrol_tinggi();
      yield();
      Serial.println("Mengaktifkan relay_buangSampel");
      digitalWrite (relay_buangSampel, LOW); 
      pompa3 = "Menyala";
      Serial.println("delay 5 detik ");
      delay(3000);
      Serial.println("Menonaktifkan relay_buangSampel");
      digitalWrite (relay_buangSampel, HIGH); 
      pompa3 = "Padam";
      yield();
      Serial.println("Mengaktifkan relay_ambilSampel");
      digitalWrite (relay_ambilSampel, LOW); 
      pompa2 = "Menyala";
      Serial.println("delay 5 detik ");
      delay(3000);
      Serial.println("Menonaktifkan relay_ambilSampel");
      digitalWrite (relay_ambilSampel, HIGH); 
      pompa2 = "Padam";
      status = "Sebelum";
      yield();
      //data sensor sebelum diotomatisasi
      spreadsheet();
      readSpreadsheetData();

      Serial.println("menjalankan kontrol_pH");
      kontrol_pH();
      Serial.println("menjalankan kontrol_PPM");
      kontrol_PPM(); 
      yield();
      Serial.println("Mengaktifkan relay_mixer");
      digitalWrite (relay_mixer, LOW);  
      pompa4 = "Menyala";
      Serial.println("delay 5 detik ");
      delay(3000);
      Serial.println("Menonaktifkan relay_mixer");
      digitalWrite (relay_mixer, HIGH); 
      pompa4 = "Padam";
      yield();
      Serial.println("Mengaktifkan relay_buangSampel");
      digitalWrite (relay_buangSampel, LOW); 
      pompa3 = "Menyala";
      Serial.println("delay 5 detik ");
      delay(3000);
      Serial.println("Menonaktifkan relay_buangSampel");
      digitalWrite (relay_buangSampel, HIGH); 
      pompa3 = "Padam";
      yield();
      Serial.println("Mengaktifkan relay_ambilSampel");
      digitalWrite (relay_ambilSampel, LOW); 
      pompa2 = "Menyala";
      Serial.println("delay 5 detik ");
      delay(3000);
      Serial.println("Menonaktifkan relay_ambilSampel");
      digitalWrite (relay_ambilSampel, HIGH);  
      pompa2 = "Padam";
      status = "Sesudah";
      spreadsheet();
      readSpreadsheetData();
      //data sensor setelah diotomatisasi
      yield();
      Serial.println("Mengaktifkan relay_penyiram");
      digitalWrite (relay_penyiram, LOW); 
      pompa5 = "Menyala";
      Serial.println("delay 5 detik ");
      delay(3000);
      Serial.println("Menonaktifkan relay_penyiram");
      digitalWrite (relay_penyiram, HIGH); 
      pompa5 = "Padam";
      yield();
      Serial.println("menjalankan kontrol_tinggi ");
      kontrol_tinggi();
      String statusSistem = "Otomatisasi Berhasil";
      client.publish(PUBTOPIC_STATUS_SISTEM, statusSistem.c_str());
      yield();
    }
    else {
      Serial.println("==================================");
      Serial.println("kondisi diluar mainTask");
      Serial.println("==================================");
    }
    delay(1000);
}

void monitoring(){
    baca_pH(); //pH
    baca_PPM(); //PPM
    baca_jarak(); //jarak
    baca_suhu();  //suhu
    //DateTime now = rtc.now(); //waktu
    outputFuzzy(); //output fuzzy

    //Serial Monitor
   /* baca_RTC();
    Serial.print("pH : "); Serial.println(pH);
    Serial.print("EC : "); Serial.println(PPM);
    Serial.print("Suhu : "); Serial.println(suhu);
    Serial.print("Ketinggian Air (cm): "); Serial.println(tinggi_cm);
    Serial.print("Output fuzzy pH : "); Serial.println(p_hasil);
    Serial.print("Output fuzzy nutrisi : "); Serial.println(n_hasil);
    Serial.println("");
   */
    //Print LCD
    /*
    lcd.setCursor(0,1); 
    lcd.print("nilai pH : "); lcd.print(pH);
    lcd.setCursor(0,2); 
    lcd.print("nilai EC : "); lcd.print(PPM);
    lcd.setCursor(0,3); 
    lcd.print("Nilai Suhu : "); lcd.print(suhu); lcd.print(" C");
    lcd.clear();
    */
   
    //Print MQTT
    baca_RTC();
    client.publish(PUBTOPIC_PH, String(pH).c_str());
    client.publish(PUBTOPIC_PPM, String(PPM).c_str());
    client.publish(PUBTOPIC_SUHU, String(suhu).c_str());
    client.publish(PUBTOPIC_TINGGI, String(tinggi_cm).c_str());
    //client.publish(PUBTOPIC_TINGGI, String(jarak_cm).c_str());

    client.publish(PUBTOPIC_POMPA1, pompa1.c_str());
    client.publish(PUBTOPIC_POMPA2, pompa2.c_str());
    client.publish(PUBTOPIC_POMPA3, pompa3.c_str());
    client.publish(PUBTOPIC_POMPA4, pompa4.c_str());
    client.publish(PUBTOPIC_POMPA5, pompa5.c_str());
    client.publish(PUBTOPIC_POMPA6, pompa6.c_str());
    client.publish(PUBTOPIC_POMPA7, pompa7.c_str());
    client.publish(PUBTOPIC_POMPA8, pompa8.c_str());
    if (dataSpreadsheet > 1000){
      warning = "Data sudah terlalu banyak";
    }else{
      warning = String (dataSpreadsheet);  
    }
    client.publish(PUBTOPIC_WARNING, warning.c_str());
}

void baca_pH(){
  //float potValue1 = analogRead(pin_pH);
  //pH = potValue1 / 4095 * 14;
  int randomNumber = random(0, 41);  
  //angka random dari 5 sampai 9 dengan interval 0,1
  pH = 5.0 + (randomNumber * 0.1);

  selisih_pH = abs(set_pH - pH);
  Error_pH = selisih_pH;
}

void kontrol_pH(){
  baca_pH();
  outputFuzzy();
  if (pH < set_pH  ){
    Serial.println("Mengaktifkan relay_pHup");
    digitalWrite(relay_pHup, LOW);
    pompa6 = "Menyala";
    digitalWrite(relay_pHdn, HIGH);
    pompa7 = "Padam";
    Serial.print("delay : ");Serial.print(lamaPompa_pH); Serial.println(" detik");
    delay(lamaPompa_pH*1000);
    Serial.println("Menonaktifkan relay_pHup");
    digitalWrite(relay_pHup, HIGH);
    pompa6 = "Padam";
    digitalWrite(relay_pHdn, HIGH);
    pompa7 = "Padam";
    yield();
  }
  else if (pH >= set_pH  ){
    Serial.println("Mengaktifkan relay_pHdn");
    digitalWrite(relay_pHup, HIGH);
    pompa6 = "Padam";
    digitalWrite(relay_pHdn, LOW);
    pompa7 = "Menyala";
    Serial.print("delay : ");Serial.print(lamaPompa_pH); Serial.println(" detik");
    delay(lamaPompa_pH*1000);
    Serial.println("Menonaktifkan relay_pHdn");
    digitalWrite(relay_pHup, HIGH);
    pompa6 = "Padam";
    digitalWrite(relay_pHdn, HIGH);
    pompa7 = "Padam";
    yield();
  }
}

void baca_PPM(){
  //float potValue2 = analogRead(pin_EC);
  //PPM = potValue2 / 4095 * 100;
  PPM = random(550, 1051); 
  if (PPM <= set_PPM){
    selisih_PPM = set_PPM - PPM;
  }
  else if (PPM > set_PPM){
    selisih_PPM = 0;
  }
  Error_PPM = selisih_PPM;
}

void kontrol_PPM(){
  baca_PPM();
  outputFuzzy();
  if (PPM < set_PPM  ){
    Serial.println("Mengaktifkan relay_nutrisi");
    digitalWrite(relay_nutrisi, LOW);
    pompa8 = "Menyala";
    Serial.print("delay : ");Serial.print(lamaPompa_PPM); Serial.println(" detik");
    delay(lamaPompa_PPM*1000);
    Serial.println("Menonaktifkan relay_nutrisi");
    digitalWrite(relay_nutrisi, HIGH);
    pompa8 = "Padam";
    yield();
  }
  else if (PPM >= set_PPM  ){
    Serial.println("Menonaktifkan relay_nutrisi");
    digitalWrite(relay_nutrisi, HIGH);
    pompa8 = "Padam";
  }
}

void baca_jarak(){
  //digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  //digitalWrite(PIN_TRIG, LOW);
  //duration = pulseIn(PIN_ECHO, HIGH);
  //jarak_inci = duration / 148; //dalam inci
  //jarak_cm = duration / 58; //dalam cm
  jarak_cm  = random(0, 21); 
  tinggi_cm = set_jarak - jarak_cm;
}

void kontrol_tinggi(){
  baca_jarak();
  while (tinggi_cm < set_jarak - 90) {
  //while (jarak_cm < set_jarak - 90) {
    Serial.println("Mengaktifkan relay_tangki");
    digitalWrite(relay_tangki, LOW);
    pompa1 = "Menyala";
    yield();
  }
  Serial.println("delay sampai tinggi air terpenuhi");
  delay(1000);
  Serial.println("Menonaktifkan relay_tangki");
  digitalWrite(relay_tangki, HIGH);
  pompa6 = "Padam";
}

void baca_suhu(){
  //sensors.requestTemperatures(); 
  //suhu = sensors.getTempCByIndex(0);
  suhu = random(25, 35); 
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Receive Topic: ");
  Serial.println(topic);

  Serial.print("Payload: ");
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';  
  Serial.println(msg);

  // kontrol Penyiram
  if (!strcmp(topic, SUBTOPIC_PENYIRAM)) {
    if (!strncmp(msg, "on", length)) {
      digitalWrite(relay_penyiram, LOW);
      Serial.println("Penyiram manual ON");
      pompa5 = "Menyala";
    } else if (!strncmp(msg, "off", length)) {
      digitalWrite(relay_penyiram, HIGH);
      Serial.println("Penyiram manual OFF");
      pompa5 = "Padam";
    }
  }

  // kontrol mainTask (test)
  if (!strcmp(topic, SUBTOPIC_TEST)) {
    if (!strncmp(msg, "on", length)) {
      kondisi = true;
      //Serial.println("ON");
    } else if (!strncmp(msg, "off", length)) {
      kondisi = false;
      //Serial.println("OFF");
    }
  }

  // Set pH
  if (!strcmp(topic, SUBTOPIC_SETPH)) {
    set_pH = atof(msg); 
    Serial.print("Set pH menjadi : ");
    Serial.println(set_pH);
  }

  // Set PPM
  if (!strcmp(topic, SUBTOPIC_SETPPM)) {
    set_PPM = atof(msg); 
    Serial.print("Set PPM menjadi : ");
    Serial.println(set_PPM);
  }
  
  // Set Waktu
  /*
  if (!strcmp(topic, SUBTOPIC_TEST)) {
    menit = atoi(msg); 
    Serial.print("Set menit menjadi : ");
    Serial.println(menit);
  }
  */
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    String clientId = "esp32-clientId-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected");
      Serial.print("Status OTA : ");
      Serial.println(OTA_msg);
      client.publish(PUBTOPIC_OTA, OTA_msg.c_str());
      
      client.subscribe(SUBTOPIC_PENYIRAM);
      client.subscribe(SUBTOPIC_SETPH);
      client.subscribe(SUBTOPIC_SETPPM);
      client.subscribe(SUBTOPIC_TEST);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      delay(3000);
    }
  }
}

void baca_RTC(){

  // Membuat string pesan dengan format yang diinginkan
  /*
  DateTime now = rtc.now();
  String pesan = ">> ";
  pesan += String(now.year());
  pesan += '/';
  pesan += String(now.month());
  pesan += '/';
  pesan += String(now.day());
  pesan += " (";
  pesan += daysOfTheWeek[now.dayOfTheWeek()];
  pesan += ") - ";
  pesan += String(now.hour());
  pesan += ':';
  pesan += String(now.minute());
  pesan += ':';
  pesan += String(now.second());

  // Kirimkan pesan ke MQTT
  client.publish(PUBTOPIC_WAKTU, pesan.c_str());
  */
  //Serial monitor
  /*
  Serial.print("Current time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(" - ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
*/

  //LCD
  /*
  lcd.setCursor(0,0);
  lcd.print(now.year(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.print(" - ");
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  */
}

void outputFuzzy (){
  baca_pH();
  baca_PPM();
  Momen();
  lamaPompa_pH = deffuzzyfikasi_pH();
  lamaPompa_PPM = deffuzzyfikasi_PPM();
}

void setupOTA() {
    ArduinoOTA.setHostname("esp8266 - Nutrisi Otomatis");
    ArduinoOTA.setPassword(ota_password);

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_SPIFFS
            type = "filesystem";
        }
        // Notifikasi OTA dimulai
        Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress * 100) / total);
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });

    ArduinoOTA.begin();
    Serial.println("OTA ready with authentication enabled");
}

void spreadsheet(){
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);

    String Send_Data_URL = Web_App_URL + "?sts=write";
    Send_Data_URL += "&pH=" + String (pH);
    Send_Data_URL += "&PPM=" + String(PPM);
    Send_Data_URL += "&tinggi_cm=" + String(tinggi_cm);
    Send_Data_URL += "&suhu=" + String(suhu);
    Send_Data_URL += "&status=" + status;

    Serial.println();
    Serial.println("-------------");
    Serial.println("Send data to Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Send_Data_URL);

      WiFiClientSecure client;
      client.setInsecure(); // Nonaktifkan validasi SSL
      HTTPClient http;
      http.begin(client, Send_Data_URL);  
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      int httpCode = http.GET(); 
      Serial.print("HTTP Status Code : ");
      Serial.println(httpCode);
      String payload;
      if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload : " + payload);    
      }
      http.end();
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("-------------");
  }
}

void readSpreadsheetData() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);

    String Read_Data_URL = Web_App_URL + "?sts=read";
    Serial.println();
    Serial.println("-------------");
    Serial.println("Read data from Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Read_Data_URL);

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.begin(client, Read_Data_URL);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    int httpCode = http.GET();
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Payload : " + payload);
      dataSpreadsheet = payload.toInt();  // Convert payload to integer
    }
    
    http.end();
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("-------------");
  }
}
