//mqtt_function.cpp

#include "mqtt_function.h"

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
      digitalWrite(relay_penyiram, HIGH);
    } else if (!strncmp(msg, "off", length)) {
      digitalWrite(relay_penyiram, LOW);
    }
  }

  // Set pH
  if (!strcmp(topic, SUBTOPIC_SETPH)) {
    set_pH = atoi(msg); 
    Serial.print("Set pH menjadi : ");
    Serial.println(set_pH);
  }

  // Set PPM
  if (!strcmp(topic, SUBTOPIC_SETPPM)) {
    set_PPM = atoi(msg); 
    Serial.print("Set PPM menjadi : ");
    Serial.println(set_PPM);
  }
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
      client.subscribe(SUBTOPIC_SETPH);
      client.subscribe(SUBTOPIC_SETEC);
      client.subscribe(SUBTOPIC_TEST);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      delay(3000);
    }
  }
}

