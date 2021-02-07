#ifndef SERIAL_MSG_h
#define SERIAL_MSG_h

#include <Arduino.h>
#include <Wire.h>
#include <OLED.h>

char data_char;
bool stringComplete = false;  // whether the string is complete
String Matriz[30];
int count_index=0;
bool wtf=true;

void Init_Matriz()
{
  count_index=0;
  for(int i=0;i<=30-1;i++)
  {
    Matriz[i].reserve(100);
    Matriz[i]="";
  }
}

void serialEvent() {
  String inputString;
  unsigned long timecheck=millis();
  inputString.reserve(100);
  while (millis()-timecheck<=3000) {
    // get the new byte:
    if(Serial.available()) {
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if ((inChar == '\n')||(inputString.length()>=100)) {
      Matriz[count_index]=inputString;
      count_index++;
      stringComplete = true;
      inputString="";
      #ifdef TEST
      Serial.print(inputString);
      #endif
    }
    }
  }
}

void OLED_Init()
{
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.display();
}

void OLED_Print( )
{
  for(int i=0;i<=count_index;i++)
  {
  OLED_Init();
  display.println(Matriz[i]);
  display.display();
  delay(4000);
  }
  Init_Matriz();
}

void Simplex_Init()
{
  Serial.println("");
  delay(1000);
  Serial.println("LOGIN");
  delay(1000);
  Serial.println("444");
  //Needed Delay to receive the answer from the central
  while(Serial.available())
  {
    // get the new byte:
    Serial.read();
  }
  Init_Matriz();
}

//TODO Make a previous validation to check if there are any fails
bool Simplex_Fail_List()
{
    bool rtn =true;
    Serial.println("L T");
    serialEvent();
    return rtn;
}

bool Simplex_Alarm_List()
{
    bool rtn =true;
    Serial.println("L F");
    serialEvent();
    return rtn;
}

bool get_flag()
{
    return stringComplete;    
}

void Print_Msg_Data()
{
    if(get_flag())
    {
        OLED_Init();
        display.print("Numero de Mensajes: ");
        display.println(count_index);
        display.display();
        delay(2000);
        OLED_Print();
        #ifdef TEST
        Serial.println(data_char);
        #endif
        stringComplete = false;
    }
}

#endif