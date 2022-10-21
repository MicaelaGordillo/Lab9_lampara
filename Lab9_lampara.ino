//Importar librerias
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

//Incluir el archivo config.h
#include "config.h"

//Sensor LDR
#define ADC_VREF_mV    3300.0 // 3.3v en millivoltios
#define ADC_RESOLUTION 4096.0
#define LIGHT_SENSOR_PIN       36 // ESP32 pin GIOP36 (ADC0) conectado al LDR---------------------------------------------------

//Declarar componentes
//Foco
int foco = 22; //---------------------------------------------------------------------
//Led
int led = 21; //======================================================================
int estadoFoco = LOW; //Para guardar el estado del Foco
//Sensor LDR
int datoADC;
bool auxLDR = false;
float porcentaje=0.0;
float factor=100.0/ADC_RESOLUTION;

//PWM
#define PWM1_Ch    0
#define PWM1_Res   8
#define PWM1_Freq  1000

String pwmValue;

AsyncWebServer server(80);

//Horario
String hEncendido, mEncendido, hApagado, mApagado, aux;
bool flagHorario = false;

//Sensor PIR
const int PIN_TO_SENSOR = 19; // GIOP19 pin conectado al sensor ============--------------------------------------
int pinStateCurrent   = LOW;  // estado actual
int pinStatePrevious  = LOW;  // estado previo
bool auxPIR = false;
int datoPIR;

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

//Para el sensor LDR
boolean flag_mode=true;

//Para el sensor PIR
boolean flag_mode1=true;

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
  
  ledcAttachPin(led, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);

  //Primera pestaña
  //Encendido y apagado
  server.on("/FocoEstado0", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Estado foco: apagado\t");
    digitalWrite(foco, LOW);
  });
  server.on("/FocoEstado1", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Estado foco: encendido\t");
    digitalWrite(foco, HIGH);
  });

  //Segunda pestaña
  //Intensidad de la luz
  server.on("/SLIDER", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue = request->arg("pwmValue");
    Serial.print("PWM:\t");
    Serial.println(pwmValue);
    request->redirect("/pwm.html");
    ledcWrite(PWM1_Ch, pwmValue.toInt());
  });
  
  //Tercera pestaña
  //Sensor LDR
  server.on("/EstadoSensorLDR0", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Sensor LDR: Desactivado\t");
    ledcWrite(PWM1_Ch, 0);
    auxLDR = false;
  });
  server.on("/EstadoSensorLDR1", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Sensor LDR: Activado\t");
    auxLDR = true;
  });
  server.on("/SensorLDR", HTTP_GET, [](AsyncWebServerRequest *request){
    if(flag_mode){request->send_P(200, "text/plain", String(datoADC).c_str());}
  });

  //Cuarta pestaña
  //Horario
  server.on("/HORARIO", HTTP_GET, [](AsyncWebServerRequest *request){
    hEncendido = request->arg("horaEncendido");
    mEncendido = request->arg("minutoEncendido");
    hApagado = request->arg("horaApagado");
    mApagado = request->arg("minutoApagado");
    aux = request->arg("auxHorario");
    Serial.print("Hora encendido ->");
    Serial.print(hEncendido);
    Serial.print(":");
    Serial.print(mEncendido);
    Serial.print("\t");
    Serial.print("Hora apagado ->");
    Serial.print(hApagado);
    Serial.print(":");
    Serial.println(mApagado);
  });

  // Quinta pestaña
  // Sensor PIR
  pinMode(PIN_TO_SENSOR, INPUT); // Configurando el pin como entrada
  server.on("/EstadoSensorPIR0", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Sensor PIR: Desactivado\t");
    ledcWrite(PWM1_Ch, 0);
    auxPIR = false;
  });
  server.on("/EstadoSensorPIR1", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Sensor PIR: Activado\t");
    auxPIR = true;
  });
  server.on("/SensorPIR", HTTP_GET, [](AsyncWebServerRequest *request){
    if(flag_mode1){request->send_P(200, "text/plain", String(datoPIR).c_str());}
  });
  
  // Start server
  server.begin();
}

void loop(){
  // Sensor LDR - Lectura de datos
  // lectura del dato analogico (valor entre 0 y 4095)
  datoADC = analogRead(LIGHT_SENSOR_PIN);
  porcentaje=factor*datoADC;
  if (auxLDR == true){
    Serial.print("Valor Analogico = ");
    Serial.print(datoADC);   
    Serial.print("  Porcentaje = ");
    Serial.print(porcentaje);  
    if (datoADC < 40) {
      Serial.println("%  => Oscuro");
      ledcWrite(PWM1_Ch, 1023);
    } else if (datoADC < 800) {
      Serial.println("% => Tenue");
      ledcWrite(PWM1_Ch, 765);
    } else if (datoADC < 2000) {
      Serial.println("% => Claro");
      ledcWrite(PWM1_Ch, 510);
    } else if (datoADC < 3200) {
      Serial.println("% => Luminoso");
      ledcWrite(PWM1_Ch, 255);
    } else {
      Serial.println("% => Muy Luminoso");
      ledcWrite(PWM1_Ch, 0);
    } 
  }
  // Sensor PIR - Lectura de datos
  pinStatePrevious = pinStateCurrent;             // salvando el estado anteorior
  pinStateCurrent = digitalRead(PIN_TO_SENSOR);   // leyendo el estado nuevo
  datoPIR = analogRead(PIN_TO_SENSOR);
  if(auxPIR == true){
    Serial.print("RIP: ");
    Serial.println(datoPIR);
    if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {   // Cambio de estado del pin: LOW -> HIGH
      Serial.println("Movimiento detectado!");
      ledcWrite(PWM1_Ch, 1023);
    } else if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {   // Cambio de estadi del pin: HIGH -> LOW
      Serial.println("Sin movimiento!");
      ledcWrite(PWM1_Ch, 0);
    }
  }
  delay(100);
}
