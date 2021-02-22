#ifndef SERIAL_MSG_h
#define SERIAL_MSG_h

#include <Arduino.h>
#include <Wire.h>
#include <OLED.h>
#include <Simplex_Protocol.h>

//Firebase JSON
#include <FirebaseESP8266.h>

#define MAXMSGS 60
#define MAXMSGLENGTH 100
#define TIME_WAIT 2500

char data_char;
bool stringComplete = false; // whether the string is complete
String Matriz[MAXMSGS];
int count_index = 0;
bool wtf = true;

FirebaseJson json1;

void Json_Setup_Fail(String point_name, String name, String type, String status)
{
  json1.add("id", point_name);
  json1.add("name", name);
  json1.add("type", type);
  //json1.add("status", status);
  json1.add("status", "PROBLEMA");
}

void Json_Setup_Alarm(String point_name, String name, String type, String status)
{
  json1.add("id", point_name);
  json1.add("name", name);
  json1.add("type", type);
  //json1.add("status", status);
  json1.add("status", "ALARMA");
}



void Matriz_Setup()
{
  count_index = 0;
  for (int i = 0; i < MAXMSGS; i++)
  {
    Matriz[i].reserve(MAXMSGLENGTH);
    Matriz[i] = "";
  }
}

void Init_Matriz()
{
  for (int i = 0; i < count_index; i++)
  {
    Matriz[i] = "";
  }
  count_index = 0;
}

void Msg_Upload(String inputString)
{
  Matriz[count_index] = inputString;
  count_index++;
  stringComplete = true;
  #ifdef TEST
  Serial.print(F("Message number: "));
  Serial.println((count_index-1));
  Serial.print(F("The message received is: "));
  Serial.println(inputString.c_str());
  #endif
}

void serialEvent()
{
  unsigned long timecheck = millis();
  unsigned long timecheck1 = millis();
  String inputString;
  inputString.reserve(MAXMSGLENGTH);
  /*
  *Cea una version que se de  polling y no de while
  * */
  Serial.println("Serial Event Listener");
  while (timecheck1 - timecheck <= TIME_WAIT)
  {
    timecheck1 = millis();
    //Serial.print(".");
    //delay(1);
    // get the new byte:
    if (Serial.available())
    {
      char inChar = (char)Serial.read();
      // add it to the inputString:
      inputString += inChar;
      // if the incoming character is a newline, set a flag so the main loop can
      // do something about it:
      if (((inChar == '\n') || (inputString.length() >= 100))and(count_index<MAXMSGS))
      {
        /*
        Matriz[count_index] = inputString;
        count_index++;
        stringComplete = true;
        #ifdef TEST
        Serial.print(inputString);
        #endif
        */
        inputString = "";
      }
    }
  }
  Serial.println("Serial Event Listener OUT");
}

void OLED_Init()
{
  display.clearDisplay();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.display();
}

void OLED_Print()
{
  String jsonStr;
  message_parser_t rtn; 
  #ifdef TEST
  Serial.println("Arranca");
  Serial.print("Counter: ");
  Serial.println(count_index);
  #endif
  for (int i = 0; i < count_index; i++)
  {
    #ifdef TEST
    
    Serial.print("Print: ");
    Serial.println(i);
    Serial.print("Message Extracted: ");
    Serial.println(Matriz[i]);
    
    Serial.println(" ****** Segmentation TEST ****** ");
    rtn = Separator_Search((char *)Matriz[i].c_str());
    Serial.printf("Point Name obtained: %s\n", rtn.point_name);
		Serial.printf("Name obtained: %s\n", rtn.name);
		Serial.printf("Type obtained: %s\n", rtn.type);
		Serial.printf("Status obtained: %s\n", rtn.status);
    Serial.println(" ****** ***************** ****** ");

    Serial.println(" ------------------------------- ");

    Json_Setup_Fail(rtn.point_name,rtn.name,rtn.type,rtn.status);
    json1.toString(jsonStr, true);
    Serial.println(jsonStr);

    Json_Setup_Alarm(rtn.point_name,rtn.name,rtn.type,rtn.status);
    json1.toString(jsonStr, true);
    Serial.println(jsonStr);

    Serial.println(" ------------------------------- ");
    #endif
    OLED_Init();
    display.println(Matriz[i]);
    display.display();
    delay(2000);
  }
  #ifdef TEST
  Serial.println("Final");
  #endif
  Init_Matriz();
  #ifdef TEST
  Serial.println("Final OK");
  #endif
}

void Serial_Configuration()
{
    Matriz_Setup();
}

void Simplex_Init()
{
  //unsigned long timing = 0;
  Serial.print("exit\n");
  delay(50);
  Serial.print("LOGIN\n");
  delay(50);
  Serial.print("444\n");
  //Needed Delay to receive the answer from the central
  //timing = millis();
  /*
  while (millis() - timing <= 500)
  {
    if (Serial.available())
    {
      // get the new byte:
      Serial.read();
    }
  }
  */
}

//TODO Make a previous validation to check if there are any fails
bool Simplex_Fail_List()
{
  bool rtn = true;
  Serial.print("L T\n");
  //serialEvent();
  return rtn;
}

bool Simplex_Alarm_List()
{
  bool rtn = true;
  Serial.print("L F\n");
  //serialEvent();
  return rtn;
}

bool get_flag()
{
  return stringComplete;
}

void Print_Msg_Data()
{
  
  if (get_flag())
  {
    /*
    OLED_Init();
    display.print("Mensajes: ");
    display.println(count_index+1);
    display.display();
    delay(1000);
    OLED_Print();
    */
    OLED_Print();
    #ifdef TEST
      Serial.println("Print Finished");
    #endif
    stringComplete = false;
    Init_Matriz();
  }
}

#endif