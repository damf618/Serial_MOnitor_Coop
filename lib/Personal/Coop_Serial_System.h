#ifndef COOPSystem_h
#define COOPSystem_h

#include <OLED.h>
#include <WiFi_Config.h>
#include <Firebase_Comm.h>
#include <Serial_Msg.h>

class Coop_System
{
public:
  /*
  Coop_System()
  {
    Msg_Event_counter  = 0;
    Msg_Event_attended = 0;
  }*/

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

  void Print_Serial_Msg()
  {
    Print_Msg_Data();

  }

  bool Start()
  {
    bool rtn = true;
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

  void OLED_Events()
  {
    Print_Msg_Data();
  }

  //TODO Make a previous validation to check if there are any fails
  bool get_Fails()
  {
    bool rtn =false;
    rtn = Simplex_Fail_List();
    return rtn;
  }

  //TODO Make a previous validation to check if there are any fails
  bool get_Alarms()
  {
    bool rtn =false;
    rtn = Simplex_Alarm_List();
    return rtn;
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

  bool Firebase_Set_System_up()
  {
    bool rtn = false;
    #ifdef TEST
    Serial.println("Firebase FIX");
    #endif
    rtn = Firebase_Fix();
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
    rtn = dataupload();
    return rtn;
  }

  bool Firebase_enable()
  {
    int rtn = 2;
    bool rtn_b = false;
    rtn = Firebase_Enable();
    oled_event_t msg_event;

    if (0 == rtn)
    {
      msg_event.oled_msg = WEB_CONFIG;
      Print_Upload(msg_event);
      //OLED_write_Web_Config();
      //Configurar o contactarse con servicio tecnico.
    }
    else
    {
      msg_event.oled_msg = F_ENABLED;
      Print_Upload(msg_event);
      //OLED_write_Firebase_Enabled();
      rtn_b = true;
    }
    return rtn_b;
  }

  bool Print_OLED()
  {
    return Print_Download();
  }
};

#endif
