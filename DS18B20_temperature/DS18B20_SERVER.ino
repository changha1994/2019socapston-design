#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClient.h>


#define myPeriodic 60 //in sec | Thingspeak pub is 15sec
#define ONE_WIRE_BUS D4  // DS18B20 on arduino pin2 corresponds to D4 on physical board

WiFiClient client;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
char INSERT_DATA[] = "INSERT INTO mydb.sensors (value) VALUES (%f)";
char query[128];
float prevTemp = 0;

const char* server = "api.thingspeak.com";
String apiKey ="TRU52JXKXOYVG47B";
const char* MY_SSID = "j"; 
const char* MY_PWD = "12342345";
int sent = 0;
void setup() {
  Serial.begin(115200);
  connectWifi();
}

bool flag = false;
void loop() {
  float temp;
  //char buffer[10];
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0);
  Serial.print(String(sent)+" Temperature: ");
  Serial.println(temp);
  
  if (flag)
    sendTeperatureTS(temp);
  else
    sendTeperatureTStoMyServer(temp);
  flag = !flag;
  int count = myPeriodic;
  while(count--)
  delay(1000);
}

void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}//end connect

void sendTeperatureTS(float temp)
{  
  
   if (client.connect(server, 80)) { // use ip or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(temp);
   postStr += "\r\n\r\n";
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}


void sendTeperatureTStoMyServer(float temp)
{  
   if (client.connect("3.18.215.55", 9000)) { // use ip or api.thingspeak.com
   Serial.println("WiFi Client connected (http) ");
   
   String postStr = "value=";
   postStr += String(temp);
   client.print(String("GET /log?") + postStr + 
                " HTTP/1.1\r\n" + 
                "Host: 3.18.215.55\r\n" + 
                "Connection: close\r\n\r\n");
   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}
