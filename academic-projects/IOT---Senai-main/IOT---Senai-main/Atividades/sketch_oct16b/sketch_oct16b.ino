#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// === CONFIGURAÇÕES OLED ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// === CONFIGURAÇÕES SERVO ===
#define SERVO_PIN 15
Servo servo;

// === CONFIGURAÇÕES Wi-Fi e MQTT ===
const char* SSID = "Iphone do pavarin";
const char* PASSWORD = "pavarinii";
const char* BROKER = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char* TOPIC = "dupla15/servo/comando";

WiFiClient espClient;
PubSubClient client(espClient);

// === FUNÇÃO CALLBACK (recebe MQTT) ===
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

  int angulo = doc["angulo"];
  servo.write(angulo);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Servo Remoto");
  display.setCursor(0, 30);
  display.print("Angulo: ");
  display.print(angulo);
  display.println("°");
  display.display();
}

// === FUNÇÃO PARA CONECTAR AO MQTT ===
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("ESP32_Servo")) {
      Serial.println("Conectado!");
      client.subscribe(TOPIC);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}

// === FUNÇÃO PARA CONECTAR AO WI-FI COM TIMEOUT ===
bool conectarWiFi() {
  WiFi.begin(SSID, PASSWORD);
  int tentativas = 0;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,10);
  display.println("Conectando WiFi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED && tentativas < 20) { // ~10s
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.setCursor(0,10);
    display.println("WiFi conectado!");
    display.display();
    return true;
  } else {
    Serial.println("\nFalha ao conectar WiFi");
    display.clearDisplay();
    display.setCursor(0,10);
    display.println("Falha WiFi!");
    display.display();
    return false;
  }
}

void setup() {
  Serial.begin(115200);

  // Inicializa I2C OLED
  Wire.begin(21, 22); // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Falha ao iniciar OLED!");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,10);
  display.println("Iniciando...");
  display.display();

  // Inicializa servo
  servo.attach(SERVO_PIN);
  servo.write(90); // posição inicial

  // Conecta Wi-Fi
  bool conectado = conectarWiFi();
  if (!conectado) {
    Serial.println("Continuando sem Wi-Fi, aguardando conexão...");
  }

  // Configura MQTT
  client.setServer(BROKER, BROKER_PORT);
  client.setCallback(callback);
}

void loop() {
  // Tenta reconectar Wi-Fi se desconectado
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }

  // Tenta reconectar MQTT se desconectado
  if (!client.connected()) reconnectMQTT();

  client.loop();
}
