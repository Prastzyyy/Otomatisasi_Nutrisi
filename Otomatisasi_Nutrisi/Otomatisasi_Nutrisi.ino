#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
//#include <RTClib.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
//#include "fuzzy_function.h"

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

//const char* ssid ="bebas";
//const char* password = "akunulisaja";
const char* ssid ="Prastzy.net";
const char* password = "123456781";

//Deklarasi Pin
/*
#define pin_pH 34  //analog pin
#define pin_EC 33  //analog pin
#define pin_TRIG 12
#define pin_ECHO 14
*/
#define relay_pHup 2   //D4
#define relay_pHdn 14  //D5
#define relay_nutrisi 12  //D6
#define relay_ambilSampel 0  //D3
#define relay_buangSampel 4  //D2
#define relay_penyiram 15  //D8
#define relay_tangki 5  //D1
#define relay_mixer 13  //D2

//#define ONE_WIRE_BUS D0
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27,20,4);
//RTC_DS1307 rtc;
//int setWaktu1 [] = {8,0,0}; //jam, menit, detik
int detik;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

float set_jarak = 100;
float pH, PPM, selisih_pH, selisih_PPM, set_pH = 7, set_PPM = 100;
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
  //pinMode(pin_ECHO, INPUT);
  
  //pinMode(pin_TRIG, OUTPUT);
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
  /*
  lcd.init();                      
  lcd.backlight();
  lcd.setCursor(3,1);
  lcd.print("START  PROGRAM");
  lcd.setCursor(3,2);
  lcd.print("==============");
  delay(1000);
  lcd.clear();
  */
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
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  task.attach(1, monitoring);
}

void loop() { 
  if (!client.connected()) {
          reconnect();
  }
  client.loop();
  mainTask ();
}

