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
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
  delay(5000); 
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

void OLED_write_init(String SSID)
{
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
  delay(5500);  
}

void OLED_write_WiFi_AP(IPAddress IP_Address)
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
}

void OLED_write_WiFi_Fail()
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
  display.println("\nProcesando Nuevo Intento");
  display.display();
  delay(2500);
}

#endif
