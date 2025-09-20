#include <WiFi.h>
#include <ESP32Servo.h>

const char* ssid     = "-----";
const char* password = "-----";

#define SW 19
#define LED 17

Servo servo1;
WiFiServer server(80);

int swf=HIGH;

int getSw(){
  int a,r=0;
  a=digitalRead(SW);
  if(a==LOW && swf==HIGH){
    r=1;
  }
  swf=a;
  return r;
}

void setup()
{
  servo1.setPeriodHertz(60); // Standard 50hz servo
  servo1.attach(14, 500, 2400); // attaches the servo on pin 14 to the servo object
  pinMode(SW,INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  servo1.write(0);

  Serial.begin(115200);
  pinMode(14, OUTPUT);
  pinMode(17, OUTPUT);

  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
    
  server.begin();
}

void loop(){
  int s;
  static int f=0;
  s=getSw();
  if(s==1){
    if(f==0){
      digitalWrite(14, HIGH);  
      digitalWrite(17, HIGH); 
      delayMicroseconds(1450);
      digitalWrite(14, LOW);
      delay(10);
      f=1;
      delay(5);
    }else{
      digitalWrite(14, HIGH); 
      digitalWrite(17, LOW); 
      delayMicroseconds(2400);
      digitalWrite(14, LOW);
      f=0;
      delay(5);
    }
  }

  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {

          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("Click <a href=\"/H\">here</a> to unlock.<br>");
            client.print("Click <a href=\"/L\">here</a> to lock.<br>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /H")) {
          digitalWrite(14, HIGH);  
          digitalWrite(17, HIGH); 
          delayMicroseconds(1450);
          digitalWrite(14, LOW);
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(14, HIGH); 
          digitalWrite(17, LOW); 
          delayMicroseconds(2400);
          digitalWrite(14, LOW);
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
  delay(2);
}