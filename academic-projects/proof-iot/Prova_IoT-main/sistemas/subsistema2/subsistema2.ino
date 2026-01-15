#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "DHT.h"
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

// =========================
// CONFIG Wi-Fi
// =========================
const char* ssid = "Iphone do pavarin";
const char* password = "pavariniii";

// =========================
// CONFIG MQTT HiveMQ Cloud
// =========================
const char* mqtt_server = "21862f7b718544f49677b82f78c2215d.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "pavarin";
const char* mqtt_password = "1234567Ccc";

// =========================
// TOPICOS MQTT
// =========================
#define TOPICO_PUBLISH "empresa01/area_prototipagem/ambiente/dado"
#define TOPICO_SUBSCRIBE "empresa01/area_prototipagem/estado/consolidado"
#define TOPICO_LWT "empresa01/area_prototipagem/status/umaf"

// =========================
// PINOS
// =========================
#define DHT_PIN 4
#define DHT_TYPE DHT11
#define OLED_SDA 21
#define OLED_SCL 22
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LED_PIN 16
#define NUM_LEDS 1

// =========================
// OBJETOS
// =========================
WiFiClientSecure espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_NeoPixel led(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// =========================
// VARIAVEIS
// =========================
unsigned long lastMsg = 0;
char msg[100];
float tempAtual = 0;
float umidAtual = 0;
String estadoAtual = "Aguardando";
int ocupacaoAtual = 0;
int limiteAtual = 5;

// =========================
// WIFI
// =========================
void setup_wifi() {
  Serial.print("Conectando ao WiFi ");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println("\nWiFi OK!");
  Serial.println(WiFi.localIP());
}

// =========================
// DISPLAY OLED
// =========================
void atualizarDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Linha 1: Status da Sala
  display.setCursor(0, 0);
  display.print("Status: ");
  display.setTextSize(1);
  display.println(estadoAtual);
  
  // Linha 2: Ocupação
  display.setCursor(0, 16);
  display.print("Ocupacao: ");
  display.print(ocupacaoAtual);
  display.print("/");
  display.println(limiteAtual);
  
  // Linha 3: Temperatura
  display.setCursor(0, 32);
  display.print("Temp: ");
  display.print(tempAtual, 1);
  display.println(" C");
  
  // Linha 4: Umidade
  display.setCursor(0, 48);
  display.print("Umid: ");
  display.print(umidAtual, 1);
  display.println(" %");
  
  display.display();
}

// =========================
// LED RGB
// =========================
void atualizarLED(String estado) {
  if (estado == "Livre")
    led.setPixelColor(0, led.Color(0, 255, 0));    // Verde
  else if (estado == "Atenção")
    led.setPixelColor(0, led.Color(255, 255, 0));  // Amarelo
  else if (estado == "Alerta")
    led.setPixelColor(0, led.Color(255, 0, 0));    // Vermelho
  else
    led.setPixelColor(0, led.Color(0, 0, 255));    // Azul (padrão)

  led.show();
}

// =========================
// CALLBACK MQTT
// =========================
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String mensagem = String((char*)payload);

  Serial.println("Recebido do Orquestrador: " + mensagem);

  // Parse do JSON recebido
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, mensagem);
  
  if (!error) {
    estadoAtual = doc["status_area"].as<String>();
    ocupacaoAtual = doc["ocupacao"];
    limiteAtual = doc["limite_op"];
    
    Serial.println("Estado: " + estadoAtual);
    Serial.println("Ocupação: " + String(ocupacaoAtual));
    Serial.println("Limite: " + String(limiteAtual));
    
    atualizarLED(estadoAtual);
    atualizarDisplay();
  } else {
    Serial.println("Erro ao parsear JSON do Orquestrador");
  }
}

// =========================
// CONECTAR AO MQTT - CORRIGIDO
// =========================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT HiveMQ...");

    // Gera um ClientID único para evitar conflitos
    String clientId = "UMAF_";
    clientId += String(random(0xffff), HEX);
    
    // Configura o certificado para TLS
    espClient.setInsecure(); // Usa certificado padrão
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password, TOPICO_LWT, 1, true, "offline")) {
      Serial.println("Conectado ao HiveMQ!");
      
      // Subscribe no tópico
      if (client.subscribe(TOPICO_SUBSCRIBE)) {
        Serial.println("Subscribed no tópico: " + String(TOPICO_SUBSCRIBE));
      } else {
        Serial.println("Falha no subscribe");
      }
      
      // Publica status online
      client.publish(TOPICO_LWT, "online", true);
      Serial.println("Status online publicado");

    } else {
      Serial.print("Falha na conexão, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      
      // Mostra o erro específico
      switch (client.state()) {
        case -4: Serial.println("MQTT_CONNECTION_TIMEOUT"); break;
        case -3: Serial.println("MQTT_CONNECTION_LOST"); break;
        case -2: Serial.println("MQTT_CONNECT_FAILED"); break;
        case -1: Serial.println("MQTT_DISCONNECTED"); break;
        case 1: Serial.println("MQTT_CONNECT_BAD_PROTOCOL"); break;
        case 2: Serial.println("MQTT_CONNECT_BAD_CLIENT_ID"); break;
        case 3: Serial.println("MQTT_CONNECT_UNAVAILABLE"); break;
        case 4: Serial.println("MQTT_CONNECT_BAD_CREDENTIALS"); break;
        case 5: Serial.println("MQTT_CONNECT_UNAUTHORIZED"); break;
        default: Serial.println("Erro desconhecido"); break;
      }
      
      delay(5000);
    }
  }
}

// =========================
// SETUP
// =========================
void setup() {
  Serial.begin(115200);
  randomSeed(micros());

  Serial.println("Iniciando UMAF...");
  
  // Inicializa hardware
  Wire.begin(OLED_SDA, OLED_SCL);
  dht.begin();
  led.begin();
  led.show();

  // Inicializa OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro no OLED!");
    while(true);
  }

  // Display inicial
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Iniciando UMAF");
  display.setCursor(0, 16);
  display.println("Conectando WiFi...");
  display.display();
  
  // Conecta WiFi
  setup_wifi();

  // Configura MQTT
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.setBufferSize(512); // Aumenta buffer para mensagens grandes
  client.setKeepAlive(60);

  // Atualiza display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Conectado!");
  display.setCursor(0, 16);
  display.println("IP: ");
  display.setCursor(0, 24);
  display.println(WiFi.localIP());
  display.setCursor(0, 40);
  display.println("Conectando MQTT...");
  display.display();
}

// =========================
// LOOP
// =========================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();

  if (now - lastMsg > 5000) {
    lastMsg = now;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      tempAtual = t;
      umidAtual = h;

      // Cria JSON corretamente
      StaticJsonDocument<200> doc;
      doc["temperatura"] = t;
      doc["umidade"] = h;
      
      String output;
      serializeJson(doc, output);
      
      Serial.println("Publicando: " + output);
      
      if (client.publish(TOPICO_PUBLISH, output.c_str(), true)) {
        Serial.println("Dados ambientais publicados!");
      } else {
        Serial.println("Falha ao publicar dados ambientais");
      }

      // Atualiza o display
      atualizarDisplay();
      
    } else {
      Serial.println("Erro na leitura do DHT11");
    }
  }
}