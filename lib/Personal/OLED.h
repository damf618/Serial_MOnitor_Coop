#ifndef OLED_h
#define OLED_h

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define ANIMATION_TIMES 5
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define WEBPAGE "www.monitorisolse.com"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool msg_flag=false;

void OLED_write_start()
{
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);
  display.println("Sistema en");
  display.println("Arranque");
  display.display();
  delay(5000);  
}

void OLED_write_done()
{
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(20,0);
  display.println("Sistema");
  display.setTextSize(2);
  display.setCursor(40,16);
  display.println("OK");
  //display.println("Funcionando"); hasta la d
  display.display();
  delay(2000); 
}

bool OLED_setup()
{
  bool rtn = true;
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x32
    #ifdef DEBUG
    Serial.println(F("Falla en conexion con Display OLED "));
    #endif
    rtn = false;
  }
  return rtn;
}

void OLED_write_init(String SSID,String SSID1)
{
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);
  display.println("Apagando la red WiFi:\n");
  display.println(SSID1);  
  display.display();
  delay(5000);
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);
  display.println("Conectando al WiFi\n");
  display.setTextSize(1);
  display.println(SSID);  
  display.display();
  delay(5000);  
}

void OLED_write_WiFi_OK()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,10);
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.print("WiFi:");
  display.setCursor(70,7);
  display.setTextSize(3);             // Normal 1:1 pixel scale
  display.println("OK!");
  display.display();
  delay(2500);  
}


void OLED_write_Web_Config()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.println("Por favor continuar\ncon la configuracion\nen:");
  display.print(WEBPAGE);
  display.display();
  delay(500);  
}

void OLED_write_Firebase_Enabled()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(10,0);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.println("Vinculo con base");
  display.setCursor(10,20);
  display.println("de datos Exitoso");
  display.display();
  delay(500);  
}

void OLED_write_Firebase_Start()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.println("Iniciando conexion\ncon el servidor");
  display.display();
}

void OLED_write_Firebase_Think()
{
  display.print(".");
  display.display();
  delay(350);
}

void OLED_write_Firebase_Done()
{
  display.print("\nConexion establecida");
  display.display();
  delay(2000);
}

void OLED_write_WiFi_AP(const char* IP_Address)
{
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println("Entra en");
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.println("");
  display.println(IP_Address);
  display.display();
  delay(7500);  
}

void OLED_write_WiFi_Network(String SSID)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.println("Por Favor");
  display.println("conectarse a:\n");
  display.println(SSID);
  display.display();
  delay(7500);
}

void OLED_Write_WiFi_Attention(String SSID,const char* IP_Address)
{
  if(!msg_flag)
  {
    OLED_write_WiFi_Network(SSID);
  }
  else
  {
    OLED_write_WiFi_AP(IP_Address);
  }
  msg_flag = !msg_flag;
}

void OLED_write_WiFi_Fail(String SSID)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,10);
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.print("WiFi:");
  display.setCursor(70,7);
  display.setTextSize(3);             // Normal 1:1 pixel scale
  display.println("NO!");
  display.display();
  delay(3500);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.println("Restaurando Sistema");
  display.display();
  delay(350);
  for(int i=0;i<=ANIMATION_TIMES;i++)
  {
    display.print(".");
    display.display();
    delay(350);
  }
  display.println("\nProcesando Nuevo\nIntento");
  display.display();
  delay(2500);
  OLED_write_WiFi_Network(SSID);
}



#endif
