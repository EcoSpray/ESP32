#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
 
#define DHTPIN 4
#define DHTTYPE DHT11
#define LDR_PIN 32 // Define o pino do LDR
 
const char* ssid = "iot-senac";
const char* password = "senac@2025";
 
const char* serverName = "http://192.168.10.101/sensores/DadosSensores/inserir";
 
DHT dht(DHTPIN, DHTTYPE);
 
float umidade, temperatura;
int ldrValor;
String nivelLuminosidade;
 
void setup(void){
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
  dht.begin();
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi conectada.");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}
 
void loop(){
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();
 
  ldrValor = analogRead(LDR_PIN); // Leitura do LDR (0 a 4095)
 
  // Interpreta nível de luminosidade de forma qualitativa (opcional)
  if (ldrValor > 3000) {
    nivelLuminosidade = "Alta";
  } else if (ldrValor > 1500) {
    nivelLuminosidade = "Média";
  } else {
    nivelLuminosidade = "Baixa";
  }
 
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha ao ler o DHT11");
    delay(2000);
    return;
  }
 
  Serial.print("Temp: ");
  Serial.print(temperatura);
  Serial.print(" °C | Umid: ");
  Serial.print(umidade);
  Serial.print(" % | LDR: ");
  Serial.print(ldrValor);
  Serial.print(" | Nível: ");
  Serial.println(nivelLuminosidade);
 
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
 
    // Requisição com os dados
    String httpRequestData = "temperatura=" + String(temperatura) +
                             "&umidade=" + String(umidade) +
                             "&luminosidade=" + nivelLuminosidade;
 
    int httpResponseCode = http.POST(httpRequestData);
 
    if (httpResponseCode > 0) {
      String resposta = http.getString();
      Serial.println("Resposta do servidor: " + resposta);
    } else {
      Serial.print("Erro no envio POST. Código: ");
      Serial.println(httpResponseCode);
    }
 
    http.end();
  }
 
  delay(10000);
}