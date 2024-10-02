//mqtt_function.h

#ifndef MQTT_FUNCTION_H
#define MQTT_FUNCTION_H
#include <Arduino.h>

const char* SUBTOPIC_SETPH = "greenhouse/input/setph";
const char* SUBTOPIC_SETPPM = "greenhouse/input/setppm";
const char* SUBTOPIC_PENYIRAM = "greenhouse/input/penyiram";

const char* SUBTOPIC_TEST = "greenhouse/input/test";

extern const char* PUBTOPIC_PH = "greenhouse/output/ph";
extern const char* PUBTOPIC_PPM = "greenhouse/output/ppm";
extern const char* PUBTOPIC_TINGGI = "greenhouse/output/tinggi";

extern float set_pH;
extern float set_PPM;

const char* ssid ="Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "ee.unsoed.ac.id";

void setup_wifi();
void reconnect();
void callback(char *topic, byte *payload, unsigned int length);

#endif