//=========================================================
void mainTask (){
    //if (now.hour() == setWaktu1[0] && now.minute() == setWaktu1[1] && now.second() == setWaktu1[2] ){
    //DateTime now = rtc.now();
    //if (now.second() == detik ){
    if (kondisi == true){ //pengganti rtc 
      Serial.println("==================================");
      Serial.println("kondisi didalam mainTask ");
      Serial.println("==================================");

      Serial.println("menjalankan kontrol_tinggi ");
      kontrol_tinggi();

      Serial.println("Mengaktifkan relay_buangSampel");
      digitalWrite (relay_buangSampel, LOW); 
      Serial.println("delay 5 detik ");
      delay(5000);
      Serial.println("Menonaktifkan relay_buangSampel");
      digitalWrite (relay_buangSampel, HIGH); 

      Serial.println("Mengaktifkan relay_ambilSampel");
      digitalWrite (relay_ambilSampel, LOW); 
      Serial.println("delay 5 detik ");
      delay(5000);
      Serial.println("Menonaktifkan relay_ambilSampel");
      digitalWrite (relay_ambilSampel, HIGH); 

      //data sensor sebelum diotomatisasi

      Serial.println("menjalankan kontrol_pH");
      kontrol_pH();
      Serial.println("menjalankan kontrol_PPM");
      kontrol_PPM(); 

      Serial.println("Mengaktifkan relay_mixer");
      digitalWrite (relay_mixer, LOW);  
      Serial.println("delay 5 detik ");
      delay(5000);
      Serial.println("Menonaktifkan relay_mixer");
      digitalWrite (relay_mixer, HIGH); 

      Serial.println("Mengaktifkan relay_buangSampel");
      digitalWrite (relay_buangSampel, LOW); 
      Serial.println("delay 5 detik ");
      delay(5000);
      Serial.println("Menonaktifkan relay_buangSampel");
      digitalWrite (relay_buangSampel, HIGH); 

      Serial.println("Mengaktifkan relay_ambilSampel");
      digitalWrite (relay_ambilSampel, LOW); 
      Serial.println("delay 5 detik ");
      delay(5000);
      Serial.println("Menonaktifkan relay_ambilSampel");
      digitalWrite (relay_ambilSampel, HIGH);  

      //data sensor setelah diotomatisasi

      Serial.println("Mengaktifkan relay_penyiram");
      digitalWrite (relay_penyiram, LOW); 
      Serial.println("delay 5 detik ");
      delay(5000);
      Serial.println("Menonaktifkan relay_penyiram");
      digitalWrite (relay_penyiram, HIGH); 

      Serial.println("menjalankan kontrol_tinggi ");
      kontrol_tinggi();
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
    //baca_RTC();
    //Serial.print("pH : "); Serial.println(pH);
    //Serial.print("EC : "); Serial.println(PPM);
    //Serial.print("Suhu : "); Serial.println(suhu);
    //Serial.print("Ketinggian Air (cm): "); Serial.println(tinggi_cm);
    //Serial.print("Output fuzzy pH : "); Serial.println(p_hasil);
    //Serial.print("Output fuzzy nutrisi : "); Serial.println(n_hasil);
    //Serial.println("");

    //Print LCD
    lcd.setCursor(0,1); 
    lcd.print("nilai pH : "); lcd.print(pH);
    lcd.setCursor(0,2); 
    lcd.print("nilai EC : "); lcd.print(PPM);
    lcd.setCursor(0,3); 
    lcd.print("Nilai Suhu : "); lcd.print(suhu); lcd.print(" C");

    //Print MQTT
    client.publish(PUBTOPIC_PH, String(pH).c_str());
    client.publish(PUBTOPIC_PPM, String(PPM).c_str());
    client.publish(PUBTOPIC_SUHU, String(suhu).c_str());
    client.publish(PUBTOPIC_TINGGI, String(tinggi_cm).c_str());
}

void baca_pH(){
  //float potValue1 = analogRead(pin_pH);
  //pH = potValue1 / 4095 * 14;
  int randomNumber = random(0, 21);  
  //angka random dari 6 sampai 8 dengan interval 0,1
  pH = 6.0 + (randomNumber * 0.1);

  selisih_pH = abs(set_pH - pH);
  p_inputf = selisih_pH;
}

void kontrol_pH(){
  baca_pH();
  outputFuzzy();
  if (pH < set_pH  ){
    Serial.println("Mengaktifkan relay_pHup");
    digitalWrite(relay_pHup, LOW);
    digitalWrite(relay_pHdn, HIGH);
    Serial.print("delay : ");Serial.println(p_hasil);
    delay(p_hasil);
    Serial.println("Menonaktifkan relay_pHup");
    digitalWrite(relay_pHup, HIGH);
    digitalWrite(relay_pHdn, HIGH);
  }
  else if (pH >= set_pH  ){
    Serial.println("Mengaktifkan relay_pHdn");
    digitalWrite(relay_pHup, HIGH);
    digitalWrite(relay_pHdn, LOW);
    Serial.print("delay : ");Serial.println(p_hasil);
    delay(p_hasil);
    Serial.println("Menonaktifkan relay_pHdn");
    digitalWrite(relay_pHup, HIGH);
    digitalWrite(relay_pHdn, HIGH);
  }
}

void baca_PPM(){
  //float potValue2 = analogRead(pin_EC);
  //PPM = potValue2 / 4095 * 100;
  PPM = random(30, 101); 
  if (PPM <= set_PPM){
    selisih_PPM = set_PPM - PPM;
  }
  else if (PPM > set_PPM){
    selisih_PPM = 0;
  }
  n_inputf = selisih_PPM;
}

void kontrol_PPM(){
  baca_PPM();
  outputFuzzy();
  if (PPM < set_PPM  ){
    Serial.println("Mengaktifkan relay_nutrisi");
    digitalWrite(relay_nutrisi, LOW);
    Serial.print("delay : ");Serial.println(n_hasil);
    delay(n_hasil);
    Serial.println("Menonaktifkan relay_nutrisi");
    digitalWrite(relay_nutrisi, HIGH);
  }
  else if (PPM >= set_PPM  ){
    Serial.println("Menonaktifkan relay_nutrisi");
    digitalWrite(relay_nutrisi, HIGH);
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
  while (tinggi_cm < set_jarak - 50) {
    Serial.println("Mengaktifkan relay_tangki");
    digitalWrite(relay_tangki, LOW);
  }
  Serial.println("delay sampai tinggi air terpenuhi");
  delay(2000);
  Serial.println("Menonaktifkan relay_tangki");
  digitalWrite(relay_tangki, HIGH);
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
    } else if (!strncmp(msg, "off", length)) {
      digitalWrite(relay_penyiram, HIGH);
      Serial.println("Penyiram manual OFF");
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
  /*
  // Set Waktu
  if (!strcmp(topic, SUBTOPIC_TEST)) {
    detik = atoi(msg); 
    Serial.print("Set detik menjadi : ");
    Serial.println(detik);
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
/*
void baca_RTC(){
  // Membuat string pesan dengan format yang diinginkan
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
  /*
  //Serial monitor
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
  //LCD
  
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
  
}
*/
void outputFuzzy (){
  baca_pH();
  baca_PPM();
  //p_moment();
  //n_moment();
  //p_hasil = p_deffuzzyfikasi();
  //n_hasil = n_deffuzzyfikasi();
  p_hasil = p_inputf * 10000;
  n_hasil = n_inputf * 100;
}
