#include <WiFi.h>
#include <PubSubClient.h>

const char* SSID = "Iphone do pavarin";
const char* PSWD = "pavarinii";
const char* brokerUrl = "test.mosquitto.org";
const int brokerPort = 1883;
const char* topic = "AulaIoTSul/Chat";
const String userName = "Pavarin";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastWiFiCheck = 0;
unsigned long lastMQTTCheck = 0;

void setup() {
  Serial.begin(115200);
  
  mqttClient.setServer(brokerUrl, brokerPort);
  mqttClient.setCallback(mqttCallback);
  
  connectToWiFi();
  Serial.println("Sistema de Chat MQTT Iniciado");
  Serial.println("Digite uma mensagem e pressione ENTER:");
}

void loop() {
  unsigned long now = millis();

  // Reconectar WiFi
  if (now - lastWiFiCheck > 5000) {
    lastWiFiCheck = now;
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi desconectado. Reconectando...");
      connectToWiFi();
    }
  }

  // Reconectar MQTT
  if (now - lastMQTTCheck > 5000) {
    lastMQTTCheck = now;
    if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
      connectToMqtt();
    }
  }

  mqttClient.loop();

  // Ler mensagem do Serial
  if (Serial.available() > 0) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      sendMessage(msg);
    }
  }
}


void connectToWiFi() {
  WiFi.begin(SSID, PSWD);
}

void connectToMqtt() {
  String clientId = "ESP32-Chat-" + String(random(0xffff), HEX);
  if (mqttClient.connect(clientId.c_str())) {
    Serial.println("Conectado ao broker MQTT!");
    mqttClient.subscribe(topic);
  } else {
    Serial.print("Falha MQTT: ");
    Serial.println(mqttClient.state());
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
}

void sendMessage(String msg) {
  if (mqttClient.connected()) {
    String fullMessage = userName + ": " + msg;
    mqttClient.publish(topic, fullMessage.c_str());
    Serial.println("Você: " + msg);
  } else {
    Serial.println("Erro: MQTT desconectado");
  }
}