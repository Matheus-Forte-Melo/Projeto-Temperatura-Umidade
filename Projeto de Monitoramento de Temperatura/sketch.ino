#include "DHTesp.h"
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ThingSpeak.h>


WiFiClient client;
const char* ssid = "Wokwi-GUEST";
const char* senha = "";

unsigned long CANAL = 2253669;
const char* APIKEY = "P5GTARB8HSXNKQYR";  

//HTTPClient http; // Instanciamento

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
  ThingSpeak.begin(client);
  Serial.println("Conectado ao ThingSpeak");
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
    
    enviaDadosThingSpeak(temperatura, umidade);
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

void resetarTela() { // Evita um bug estranho que tava acontecendo
  tela.setCursor(0,0);
  tela.print("                ");
  tela.setCursor(0,1);
  tela.print("                ");
}

void enviaDadosThingSpeak(String temperatura, String umidade) {
  ThingSpeak.setField(1, temperatura);
  ThingSpeak.setField(2, umidade);
  int x = ThingSpeak.writeFields(CANAL, APIKEY);

  if (x == 200) { // && y == 200 
    Serial.println("Dado de temperatura e umidade enviados!");
  } else {
    Serial.println("ERRO HTTP! Alguns dados podem não ter sido enviados " + String(x)); //
  }  
}
