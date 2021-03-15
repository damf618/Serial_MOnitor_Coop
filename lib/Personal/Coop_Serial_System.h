#ifndef COOPSystem_h
#define COOPSystem_h

//#include <OLED.h>
#include <WiFi_Config.h>
#include <Firebase_Comm.h>
#include <Serial_Msg.h>
#include <Simplex_Protocol.h>

FirebaseJsonArray  data;
FirebaseJson       data_j;
String jsonStr;
int N_uploads= 0;

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
  }

  void Serial_Msg_Upload(String Msg)
  {
    Msg_Upload(Msg);
  }

  void Trouble_Protocol()
  {
    bool mode= true;
    //Trouble_Msg_Data();
    Print_Msg_Data(mode);
  }

  void Fire_Protocol()
  {
    //Fire_Msg_Data();
    bool mode= false;
    Print_Msg_Data(mode);
  }

  bool Start()
  {
    bool rtn = true;    
    jsonStr.reserve(500);

    //**************
    //OLED_setup();
    OLED_Start();

    SPIFFS_Start();

    //**************
    //OLED_write_start();

    //Conexion WiFi con datos
    if (WiFi_Configuration())
    {
      //**************
      //OLED_write_done();
      #ifdef TEST
        Serial.println("*---**---**---**---*");
        Serial.println("WIFI OK!");
        Serial.println("*---**---**---**---*");
      #endif
    }
    else
    {
      rtn = false;
    }

    return rtn;
  }

  //TODO Make a previous validation to check if there are any fails
  void get_Fails()
  {
    Simplex_Fail_List();
  }

  //TODO Make a previous validation to check if there are any fails
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

  bool Firebase_upload(bool mode)
  {
    bool rtn = false;
    char msg_line[100];
    char aux[100];
    int loops= get_count_index();
    int cycles = 0;
    char cycle_count = 0;
    int cycles_needed = 0;
    int resta = 0;

    if(loops>0)
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
            Serial.print("paths to be deleted... addres + ");
            Serial.println(cycles_needed + i);
          #endif
          Firebase_Clean_Node(cycles_needed + i);
        }
      }

      for(int i=0;i<loops;i++)
      {
        cycles++;
        strcpy(msg_line,get_Serial_Msg());
        strcpy(aux,USER_FLAG);
        if(0!=memcmp (msg_line, aux, sizeof(msg_line)))
        {
          JSON_Conversion2(msg_line,mode,&data_j);
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
      //clean_JSON_array();
    }
    else
    {
      if(0!=N_uploads)
      {
        #ifdef TEST
          Serial.println(F("Borrado total de Firebase"));
          Firebase_Clean();
        #endif
      }
    }

    /*

    if(loops>0)
    {
      #ifdef TEST
        Serial.println(" ********************************** ");
        data.toString(jsonStr, true);
        Serial.println(jsonStr);
        Serial.println(" ********************************** ");
      #endif
      rtn = dataupload2(&data);
    }

    */
    N_uploads = cycle_count;
    #ifdef TEST
      Serial.println(F("Borrado de final"));
      Serial.print(F("The new number of Events is: "));
      Serial.println(N_uploads);
    #endif

    //data.clear();
    clean_JSON_array();

    if(rtn)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  bool Firebase_enable()
  {
    int rtn;
    rtn = Firebase_Enable();
    
    /*oled_event_t msg_event;

    if (0 == rtn)
    {
      msg_event.oled_msg = WEB_CONFIG;
      Print_Upload(msg_event);
    }
    else
    {
      msg_event.oled_msg = F_ENABLED;
      Print_Upload(msg_event);
    }
    */
    return rtn;
  }

  bool Print_OLED()
  {
    //return Print_Download();
    
    testdrawbitmap();    // Draw a small bitmap image
    OLED_Display.Screen_Set();

    return true;
  }
};

#endif