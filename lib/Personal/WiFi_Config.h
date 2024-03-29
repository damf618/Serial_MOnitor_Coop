#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <ESPAsyncDNSServer.h>
#include <SPIFFS_Serial_Monitor.h>


#define TRIES             10
#define HOSTNAME          "www.monitorisolse.com"
#define WIFI_INIT_DELAY   1000
#define WIFI_TRY_DELAY    500
const char* ssid_ap =     "MonitoreoIsolseWiFi";
const char* password_ap = "MonitoreoIsolseWiFi";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
AsyncDNSServer dnsServer;

bool all_set        = false;
bool WiFi_Incorrect = true;

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

const char* processor1(const String& var)
{
  if(var == "SSID")
  {
    return ssid_ap;
  }
  else
  {
    return HOSTNAME;
  }
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Lo sentimos Pagina no Encontrada");
}

//TODO PUNTO DE MEJOR DE STRING 
bool Wifi_Connection(String SSID, String PSW)
{
  int  counter  =0;
  bool rtn=false;
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID,PSW);
  delay(WIFI_INIT_DELAY);
  while ((WiFi.status() != WL_CONNECTED)&&(counter<=TRIES))
  {
    delay(WIFI_TRY_DELAY);
    counter++;
  }
  if(counter<=TRIES)
  {
    rtn=true;
  }
  return rtn;
}

//TODO PUNTO DE MEJOR DE STRING 
bool Wifi_Con_Reset()
{
  int  counter =0;
  bool rtn=false;
  char SSID_c[30];
  char PSW_c[30];

  //memcpy(msg,MSG_CLEAN,strlen(MSG_CLEAN)+1);
  memcpy(SSID_c,readFile(SPIFFS, "/WIFI_SSID.txt").c_str(),strlen(readFile(SPIFFS, "/WIFI_SSID.txt").c_str())+1);
  memcpy(PSW_c,readFile(SPIFFS, "/WIFI_PSW.txt").c_str(),strlen(readFile(SPIFFS, "/WIFI_PSW.txt").c_str())+1);

  #ifdef TEST
  Serial.print("WiFi SSID: ");
  Serial.println(SSID_c);
  Serial.print("WiFi PSW: ");
  Serial.println(PSW_c);
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_c,PSW_c);

  delay(WIFI_INIT_DELAY);
  while ((WiFi.status() != WL_CONNECTED)&&(counter<=TRIES))
  {
    delay(WIFI_TRY_DELAY);
    counter++;
  }
  if(counter<=TRIES)
  {
    rtn=true;
  }
  else
  {
    ESP.reset();
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
  WiFi.softAP(ssid_ap, password_ap);
  dnsServer.start(DNS_PORT, HOSTNAME, apIP);
  server.begin();
}

void WiFi_Attention()
{
  #ifdef TEST
    Serial.print("\n Conectarse a: ");
    Serial.println(ssid_ap);
    Serial.print("Y entrar en la pagina web: ");
    Serial.println(HOSTNAME);
  #endif

  OLED_WiFi_AP(ssid_ap,HOSTNAME);
}

//TODO PUNTO DE MEJOR DE STRING  REVISAR   prepareDatabaseRules
void Wifi_AP_setup()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid_ap, password_ap);
  dnsServer.setErrorReplyCode(AsyncDNSReplyCode::ServerFailure);
  dnsServer.start(DNS_PORT, HOSTNAME, apIP); 

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    bool val=true;
    inputMessage.reserve(50);
    
     // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(WIFI_SSID)) {
      inputMessage = request->getParam(WIFI_SSID)->value();
      writeFile(SPIFFS, "/WIFI_SSID.txt", inputMessage.c_str());
    }
    if(2>inputMessage.length())
    {
      val=false;
    }

    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    if (request->hasParam(WIFI_PSW)) {
      inputMessage = request->getParam(WIFI_PSW)->value();
      writeFile(SPIFFS, "/WIFI_PSW.txt", inputMessage.c_str());
    }
    if(2>inputMessage.length())
    {
      val=false;
    }

    // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
    if (request->hasParam(USERNAME)) {
      inputMessage = request->getParam(USERNAME)->value();
      writeFile(SPIFFS, "/USERNAME.txt", inputMessage.c_str());
    }
    if(2>inputMessage.length())
    {
      val=false;
    }

    if (request->hasParam(USER_PSW)) {
      inputMessage = request->getParam(USER_PSW)->value();
      writeFile(SPIFFS, "/USER_PSW.txt", inputMessage.c_str());
    }
    if(2>inputMessage.length())
    {
      val=false;
    }

    // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
    if (request->hasParam(FACILITY)) {
      inputMessage = request->getParam(FACILITY)->value();
      writeFile(SPIFFS, "/FACILITY.txt", inputMessage.c_str());
    }
    if(2>inputMessage.length())
    {
      val=false;
    }

    if(val)
    {
      all_set=true;
    }

    #ifdef TEST    
    Serial.println(inputMessage);
    #endif

    request->send(SPIFFS, "/index_return.html", String(), false, processor1);
  }); 
  server.onNotFound(notFound);
  server.begin();
}

//TODO PUNTO DE MEJOR DE STRING
bool WiFi_Configuration()
{
  bool rtn = false;
  String Data_SSID;
  Data_SSID.reserve(50);
  Data_SSID = readFile(SPIFFS, "/WIFI_SSID.txt");
  String Data_PSW;
  Data_PSW.reserve(50);
  Data_PSW  = readFile(SPIFFS, "/WIFI_PSW.txt");

  if((Data_SSID.length()>1)&&(Data_PSW.length()>1))
  {
    rtn = Wifi_Connection(Data_SSID,Data_PSW);
    #ifdef TEST
      Serial.print(F("Intentamos Conexion WiFi!: "));
      Serial.print(rtn);
    #endif
  }

  #ifdef TEST
    Serial.print(F("Conexion de Wi-Fi: "));
    Serial.print(rtn);
  #endif
  
  if(!rtn)
  {  
    Wifi_AP_setup();
    all_set=false;
  }
  else
  {
    WiFi_Incorrect=false;
    //**************
    OLED_WiFi_State(true);
  }
  return rtn;
}

bool WiFi_Connected()
{
  bool rtn = !WiFi_Incorrect;
  return rtn;
}

#endif
