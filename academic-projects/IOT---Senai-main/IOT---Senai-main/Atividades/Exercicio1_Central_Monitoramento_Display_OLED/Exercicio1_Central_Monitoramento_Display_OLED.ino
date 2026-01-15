#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === CONFIGURAÇÕES OLED ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// === CONFIGURAÇÕES WiFi e MQTT ===
const char* SSID = "Iphone do pavarin";
const char* PSWD = "pavarinii";
const char* BROKER = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char* TOPIC = "dupla15/estacao/dados";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// === FUNÇÕES ===
void conectarWiFi() {
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(SSID, PSWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void conectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (mqttClient.connect("MonitorCentral")) {
      Serial.println("Conectado!");
      mqttClient.subscribe(TOPIC);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000); // Espera antes de tentar novamente
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("Mensagem recebida: ");
  Serial.println(msg);

  StaticJsonDocument<200> doc;
  DeserializationError erro = deserializeJson(doc, msg);
  if (erro) {
    Serial.println("Erro ao interpretar JSON!");
    return;
  }

  float distancia = doc["distancia_cm"];
  bool movimento = doc["movimento"];
  int limiar = doc["limiar_pot"];

  // Atualiza o display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("CENTRAL DE MONITORAMENTO");
  display.println("-------------------------");
  display.setCursor(0,20);
  display.print("Distancia: "); display.print(distancia); display.println(" cm");
  display.print("Movimento: "); display.println(movimento ? "Detectado" : "Sem Movimento");
  display.print("Limiar: "); display.println(limiar);
  display.display();
}

void setup() {
  Serial.begin(115200);

  // Inicializa I2C com pinos corretos
  Wire.begin(21,22);

  // Inicializa o display - tenta 0x3C e depois 0x3D
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C) &&
      !display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println("Erro ao inicializar display OLED!");
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,10);
  display.println("Iniciando...");
  display.display();

  conectarWiFi();
  mqttClient.setServer(BROKER, BROKER_PORT);
  mqttClient.setCallback(callback);
}

void loop() {
  if (!mqttClient.connected()) conectarMQTT();
  mqttClient.loop();
}
