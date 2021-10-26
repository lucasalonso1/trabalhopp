
  #include <thermistor.h>
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
  #include <IOXhop_FirebaseESP32.h>
  #include <IOXhop_FirebaseStream.h>
  #include <ArduinoJson.h>
  


THERMISTOR thermistor(34,        // Analog pin
                      10000,          // Nominal resistance at 25 ºC
                      3950,           // thermistor's beta coefficient
                      10000);         // Value of the series resistor
uint16_t  TEMPERATURA = 0;
          
int SET_POINT = 30;

int POWER_PWM = 0;

int pwm_motor = 0;
int temptemperatura, tempvelocidade = 0;

// Replace with your network credentials
const char* ssid = "naofunciona";
const char* password = "senha@123@senha";
#define FIREBASE_HOST "https://trabalhopp-7e58e-default-rtdb.firebaseio.com/"    
#define FIREBASE_AUTH "68MyH7t4SGxMpZztofevKjKsyiyve8K65fG2FYxG"   

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String FuncaoTemperatura() {
  // ler temperatura aqui
  //float temperatura = random(0,110);
  
  if ( TEMPERATURA != temptemperatura || temptemperatura == 0 ){
    temptemperatura = TEMPERATURA;
    Firebase.setString("/live/temperatura",String(TEMPERATURA)); 
  }
  //Firebase.push("/dados/temperatura",String(temperatura));
  return String(temptemperatura);
 
}

String FuncaoVentilador() {
  //float velocidade = random(0,100);
  if ( POWER_PWM != tempvelocidade || tempvelocidade == 0){
    tempvelocidade = POWER_PWM ;
    Firebase.setString("/live/velocidade",String(POWER_PWM));
  }
  //Firebase.push("/dados/velocidade",String(velocidade));
  return String(tempvelocidade);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(2, OUTPUT);//Definimos o pino 2 (LED) como saída.
  //pinMode(4, INPUT);//Definimos o pino 2 (LED) como saída.
  ledcAttachPin(2, 0);//Atribuimos o pino 2 ao canal 0.
  ledcSetup(0, 1000, 8);//Atribuimos ao canal 0 a frequencia de 1000Hz com resolucao de 10bits.

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  //CSS
   server.on("/main.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/main.css", "text/css");
  });
  server.on("/noscript.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/noscript.css", "text/css");
  });
  //HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
   server.on("/historico.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/historico.html");
  });
     server.on("/grafico.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/grafico.html");
  });
  //JS
   server.on("/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquery.min.js");
  });
    server.on("/jquery.scrolly.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquery.scrolly.min.js");
  });
    server.on("/jquery.scrollex.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquery.scrollex.min.js");
  });
    server.on("/browser.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/browser.min.js");
  });
    server.on("/breakpoints.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/breakpoints.min.js");
  });
    server.on("/util.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/util.js");
  });
    server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/main.js");
  });

  
  server.on("/temperatura", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", FuncaoTemperatura().c_str());
  });
  server.on("/velocidade", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", FuncaoVentilador().c_str());
  });
  // Start server
  server.begin();
}
 
void loop(){
  
   TEMPERATURA = (thermistor.read()/10);
   pwm_motor = map((TEMPERATURA),SET_POINT,100,100,255);
   if(pwm_motor <= 0) pwm_motor = 0;
   POWER_PWM = map(pwm_motor, 0, 255, 0, 100);
   ledcWrite(0, pwm_motor);
   
   /* Serial.print("Temperatura: ");
    Serial.print(TEMPERATURA/10.0);
    Serial.println(" *C");
    Serial.println("");
    
    Serial.print("Potencia Cooler: ");
    Serial.print(POWER_PWM);
    Serial.println("%");*/
    delay(1000);
   
  
}
