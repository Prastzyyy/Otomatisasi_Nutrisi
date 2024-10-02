#include <WiFi.h>
#include <PubSubClient.h>

#include "fuzzy_function.h"
#include "mqtt_function.h"

#include <stdio.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "driver/gpio.h"

#include <Arduino_FreeRTOS.h>

#include "RTClib.h"
#include <LiquidCrystal_I2C.h>

//Deklarasi Pin
#define pin_pH 34
#define pin_EC 
#define pin_TRIG 12
#define pin_ECHO 14
#define relay_pHup 18
#define relay_pHdn 5
#define relay_nutrisiA 16
#define relay_nutrisiB 4
#define relay_ambilSampel 0
#define relay_buangSampel 2
#define relay_penyiram 15
#define relay_tangki 23
#define relay_mixer 27

LiquidCrystal_I2C lcd(0x27,20,4);
RTC_DS1307 rtc;
int setWaktu1 [] = {21,44,1}; //jam, menit, detik

WiFiClient espClient;
PubSubClient client(espClient);

float set_jarak = 100;
float pH, PPM, selisih_pH, selisih_PPM;
int duration;
float tinggi_cm, jarak_cm, jarak_inci;

void baca_pH();
void baca_PPM();
void baca_jarak();
void kontrol_pH();
void kontrol_PPM();
void kontrol_tinggi();
void baca_RTC();
void outputFuzzy();

void monitoringTask (void *pvParameters);
void manualTask (void *pvParameters);
void mainTask (void *pvParameters);

