#ifndef COOPSystem_h
#define COOPSystem_h

#include <OLED.h>
#include <WiFi_Config.h>
#include <Firebase_Comm.h>
#include <Serial_Msg.h>
#include <Simplex_Protocol.h>

FirebaseJsonArray  data;
FirebaseJson       data_j;
String jsonStr;

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
    OLED_setup();

    SPIFFS_Start();
    OLED_write_start();

    //Conexion WiFi con datos
    if (WiFi_Configuration())
    {
      OLED_write_done();
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
    Serial.println("Firebase Set Up");
    #endif
    rtn = Firebase_Set_Up();
    #ifdef TEST
    Serial.println("Firebase Set Up Correctly");
    #endif
    if (rtn)
    {
    #ifdef TEST
    Serial.println("Firebase 1st Push Up");
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

    if(loops>0)
    {
      for(int i=0;i<loops;i++)
      {
        cycles++;
        strcpy(msg_line,get_Serial_Msg());
        strcpy(aux,USER_FLAG);
        if(0!=memcmp (msg_line, aux, sizeof(msg_line)))
        {
          JSON_Conversion2(msg_line,0,&data_j);
          data.add(data_j);
          data_j.clear();
        }

        //arreglas la condicion debe ser alcanar 3 y cargar en firebase
        if((cycles>=3)||(loops-1==i))
        {
          #ifdef TEST
            Serial.println(" ********************************** ");
            data.toString(jsonStr, true);
            Serial.println(jsonStr);
            Serial.println(" ********************************** ");
          #endif

          rtn = dataupload2(&data);
          
          #ifdef TEST
            Serial.println("Borrado de memoria");
          #endif
          
          data.clear();
        }
      }
      //clean_JSON_array();
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

    #ifdef TEST
      Serial.println("Borrado de final");
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
    oled_event_t msg_event;

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
    return rtn;
  }

  bool Print_OLED()
  {
    return Print_Download();
  }
};

#endif