#include <WiFi.h>
#include <HTTPClient.h>
//
const char* ssid = "SEU_SSID";
const char* password = "SENHA_WIFI";

//URL completa utilizando o metodo GET para enviar dados
String getUpdate = "http://aditamento.pmb.eb.mil.br/sensor/update.php";
String setSensor = "https://sisgep-pmb.eb.mil.br/rest/monitorAgua/monitorAgua";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 60000;


float flowRate;             // Variável para armazenar o valor em L/min
int   pulseCount;           // Variável para a quantidade de pulsos
 
float liters =      0;      // Variável para Quantidade de agua
float mililiters =  0;      // Variavel para Conversão
int sensorPin = 5;              // Atribuição do pino de entrada do sinal do sensor


int idSensor = 1;


void setup() {
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  int i = 0;
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(i++ > 10){
      break;
      }
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  pinMode(sensorPin, INPUT);            // Pino que receberá os pulsos do sensor
  attachInterrupt(sensorPin, pulse, RISING);    // Configura o pino 2(Interrupção 0)
  
}

void loop() {
  pulseCount =  0;
  sei();             //Habilita interrupção
  delay (1000);      //Aguarda 1 segundo
  cli();             //Desabilita interrupção
  flowRate = pulseCount / 4.8;    // Converte para L/min (5.5 sensor 3/4, 4.8 sensor 1pol)
  mililiters = flowRate / 60;     // Calculo de mililitros
  liters = liters + mililiters;   // Calculo de quantos litros passou pelo sensor

  Serial.print(" Pulsos: ");
  Serial.print(flowRate);
  Serial.print(" Mililitros: ");
  Serial.print(mililiters);
  Serial.print(" Litros: ");
  Serial.println(liters);
 



  
  //Send an HTTP POST request every X minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      
      Serial.println("");
      Serial.println("TimeDelay Atual: " + timerDelay);
     


      if(liters>0){
        String url = setSensor + "?codigo=" + idSensor + "&quantidade=" + liters;
        Serial.println("URL: "+url);
        String attSensor = wget(url);
        Serial.println("Resposta do Servidor: "+attSensor);
        if(attSensor == "OK"){
            liters = 0;
          }
      }
      
      
      
    }
    else {
      Serial.println("Sem Wifi: ");
      Serial.println(ssid);
    }
    lastTime = millis();
  }
}

String wget (String url){
  HTTPClient http;

      String serverPath = url;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        String payload = http.getString();
        return payload;

      }
      else {
          return "erro";
      }
      http.end();
  }

void pulse ()
{
  pulseCount++; //Incrementa a variável de pulsos
}
