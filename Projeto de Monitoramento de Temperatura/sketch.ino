#include "DHTesp.h"
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;
PubSubClient mqttClient(client);

const char* ssid = "Wokwi-GUEST";
const char* senha = "";

unsigned int port = 1883;
const char* server  = "broker.hivemq.com";  
char* usuario = "SENAI-DES-MATHEUS-12125";

const int PINO_DHT = 15;

String temperatura = "0";
String umidade = "0";
String newTemperatura = "";
String newUmidade = "";

int contador = 0;

DHTesp sensorDHT;
LiquidCrystal_I2C tela(0x27, 16, 2);


void setup() {
  Serial.begin(115200);
  //Config LCD
  tela.init();
  tela.backlight();
  //Config Sensor
  sensorDHT.setup(PINO_DHT, DHTesp::DHT22);
  //Config WiFi
  WiFi.begin(ssid, senha);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("Conectando à rede.");
  mqttClient.setServer(server, port);
  Serial.println("Conectando ao mqtt.");
  while (!mqttClient.connected()) {
    delay(500);
    mqttClient.connect(usuario);
    Serial.println(".");
  }
  Serial.println("Conectado ao MQTT!");

}

void loop() {
  contador++;

  TempAndHumidity dados = sensorDHT.getTempAndHumidity();
  newTemperatura = String(dados.temperature, 2);
  newUmidade = String(dados.humidity, 1);
  
  // So atualiza a tela quando precisa.
  if (newTemperatura != temperatura || newUmidade != umidade) {
    temperatura = newTemperatura;
    umidade = newUmidade;
    resetarTela();
    enviarDadosTela();
  }

  if (contador == 15) {
    enviaDadosMqtt(temperatura, umidade);
    contador = 0;
  } 

  Serial.println("Temperatura: " + temperatura + "C° | Umidade: " + umidade + "%");
  delay(1000); 
}

void enviarDadosTela() { 
  tela.setCursor(0,0);
  tela.print("Temp: "+ temperatura + "C");
  tela.setCursor(0,1);
  tela.print("Umidade: " + umidade + "%");
}

void resetarTela() { 
  tela.setCursor(0,0);
  tela.print("                ");
  tela.setCursor(0,1);
  tela.print("                ");
}

void enviaDadosMqtt(String temperatura, String umidade) {
  Serial.println("Enviando dados de Temperatura e Umidade");
  char mensagem[128]; // buffer para armazenar a concatenação
  //Concatenação
  sprintf(mensagem, "{\"temperatura\": \"\%s\"\, \"umidade\": \"\%s\"\}", temperatura, umidade);
  mqttClient.publish("senai/matheus", mensagem);
  Serial.println("Mensagem Enviada ");
  Serial.print(mensagem);
}
