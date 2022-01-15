// INCLUSÃO DE BIBLIOTECAS
#include <WiFi.h>
#include <TinyGPS.h>
#include<SoftwareSerial.h>
#include <MPU6050_light.h>
#include <Adafruit_BMP085.h>
#include "Wire.h"


// INSTANCIANDO OBJETOS
SoftwareSerial SerialGPS(3, 1);
TinyGPS GPS;

MPU6050 mpu(Wire);

float lati, longi, result;

bool recebido = false;

String pacote;

Adafruit_BMP085 bmp;

const char* ssid = "RpiAP";
const char* password = "87654321";
 
const uint16_t port = 8090;
const char * host = "192.168.15.6";

 WiFiClient client;

void setup(){
  
  pinMode(5, OUTPUT); //buzzer
  
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Wire.begin();
 
  //verifica wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
  //verifica conexao
  while (!client.connect(host, port)) {
 
      Serial.println("Conexão ao host falhou");
 
      delay(1000);
  }
  //verifica bmp
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP180 sensor, check wiring!");
  while (1) {}
  }
  //verifica mpu
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } 

  
  digitalWrite(5, HIGH);
  delay(300);
  digitalWrite(5, LOW);
  delay(200);
  digitalWrite(5, HIGH);
  delay(300);
  digitalWrite(5, LOW);
  delay(2000);
  

  Serial.println(F("Calculating offsets, do not move MPU6050"));  
  delay(1000);
  mpu.calcOffsets(true,true); // gyro and accelero
  Serial.println("Done!\n");
  

  digitalWrite(5, HIGH);
  delay(300);
  digitalWrite(5, LOW);
  delay(200);
  digitalWrite(5, HIGH);
  delay(300);
  digitalWrite(5, LOW);
  delay(200);
  digitalWrite(5, HIGH);
  delay(300);
  digitalWrite(5, LOW);
  
}

void loop() { 

  char lido = char(client.read());

  if (lido == '1'){

    unsigned long tempo = millis();
    
    while ((millis() - tempo) < 6000){
    
      recebido = false;
    
      mpu.update();
      
      float ac_x = mpu.getAccX()*9.81;
      float ac_y = mpu.getAccY()*9.81;
      float ac_z = mpu.getAccZ()*9.81;
    
      float gy_x = mpu.getAngleX();
      float gy_y = mpu.getAngleY();
      float gy_z = mpu.getAngleZ();
    
      float pressao = bmp.readPressure();
    
      if (!recebido) { 
        while (SerialGPS.available()) {
          char cIn = SerialGPS.read();
          recebido = GPS.encode(cIn);
        }
      }
      GPS.f_get_position(&lati, &longi);
    
      String dados = String(lati) + "|" + String(longi) + "|" + 
      String(ac_x) + "|" + String(ac_y) + "|" + String(ac_z) + "|" + 
      String(gy_x) + "|" + String(gy_y) + "|" + String(gy_z) + "|" + 
      pressao;
      
      pacote = pacote + "/" + dados;
      
      Serial.println(F("-=-=-=-=-=-=-=-=-"));
      Serial.println(lati, 6);
      Serial.println(longi, 6);
      Serial.println(F("-=-=-=-=-=-=-=-=-"));
      Serial.println(ac_x, 2);
      Serial.println(ac_y, 2);
      Serial.println(ac_z, 2);
      Serial.println(F("-=-=-=-=-=-=-=-=-"));
      Serial.println(gy_x, 2);
      Serial.println(gy_y, 2);
      Serial.println(gy_z, 2);
      Serial.println(F("-=-=-=-=-=-=-=-=-"));
      Serial.println(pressao);

      /*unsigned long timeout = millis();

      while (client.available() == 0) {
       if (millis() - timeout > 5000) {
           Serial.println(">>> Client Timeout !");
           client.stop();
           return;
        }
      }*/
    }
    client.print(pacote);
    digitalWrite(5, HIGH);
    client.stop();
    
  }  
}
  

/*
long double toRadians(const long double degree) // função para converter graus em radianos
{ 
    long double one_deg = (M_PI) / 180;  
    return (one_deg * degree); 
} 
  
long double distance(long double lat1, long double long1, // função para calcular a distância entre dois pontos usando suas latitudes e longitudes  
                     long double lat2, long double long2) 
{ 
    lat1 = toRadians(lat1); // converte todas as coordenadas para radianos
    long1 = toRadians(long1); 
    lat2 = toRadians(lat2); 
    long2 = toRadians(long2); 
      
    // fórmula de Haversine 
    long double dlong = long2 - long1; // calcula a diferença das latitudes longitudes
    long double dlat = lat2 - lat1; 
  
    long double d = pow(sin(dlat / 2), 2) +  cos(lat1) * cos(lat2) * pow(sin(dlong / 2), 2); // aplica a fórmula 
  
    d = 2 * asin(sqrt(d)); 
    
    return d; // retorna a distância
}*/
