#ifndef COOPSystem_h
#define COOPSystem_h

#include <WiFi_Config.h>
#include <Firebase_Comm.h>
#include <Serial_Msg.h>
#include <Simplex_Protocol.h>

FirebaseJsonArray  data;
FirebaseJson       data_j;
String jsonStr;
int N_uploads = 0;
char messageled[50]  = "";
bool refresh_oled = false;

class Coop_System
{
public:

  void FACP_Setup()
  {
    Simplex_Init(); 
  }

  void Serial_Config()
  {
    Serial_Configuration(); 
    OLED_Serial();
  }

  void Serial_Msg_Upload(String Msg)
  {
    Msg_Upload(Msg);
  }

  bool Start()
  {
    bool rtn = true;    
    jsonStr.reserve(500);

    //**************
    OLED_Start();

    SPIFFS_Start();

    //**************
    OLED_WiFi();
    //Conexion WiFi con datos
    if (WiFi_Configuration())
    {
      //**************
      //OLED_WiFi_State(true);

      #ifdef TEST
        Serial.println(F("*---**---**---**---*"));
        Serial.println(F("WIFI OK!"));
        Serial.println(F("*---**---**---**---*"));
      #endif
    }
    else
    {
      //**************
      OLED_WiFi_State(false);
      rtn = false;
    }
    return rtn;
  }

  void CAI_OLED()
  {
    OLED_CAI_OK();
  }

  void Firebase_OLED()
  {
    OLED_Firebase_OK();
  }

  void get_Fails()
  {
    Simplex_Fail_List();
  }

  void get_Alarms()
  {
    Simplex_Alarm_List();
  }

  bool Wi_Fi_Status()
  {
    bool rtn;
    rtn = WiFi_Status();
    return rtn;
  }

  bool Wi_Fi_Connection()
  {
    bool rtn = false;
    rtn = WiFi_Configuration();
    return rtn;
  }

  void WiFi_Attention_OLED()
  {
    WiFi_Attention();
  }

  bool Firebase_Set_up()
  {
    OLED_Firebase();
    bool rtn = false;

    #ifdef TEST
    Serial.println(F("Firebase Set Up"));
    #endif

    rtn = Firebase_Set_Up();

    #ifdef TEST
    Serial.println(F("Firebase Set Up Correctly"));
    #endif

    if (rtn)
    {
    
    #ifdef TEST
    Serial.println(F("Firebase 1st Push Up"));
    #endif

      rtn = Firebase_First_Push();
    }
    return rtn;
  }

  bool WiFi_Val()
  {
    bool rtn= false;
    rtn= Firebase_Check_Conn();
    return rtn;
  }

  bool Firebase_upload()
  {
    bool rtn = false;
    char msg_line[100];
    char aux[100];
    int loops= get_count_index();
    int cycles = 0;
    char cycle_count = 0;
    int cycles_needed = 0;
    int resta = 0;

    if(loops>=0)
    {
      cycles_needed = (int)loops/3;
      if(0!=loops%3)
      {
        cycles_needed++;
      }
      resta = N_uploads - cycles_needed;
      #ifdef TEST
        Serial.print(F("The actual number of Events is: "));
        Serial.println(N_uploads);
        Serial.print(F("There will be this number of cycles: "));
        Serial.println(cycles_needed);
      #endif
      if(cycles_needed < N_uploads)
      {
        #ifdef TEST
          Serial.print(F("We need to delete this number of blocks: "));
          Serial.println(resta);
        #endif
        for (int i=0;i<resta;i++)
        {
          #ifdef TEST
            Serial.print(F("paths to be deleted... address + "));
            Serial.println(cycles_needed + i);
          #endif
          rtn = Firebase_Clean_Node(cycles_needed + i);
        }
      }
      else if(0 == cycles_needed)
      {
        rtn = true;
      }

      if(0!=loops)
      {
        for(int i=0;i<loops;i++)
        {
          cycles++;
          strcpy(msg_line,get_Serial_Msg());
          strcpy(aux,USER_FLAG);
          if(0!=memcmp (msg_line, aux, sizeof(msg_line)))
          {
            JSON_Conversion2(msg_line,&data_j);
            data.add(data_j);
            data_j.clear();
          }

          //arreglas la condicion debe ser alcanar 3 y cargar en firebase
          if((cycles>=3)||(loops-1==i))
          {
            #ifdef TEST
              Serial.println(F(" ********************************** "));
              data.toString(jsonStr, true);
              Serial.println(jsonStr);
              Serial.println(F(" ********************************** "));
            #endif
            if(10>cycle_count)
            {
              rtn = dataupload2(&data,cycle_count);
            }
            #ifdef TEST
              else
              {
                Serial.println(F(" Max number of failures reached!"));
              }
            #endif
            cycle_count++;
            #ifdef TEST
              Serial.println(F("Borrado de memoria"));
            #endif
            cycles = 0;
            data.clear();
          }
        }
      }
    }
    else
    {
      if(0!=N_uploads)
      {
        #ifdef TEST
          Serial.println(F("Borrado total de Firebase"));
          rtn = Firebase_Clean();
        #endif
      }
    }
    N_uploads = cycle_count;
    #ifdef TEST
      Serial.println(F("Borrado de final"));
      Serial.print(F("The new number of Events is: "));
      Serial.println(N_uploads);
    #endif

    clean_JSON_array();
    return rtn;
  }

  bool Firebase_enable()
  {
    int rtn;
    rtn = Firebase_Enable();
    return rtn;
  }

  void Print_OLED(bool WiFi_Con, Central_State CAII, char * messagee)
  {
    OLED_Display.Screen_Loop_Refresh( WiFi_Con, CAII, messagee,refresh_oled);
    OLED_Display.Screen_Set();
    if(refresh_oled)
    {
      refresh_oled=!refresh_oled;
    }
  }

  void Trouble_OLED(char* msg)
  {
    refresh_oled=true;
    memcpy(msg,MSG_CLEAN,strlen(MSG_CLEAN)+1);
    strcpy(msg,MSG_FAILS);
  }

  void Fire_OLED(char* msg)
  {
    refresh_oled=true;
    memcpy(msg,MSG_CLEAN,strlen(MSG_CLEAN)+1);
    strcpy(msg,MSG_ALARM);
  }

  void OLED_Firebase_Error()
  {
    OLED_Firebase_BAD();
  }
};

#endif