#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// =========================
// CONFIGURAÇÕES DE REDE
// =========================
const char* ssid = "Iphone do pavarin";
const char* password = "pavariniii";

// =========================
// CONFIGURAÇÕES MQTT (HiveMQ Cloud)
// =========================
const char* mqttServer = "21862f7b718544f49677b82f78c2215d.s1.eu.hivemq.cloud";
const int mqttPort = 8883;
const char* mqttUser = "pavarin";
const char* mqttPassword = "1234567Ccc";

const char* topicEvento = "empresa01/area_prototipagem/fluxo/evento";
const char* topicStatus = "empresa01/area_prototipagem/status/nca";

// =========================
// CLIENT ID único
// =========================
String clientID = "ESP32_NCA_" + String((uint32_t)ESP.getEfuseMac(), HEX);

// =========================
// PINOS ULTRASSÔNICOS
// =========================
#define TRIG1 20
#define ECHO1 21
#define TRIG2 23
#define ECHO2 22

// =========================
// VARIÁVEIS DE CONTROLE
// =========================
WiFiClientSecure espClient;
PubSubClient client(espClient);

int pessoaDentro = 0;
unsigned long ultimoEventoDetectado = 0;
const unsigned long intervaloMinimoEvento = 1500; // 1,5s entre eventos válidos
const unsigned long intervaloLoop = 200;           // Delay do loop

// =========================
// FUNÇÃO DE DISTÂNCIA
// =========================
float medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracao = pulseIn(echoPin, HIGH, 50000); // timeout 50ms
  float distancia = duracao * 0.034 / 2;
  return distancia;
}

// =========================
// MÉDIA DE MÚLTIPLAS LEITURAS
// =========================
float mediaDistancia(int trigPin, int echoPin, int nLeituras = 5) {
  float soma = 0;
  int count = 0;
  for (int i = 0; i < nLeituras; i++) {
    float d = medirDistancia(trigPin, echoPin);
    if (d > 2 && d < 50) { // descarta leituras inválidas
      soma += d;
      count++;
    }
    delay(10);
  }
  if (count == 0) return 100; // nenhum valor válido → distância grande
  return soma / count;
}

// =========================
// LEITURA FILTRADA DO SENSOR
// =========================
bool sensorAtivo(int trigPin, int echoPin) {
  float d = mediaDistancia(trigPin, echoPin);
  return d < 25; // limite de detecção
}

// =========================
// CONEXÃO WI-FI
// =========================
void conectarWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi conectado!");
  Serial.println(WiFi.localIP());
}

// =========================
// CONEXÃO MQTT COM LWT
// =========================
void conectarMQTT() {
  espClient.setInsecure();
  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.print("Conectando MQTT...");
    if (client.connect(clientID.c_str(), mqttUser, mqttPassword, topicStatus, 1, true, "{\"online\":false}")) {
      Serial.println("✅ Conectado ao HiveMQ Cloud!");
      client.publish(topicStatus, "{\"online\":true}", true);
    } else {
      Serial.print("❌ Falhou rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 2s");
      delay(2000);
    }
  }
}

// =========================
// PUBLICAR EVENTO
// =========================
void publicarEvento(String tipo) {
  StaticJsonDocument<200> doc;
  doc["evento"] = tipo;
  doc["timestamp"] = millis();
  doc["pessoas"] = pessoaDentro;

  char buffer[256];
  serializeJson(doc, buffer);
  if (client.publish(topicEvento, buffer, true)) {
    Serial.println("Mensagem enviada: " + String(buffer));
  } else {
    Serial.println("Falha ao enviar mensagem: " + String(buffer));
  }
}

// =========================
// VERIFICAÇÃO DE PASSAGEM
// =========================
void verificarPassagem() {
  static int estado = 0; // 0 = aguardando, 1 = S1 ativo, 2 = S2 ativo, 3 = saída S2->S1
  static unsigned long tempoUltimoEvento = 0;

  bool s1 = sensorAtivo(TRIG1, ECHO1);
  bool s2 = sensorAtivo(TRIG2, ECHO2);

  // evita múltiplos eventos muito próximos
  if (millis() - ultimoEventoDetectado < intervaloMinimoEvento) return;

  switch (estado) {
    case 0: // aguardando
      if (s1 && !s2) estado = 1;      // possível entrada
      else if (s2 && !s1) estado = 3; // possível saída
      break;

    case 1: // s1 detectou primeiro → entrada
      if (s2) {
        pessoaDentro++;
        publicarEvento("entrada");
        Serial.println("➡️ Entrada detectada!");
        ultimoEventoDetectado = millis();
        estado = 2;
      }
      break;

    case 2: // reset depois da entrada
      if (!s1 && !s2) estado = 0;
      break;

    case 3: // s2 detectou primeiro → saída
      if (s1) {
        if (pessoaDentro > 0) pessoaDentro--;
        publicarEvento("saida");
        Serial.println("⬅️ Saída detectada!");
        ultimoEventoDetectado = millis();
        estado = 4;
      }
      break;

    case 4: // reset depois da saída
      if (!s1 && !s2) estado = 0;
      break;
  }

  Serial.print("Pessoas dentro: ");
  Serial.println(pessoaDentro);
}



// =========================
// SETUP
// =========================
void setup() {
  Serial.begin(115200);
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  conectarWiFi();
  conectarMQTT();
}

// =========================
// LOOP PRINCIPAL
// =========================
void loop() {
  if (!client.connected()) {
    conectarMQTT();
  }
  client.loop();

  verificarPassagem();
  delay(intervaloLoop);
}
