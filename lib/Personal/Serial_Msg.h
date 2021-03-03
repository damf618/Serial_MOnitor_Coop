#ifndef SERIAL_MSG_h
#define SERIAL_MSG_h

#include <Arduino.h>
#include <Wire.h>
#include <OLED.h>

#define MAXMSGS 60
#define MAXMSGLENGTH 100
#define TIME_WAIT 2500
#define ERROR_MSG "NO DATA"

bool stringComplete = false; // whether the string is complete
String Matriz[MAXMSGS];
int count_index = 0;
int count_rtn_index = 0;


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
  count_rtn_index=0;
}

void clean_JSON_array()
{
  stringComplete = false;
  Init_Matriz();
}

const char* get_Serial_Msg()
{
  if(count_rtn_index<=count_index)
  {
    count_rtn_index++;
    #ifdef TEST
      Serial.print("Matri Index: ");
      Serial.println(count_rtn_index-1);
    #endif
    return Matriz[count_rtn_index-1].c_str();
  }
  else
  {
    return ERROR_MSG;
  }
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

void Serial_Configuration()
{
    Matriz_Setup();
}

void Simplex_Init()
{
  Serial.print("exit\n");
  delay(50);
  Serial.print("LOGIN\n");
  delay(50);
  Serial.print("333\n");
}

//TODO Make a previous validation to check if there are any fails
void Simplex_Fail_List()
{
  Serial.print("L T\n");
}

void Simplex_Alarm_List()
{
  Serial.print("L F\n");
}

bool get_flag()
{
  return stringComplete;
}

void Print_Msg_Data(bool mode)
{
  #ifdef TEST
  for (int i=0;i<=count_index;i++)
  {
    Serial.print("Data ");
    if(mode)
    {
      Serial.print("Trouble ");
    }
    else
    {
      Serial.print("Fire ");
    }
    Serial.print(i);
    Serial.println(" :");
    Serial.println(Matriz[i]);
  }
  #endif
  //count_index=0;

}

int get_count_index()
{
  return count_index;
}


#endif