#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>

#define SERVO_PIN 15
#define LED_PIN 4

// === MESMO WIFI DA ATIVIDADE ANTERIOR ===
const char* ssid = "Iphone do pavarin";
const char* password = "pavarinii";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
Servo servo;

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String jsonStr = String((char*)payload);
  Serial.println("Mensagem recebida: " + jsonStr);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, jsonStr);
  if (error) {
    Serial.println("Erro no JSON!");
    return;
  }

  const char* status = doc["status"];

  if (strcmp(status, "proximidade_detectada") == 0) {
    servo.write(90);
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Cancela FECHADA 🚫");
  }
  else if (strcmp(status, "area_livre") == 0) {
    servo.write(0);
    digitalWrite(LED_PIN, LOW);
    Serial.println("Cancela ABERTA ✅");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("esp32_controle_acesso")) {
      client.subscribe("dupla15/acesso/alerta");  // mesma dupla da atividade anterior
      Serial.println("Conectado ao MQTT!");
    } else {
      Serial.print("Falha, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  servo.attach(SERVO_PIN);

  // Conectar ao Wi-Fi
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  // Configurar MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}