void setup() {
  Serial.begin(115200);
  pinMode(pin_pH, INPUT);
  pinMode(pin_EC, INPUT);
  pinMode(pin_ECHO, INPUT);
  pinMode(pin_TRIG, OUTPUT);
  pinMode(relay_pHup, OUTPUT);
  pinMode(relay_pHdn, OUTPUT);
  pinMode(relay_nutrisiA, OUTPUT);
  pinMode(relay_nutrisiB, OUTPUT);
  pinMode(relay_penyiram, OUTPUT);
  pinMode(relay_tangki, OUTPUT);
  pinMode(relay_ambilSampel, OUTPUT);
  pinMode(relay_buangSampel, OUTPUT);

  setupWiFi();
  lcd.init();                      
  lcd.backlight();
  lcd.setCursor(3,1);
  lcd.print("START  PROGRAM");
  lcd.setCursor(3,2);
  lcd.print("==============");
  delay(1000);
  lcd.clear();
  if (! rtc.begin()) {
    Serial.println("RTC tidak ditemukan");
    Serial.flush();
    abort();
  }
  //core 0
  xTaskCreatePinnedToCore(monitoringTask, "monitoringTask", 8198, NULL, 2, NULL, 0); 
  //core 1
  xTaskCreatePinnedToCore(manualTask, "manualTask", 8198, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(mainTask, "mainTask", 8198, NULL, 0, NULL, 1); 
}

void loop() {
  
}

void monitoringTask (void *pvParameters){
  while(true){
    if (!client.connected()) {
        reconnect();
    }
      client.loop();

    baca_pH(); //pH
    baca_PPM(); //PPM
    baca_jarak(); //jarak
    DateTime now = rtc.now(); //waktu
    outputFuzzy(); //output fuzzy

    //Serial Monitor
    baca_RTC();
    Serial.print("pH : "); Serial.println(pH);
    Serial.print("EC : "); Serial.println(PPM);
    Serial.print("Ketinggian Air (cm): "); Serial.println(tinggi_cm);
    Serial.print("Output pompa pH (detik): "); Serial.println(p_hasil);
    Serial.print("Output pompa nutrisi (detik): "); Serial.println(n_hasil);

    //Print LCD
    lcd.setCursor(0,1); 
    lcd.print("nilai pH : "); lcd.print(pH);
    lcd.setCursor(0,2); 
    lcd.print("nilai EC : "); lcd.print(PPM);
    lcd.setCursor(0,3); 
    lcd.print("Ketinggian Air : "); lcd.print(tinggi_cm);

    //Print MQTT
    client.publish(PUBTOPIC_PH, String(pH).c_str());
    client.publish(PUBTOPIC_PPM, String(PPM).c_str());
    client.publish(PUBTOPIC_TINGGI, String(tinggi_cm).c_str());

    vTaskDelay( 2000 / portTICK_PERIOD_MS );
  }
}

void mainTask(void *pvParameters) {
  while (true) {
    //if (now.hour() == setWaktu1[0] && now.minute() == setWaktu1[1] && now.second() == setWaktu1[2] ){
    if (sw == HIGH){ //pengganti rtc 

      kontrol_tinggi(); //memastikan air dalam tamgki sesuai ketetapan

      digitalWrite (relay_buangSampel, HIGH); delay (5000); //buang sampel air lawas
      digitalWrite (relay_buangSampel, LOW); 
      digitalWrite (relay_ambilSampel, HIGH); delay (5000); //Ambil sampel air baru untuk kontrol 
      digitalWrite (relay_ambilSampel, LOW); 

      kontrol_pH(); //menjalankan prosedur penambahan larutan pH
      kontrol_PPM(); //menjalankan prosedur penambahan nutrisi A dan B

      digitalWrite (relay_mixer, HIGH); delay (5000); //menyalakan mixer
      digitalWrite (relay_mixer, LOW); 
      digitalWrite (relay_buangSampel, HIGH); delay (5000); //buang sampel air lawas
      digitalWrite (relay_buangSampel, LOW); 
      digitalWrite (relay_ambilSampel, HIGH); delay (5000); //Ambil sampel air baru untuk monitoring
      digitalWrite (relay_ambilSampel, LOW); 
      digitalWrite (relay_penyiram, HIGH); delay (5000); //melakukan penyiraman
      digitalWrite (relay_mixer, LOW); 

      kontrol_tinggi(); //memastikan air dalam tamgki sesuai ketetapan

      vTaskDelay(100 / portTICK_PERIOD_MS); // Delay 100ms
    }
  }
}

void manualTask(void *pvParameters) {
  while (true) {
    if (!client.connected()) {
          reconnect();
      }
      client.loop();

      // Perintah kontrol dihandle melalui fungsi callback MQTT

    vTaskDelay(100 / portTICK_PERIOD_MS); // Delay 100ms
  }
}

void baca_pH(){
  float potValue1 = analogRead(pin_pH);
  pH = potValue1 / 4095 * 14;
  selisih_pH = abs(set_pH - pH);
  p_inputf = selisih_pH;
}

void kontrol_pH(){
  baca_pH();
  outputFuzzy();
  if (pH < set_pH  ){
    digitalWrite(relay_pHup, HIGH);
    digitalWrite(relay_pHdn, LOW);
    delay(p_hasil);
    digitalWrite(relay_pHup, LOW);
    digitalWrite(relay_pHdn, LOW);
  }
  else if (pH >= set_pH  ){
    digitalWrite(relay_pHup, LOW);
    digitalWrite(relay_pHdn, HIGH);
    delay(p_hasil);
    digitalWrite(relay_pHup, LOW);
    digitalWrite(relay_pHdn, LOW);
  }
}

void baca_PPM(){
  float potValue2 = analogRead(pin_EC);
  PPM = potValue2 / 4095 * 100;
  if (selisih_pH <= set_PPM){
    selisih_PPM = set_PPM - PPM;
  }
  else if (selisih_pH > set_PPM){
    selisih_PPM = 0;
  }
  n_inputf = selisih_PPM;
}

void kontrol_PPM(){
  baca_PPM();
  outputFuzzy();
  if (PPM < set_PPM  ){
    digitalWrite(relay_nutrisiA, HIGH);
    digitalWrite(relay_nutrisiB, HIGH);
    delay(n_hasil);
    digitalWrite(relay_nutrisiA, LOW);
    digitalWrite(relay_nutrisiB, LOW);
  }
  else if (PPM >= set_PPM  ){
    digitalWrite(relay_nutrisiA, LOW);
    digitalWrite(relay_nutrisiB, LOW);
  }
}

void baca_jarak(){
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  duration = pulseIn(PIN_ECHO, HIGH);
  //jarak_inci = duration / 148; //dalam inci
  jarak_cm = duration / 58; //dalam cm
  tinggi_cm = set_jarak - jarak_cm;
}

void kontrol_tinggi(){
  baca_jarak();
  do {
    digitalWrite(relay_tangki, HIGH);
  }while(tinggi_cm < set_jarak);
}

void baca_RTC(){
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

void outputFuzzy (){
  baca_pH();
  baca_PPM();
  p_moment();
  n_moment();
  p_hasil = p_deffuzzyfikasi();
  n_hasil = n_deffuzzyfikasi();
}

