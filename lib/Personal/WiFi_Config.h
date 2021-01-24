#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <ESPAsyncDNSServer.h>

#include <SPIFFS_Serial_Monitor.h>
#include <OLED.h>

#define TRIES 3
#define HOSTNAME "monitor.com"
const char* ssid_ap = "Monitoreo Isolse Wi-Fi";
const char* password_ap = "Monitoreo Isolse Wi-Fi";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

bool all_set        = false;
bool WiFi_Incorrect = true;

IPAddress IP;
//extern SPIFFS;

const char* WIFI_SSID  = "WIFI_SSID";
const char* WIFI_PSW   = "WIFI_PSW";
const char* USERNAME   = "USERNAME";
const char* USER_PSW   = "USER_PSW";
const char* FACILITY   = "FACILITY";

String processor(const String& var)
{
  if(var == "WIFI_SSID"){
    return readFile(SPIFFS, "/WIFI_SSID.txt");
  }
  else if(var == "WIFI_PSW"){
    return readFile(SPIFFS, "/WIFI_PSW.txt");
  }
  else if(var == "USERNAME"){
    return readFile(SPIFFS, "/USERNAME.txt");
  }
  else if(var == "USER_PSW"){
    return readFile(SPIFFS, "/USER_PSW.txt");
  }
  else if(var == "FACILITY"){
    return readFile(SPIFFS, "/FACILITY.txt");
  }
  else
  {
    return "";
  }
  //return String();
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Lo sentimos Pagina no Encontrada");
}


bool Wifi_Connection(String SSID, String PSW)
{
  int  counter  =0;
  bool rtn      =false;
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID,PSW);
  OLED_write_init(SSID);
  while ((WiFi.status() != WL_CONNECTED)&&(counter<=TRIES))
  {
    delay(500);
    counter++;
  }
  if(counter<TRIES)
  {
    rtn=true;
    OLED_write_WiFi_OK();
  }
  return rtn;
}

bool WiFi_Status()
{
  bool rtn = false;
  if((all_set)&&(WiFi_Incorrect))
  {
    rtn = true;
  }
  return rtn;
}

void Wifi_AP()
{
  // Start server
  WiFi.mode(WIFI_AP);
  //WiFi.softAPConfig(IP, IP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid_ap, password_ap);
  server.begin();
  OLED_write_WiFi_AP(IP);
}

void Wifi_AP_setup()
{
  WiFi.mode(WIFI_AP);
  IP = WiFi.softAPIP();
  WiFi.softAP(ssid_ap, password_ap);
  
  //#ifdef DEBUG
  Serial.print("Direccion IP: ");
  Serial.println(IP);
  //#endif
  OLED_write_WiFi_AP(IP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    bool val=true;
    
     // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(WIFI_SSID)) {
      inputMessage = request->getParam(WIFI_SSID)->value();
      writeFile(SPIFFS, "/WIFI_SSID.txt", inputMessage.c_str());
    }
    else
    {
      val=false;
    }
    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    if (request->hasParam(WIFI_PSW)) {
      inputMessage = request->getParam(WIFI_PSW)->value();
      writeFile(SPIFFS, "/WIFI_PSW.txt", inputMessage.c_str());
    }
    else
    {
      val=false;
    }
    // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
    if (request->hasParam(USERNAME)) {
      inputMessage = request->getParam(USERNAME)->value();
      writeFile(SPIFFS, "/USERNAME.txt", inputMessage.c_str());
    }
    else
    {
      val=false;
    }
    if (request->hasParam(USER_PSW)) {
      inputMessage = request->getParam(USER_PSW)->value();
      writeFile(SPIFFS, "/USER_PSW.txt", inputMessage.c_str());
    }
    else
    {
      val=false;
    }
    // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
    if (request->hasParam(FACILITY)) {
      inputMessage = request->getParam(FACILITY)->value();
      writeFile(SPIFFS, "/FACILITY.txt", inputMessage.c_str());
    }
    else
    {
      val=false;
    }

    if(val)
    {
      all_set=true;
    }

    #ifdef DEBUG    
    Serial.println(inputMessage);
    #endif

    request->send(SPIFFS, "/index.html", String(), false, processor);
  }); 
  server.onNotFound(notFound);
  server.begin();
}

bool WiFi_Configuration()
{
  bool rtn = false;
  String Data_SSID = readFile(SPIFFS, "/WIFI_SSID.txt");
  String Data_PSW  = readFile(SPIFFS, "/WIFI_PSW.txt");

  if((Data_SSID.length()>1)&&(Data_PSW.length()>1))
  {
    rtn = Wifi_Connection(Data_SSID,Data_PSW);
  }
  if(!rtn)
  {
    OLED_write_WiFi_Fail();
    Wifi_AP_setup();
  }
  return rtn;
}

#endif
