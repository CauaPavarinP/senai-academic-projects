#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define LED_VERMELHO 4
#define POT_PIN 8                   // pino do potenciômetro atualizado
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const char* ssid = "Iphone do pavarin";      // Wi-Fi da atividade anterior
const char* password = "pavarinii";          // Senha da atividade anterior
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long alertaAtivo = 0;
unsigned long tempoReset = 0;
bool alerta = false;

void publicarReset() {
  StaticJsonDocument<80> doc;
  doc["comando"] = "reset";
  char msg[80];
  serializeJson(doc, msg);
  client.publish("dupla15/seguranca/controle", msg); // mesma dupla
  Serial.println("Comando de reset enviado!");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Sistema normal.");
  display.display();
  digitalWrite(LED_VERMELHO, LOW);
  alerta = false;
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String msg = String((char*)payload);
  Serial.println("Recebido: " + msg);

  StaticJsonDocument<100> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (error) return;

  const char* evento = doc["evento"];
  if (strcmp(evento, "movimento") == 0) {
    alerta = true;
    alertaAtivo = millis();

    int potValue = analogRead(POT_PIN);
    tempoReset = map(potValue, 0, 4095, 0, 30000);

    digitalWrite(LED_VERMELHO, HIGH);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("ALERTA!");
    display.println("Movimento Detectado!");
    display.display();

    Serial.printf("Tempo para reset: %.1f s\n", tempoReset / 1000.0);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("esp32_central_monitoramento")) {
      client.subscribe("dupla15/seguranca/eventos"); // mesma dupla
      Serial.println("Conectado ao MQTT!");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_VERMELHO, OUTPUT);
  digitalWrite(LED_VERMELHO, LOW);

  // Inicializa I2C com pinos personalizados
  Wire.begin(21, 22); // SDA, SCL

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro ao iniciar OLED");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Sistema normal.");
  display.display();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if (alerta && (millis() - alertaAtivo > tempoReset)) {
    publicarReset();
  }
}
