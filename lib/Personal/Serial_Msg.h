#ifndef SERIAL_MSG_h
#define SERIAL_MSG_h

#include <Arduino.h>
#include <Wire.h>

#define MAXMSGS 21
#define MAXMSGLENGTH 100
#define ERROR_MSG "NO DATA"
#define SIMPLEX_INIT_DELAY 50

bool stringComplete = false; // whether the string is complete
String Matriz[MAXMSGS];
int count_index = 0;
int count_rtn_index = 0;

//TODO PUNTO DE MEJOR DE STRING 

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
  if(count_rtn_index <= count_index)
  {
    count_rtn_index++;
    #ifdef TEST
      Serial.print(F("Matri Index: "));
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
  if(count_index <= MAXMSGS)
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
}

void Serial_Configuration()
{
    Matriz_Setup();
}

void Simplex_Init()
{
  Serial.print(F("exit\n"));
  delay(SIMPLEX_INIT_DELAY);
  Serial.print(F("LOGIN\n"));
  delay(SIMPLEX_INIT_DELAY);
  Serial.print(F("333\n"));
}

void Simplex_Fail_List()
{
  Serial.print(F("L T\n"));
}

void Simplex_Alarm_List()
{
  Serial.print(F("L F\n"));
}

bool get_flag()
{
  return stringComplete;
}

/*
void Print_Msg_Data()
{
  #ifdef TEST
  for (int i=0;i<=count_index;i++)
  {
    Serial.print(F("Data "));
    Serial.print(i);
    Serial.println(F(" :"));
    Serial.println(Matriz[i]);
  }
  #endif
  //count_index=0;

}
*/

int get_count_index()
{
  return count_index;
}

#endif