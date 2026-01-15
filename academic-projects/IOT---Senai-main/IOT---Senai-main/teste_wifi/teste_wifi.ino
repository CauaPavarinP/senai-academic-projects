#include <WiFi.h>
const String SSID = "nome da rede";
const String PSWD = "senha da rede";

void scanLocalNetworks(); 

void setup() {
  Serial.begin(115200);
  scanLocalNetworks();
  Serial.println("Iniciando conexão com rede WiFi")
  WiFi.begin(SSID,PSWD);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".")
    delay(200);
  }
  Serial.println("\nConectado!")
}

void loop() {
  

}

void(){
  
}

void scanLocalNetworks(){
  Serial.println("Iniciando Scan de Redes Wi-Fi");
  int number = WiFi.scanNetworks();
  delay (500);
  if(number == -1){
    Serial.println("ERRO!, Deu bolete");
  }
  else{
    Serial.printf("número de redes encontradas: %d\n", number);
    for(int net = 0; net < number; net++){
      Serial.printf("%d - %s  |  %d db\n", net, WiFi.SSID(net), WiFi.RSSI(net));
    }
    
  }
}