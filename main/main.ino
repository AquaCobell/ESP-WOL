
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <cstddef>
#include <ArduinoJson.h>


const char* ssid = "POCO F3"; //Replace with your own SSID
const char* password = "1234567890"; //Replace with your 
AsyncWebServer server(80);
WiFiUDP UDP;
WakeOnLan WOL(UDP);


class Device
{
  private:
  const char* name;
  const char* mac;

  public:
  Device(const char* name,const char* mac)
  {
    this->name = name;
    this->mac= mac;
  }

  Device()
  {
    
  }



  void setname(char* name)
  {
    this->name = name;
  }

  void setmac(char* mac)
  {
    this->mac = mac;
  }

  const char* getname()
  {
    return this->name;
  }
  const char* getmac()
  {
    return this->mac;
  }
  
};


Device devicelist[15]; //* -> Pointer, weil Objekt nicht erstellt werden kann da keine Parameter übergeben werden, Array nicht initialisiert ist. Sonst wird Array mit bereits vorhandenen Objekten erstellt.


String ip;
String mac;
String name;


void setup() {
  
  Serial.begin(115200);
  if(!LittleFS.begin())
  {
    Serial.println("LittleFS Fehler");
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP()); 


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(LittleFS, "/main.html", String(), false);
  });

  server.on("/wakeup", HTTP_GET, [](AsyncWebServerRequest *request)
  {
  
    if(request->hasParam("mac"))
    {
      
      
        mac = request->getParam("mac")->value();

        wakeupbyString(mac);
    }
    request->send(LittleFS, "/main.html", String(), false);
  });

  server.on("/newdevice", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    if(request->hasParam("name") && request->hasParam("mac"))
    {
      mac = request->getParam("mac")->value();
      name = request->getParam("name")->value();

      fillArray(name.c_str(), mac.c_str());
    }
    request->send(LittleFS, "/main.html", String(), false);
    
  });

  server.on("/getdevices", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String json = ArrayToJson(devicelist);


    
    request->send(200, "text/json", json);
    
  });
  server.begin();
}

void print(String ip, String mac)
{
  Serial.println("IP:" + ip + "| MAC: "+ mac);
}

void wakeup(char* MACAdress)
{
  WOL.sendMagicPacket(MACAdress);
}

void wakeupbyString(String MACAdress)
{
    
         
  char *macchar;
  MACAdress.toCharArray(macchar, MACAdress.length() + 1);

  //wakeup(macchar);
}

void wakeupbydevice(Device device)
{
  WOL.sendMagicPacket(device.getmac());
}

void fillArray(const char* name,const char* mac)
{
 
  Device dev(name,mac);
  for(int i= 0; i<15; i++)
  {
    
    if(devicelist[i].getmac() == nullptr )
    {
     
      devicelist[i] = dev; 
      return;
    }

  }
}

String ArrayToJson(Device devicelist[]) //Device devicelist[]
{
  String json;
  StaticJsonDocument<96> doc;
  //doc["devicelist"]["device"]["mac"] = "test";

  for(int i= 0; i<15; i++)
  {
    
    if(devicelist[i].getmac() != nullptr)
    {
      doc["devicelist"][devicelist[i].getname()]["mac"] = devicelist[i].getmac();
    }

  }
    
  serializeJson(doc,json);
  return json;
}

void loop() 
{
  //char* mac = "01:23:45:67:89:AB";
  //char* name = "test"; 
  //String mac = "01:23:45:67:89:AB";
 // String name = "test"; 
  //Device dev(name,mac);
  //Serial.println(dev.getname());
  //Serial.println(dev.getmac());
  //fillArray(name.c_str(), mac.c_str());
  Serial.print("device1: ");
  Serial.println(devicelist[0].getmac());
  Serial.print("device2: ");
  Serial.println(devicelist[1].getmac());
  Serial.print("device3: ");
  Serial.println(devicelist[2].getmac());
  Serial.print("device4: ");
  Serial.println(devicelist[3].getmac());
  //Serial.println("JSON _____________________________________________");
  //Serial.println(ArrayToJson(devicelist));
  
}
