#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "NOME_DA_REDE";
const char* password = "SENHA_DO_WIFI";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

const char* topicoRecebe = "controle/dupla1";
const char* topicoEnvia  = "controle/dupla2";

const int ledPin = 2;

WiFiClient espClient;
PubSubClient client(espClient);

void conectarWiFi() {
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  String mensagemRecebida;
  for (unsigned int i = 0; i < length; i++) {
    mensagemRecebida += (char)message[i];
  }

  if (mensagemRecebida.equalsIgnoreCase("LIGAR")) {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED LIGADO");
  } else if (mensagemRecebida.equalsIgnoreCase("DESLIGAR")) {
    digitalWrite(ledPin, LOW);
    Serial.println("LED DESLIGADO");
  }
}

void conectarMQTT() {
  while (!client.connected()) {
    if (client.connect("ESP32_Dupla1")) {
      client.subscribe(topicoRecebe);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  conectarWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  conectarMQTT();
  Serial.println("Digite: LIGAR ou DESLIGAR");
}

void loop() {
  if (!client.connected()) {
    conectarMQTT();
  }

  client.loop();

  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    if (comando.length() > 0) {
      client.publish(topicoEnvia, comando.c_str());
      Serial.print("Comando enviado: ");
      Serial.println(comando);
    }
  }
}
