//Importar librerias
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

//Incluir el archivo config.h
#include "config.h"

//Declarar componentes
int foco = 22;
int estadoFoco = LOW; //Para guardar el estado del Foco
boolean dato;

#define PWM1_Ch    0
#define PWM1_Res   8
#define PWM1_Freq  1000

String pwmValue;

AsyncWebServer server(80);


String getRSSI(){
  return String(WiFi.RSSI());
}
String processor(const String& var){
    Serial.print(var+" : ");
    if (var == "IP"){
      return String(WiFi.localIP().toString().c_str());
    }
    else if (var == "HOSTNAME"){
      return String(WiFi.getHostname());
    }
    else if (var == "STATUS"){
      return String(WiFi.status());
    }
    else if (var == "SSID"){
      return String(WiFi.SSID().c_str());
    }
    else if (var == "PSK"){
      return String(WiFi.psk().c_str());
    }
    else if (var == "BSSID"){
      return String(WiFi.BSSIDstr().c_str());
    }
    else if (var == "RSSI"){
      return String(WiFi.RSSI());
    }
}


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("Error al montar SPIFFS");
    return;
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html",String(), false, processor);
  });
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.css", "text/css");
  });

  server.serveStatic("/", SPIFFS, "/");
  
  // Ruta para poner el GPIO alto
  server.on("/RSSI", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",getRSSI().c_str());
  });

  //Definimos el foco como salida
  pinMode(foco, OUTPUT); //Inicializamos el foco como salida
  digitalWrite(foco, LOW); //Dejamos inicialmente el foco apagado
  
  ledcAttachPin(foco, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);

  server.on("/SLIDER", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("pwmValue");
    Serial.print("PWM:\t");
    Serial.println(pwmValue);
    request->redirect("/");
    ledcWrite(PWM1_Ch, pwmValue.toInt());
  });

  server.on("/FocoEstado0", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.print("Estado foco: apagado\t");
    ledcWrite(PWM1_Ch, 0);
  });
  server.on("/FocoEstado1", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Estado foco: encendido\t");
    ledcWrite(PWM1_Ch, 1023);
  });
 
  // Start server
  server.begin();
}


void loop(){
  dato=1;
}
