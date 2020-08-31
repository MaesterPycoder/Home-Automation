#include <SoftwareSerial.h>
#include <stdlib.h> 
int ledPin = 13;
String apiKey = "U78K3PYRY3EZUFZR";
String ssid="Sss";
String pswd="12345678";
String wifi;

SoftwareSerial espSerial(3, 2); // RX, TX using software serial for esp8266
void setup() {                
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);    

  // enable debug serial
  Serial.begin(115200); 
  // enable software serial
  espSerial.begin(57600);
  
  wifi="AT+CWJAP_DEF=";
  wifi+='"';
  wifi+=ssid;
  wifi+='"';
  wifi+=',';
  wifi+='"';
  wifi+=pswd;
  wifi+='"';
  wifi+="\r\n";
  espSerial.println(wifi);
  delay(2000);
  // reset ESP8266
  espSerial.println("AT+RST\r\n");
  delay(5000);
}

void loop() {
    int i=0;
    char c[100]; // returned data storage
  
   // TCP connection to thingspeak.com api server;
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "api.thingspeak.com";
  cmd += "\",80";
  espSerial.println(cmd);
  if(espSerial.find("ERROR")){
    Serial.println("AT+CIPSTART error");
    espSerial.println("AT+CIPCLOSE\r\n");
    espSerial.println(cmd);
    return;
  }
  else if(espSerial.find("OK"))
  {
    Serial.println("connected to Thing speak API");
  }
  else if(espSerial.find("ALREA"))
  {
    Serial.println("Already connected");
  }
  
  // prepare GET string
  String getStr = "GET /channels/1120116/fields/1.json?results=1";
  getStr += "\r\n";


  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
 
  if(espSerial.find(">")){
    espSerial.print(getStr);
    delay(1000);
    if(espSerial.find("+IPD"))
    {
      Serial.println("+IPD found");
      while(espSerial.available())
      {
        if(espSerial.find("feeds"))
        { Serial.println("found feeds:");
          while(espSerial.available())  
         {
      c[i]=espSerial.read();
      i++;
         }
        }
      }
      if(c[i-13]=='1')
      {
      digitalWrite(ledPin,HIGH);
      }
      
      else if(c[i-13]=='0')
      {
        digitalWrite(ledPin,LOW);
      }
      Serial.println(i);
      Serial.println(c);
      c[i-13]='/0';
      espSerial.flush();
    }
    else
    {
      Serial.println("+IPD not found");
      espSerial.flush();
    }
  }
  else{
    espSerial.println("AT+CIPCLOSE\r\n");
    // alert user
    Serial.println("AT+CIPCLOSE");
    espSerial.flush();
  }
  delay(2000);
}
