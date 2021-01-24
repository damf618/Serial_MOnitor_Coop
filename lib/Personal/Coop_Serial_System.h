#ifndef COOPSystem_h
#define COOPSystem_h

#include <Msg_Se_Events.h>
#include <OLED.h>
#include <WiFi_Config.h>



#define OUTPUT_GPIO 5

//TODO Send to .C
//String temp_s;
//Msg_Event_Object temp_event(1,temp_s,1,1);
//Msg_Event_Object** Events_Array = new Msg_Event_Object*[MAX_MSGS_EVENTS];

class Coop_System 
{
  private:
    int   Msg_Event_counter;
    int   Msg_Event_attended;
    void* Msg_Events[MAX_MSGS_EVENTS];
  public:

  Coop_System()
  {
    Msg_Event_counter  = 0;
    Msg_Event_attended = 0;
  }
  
  bool set_Msg_Event()
  {
    bool rtn=false;
    
    if(MAX_MSGS_EVENTS > Msg_Event_counter)
    {
      Msg_Event_counter++;
      #ifdef TEST
        Serial.println("Evento Registrado");
      #endif
      rtn=true;
    }
    #ifdef TEST
    else
    {
      Serial.println("Maximo Numero de Eventos alcanzados"); 
    }
    #endif
    return rtn;
  }  
  
  bool clear_Msg_Event()
  {
    bool rtn=false;
    
    if(Msg_Event_attended < Msg_Event_counter)
    {
      #ifdef TEST
      Serial.println("Evento Obtenido");
      #endif
      rtn=true;
    }
    #ifdef TEST
    else if (0 >= Msg_Event_counter)
    {
      Serial.println("No existen eventos activos");
    }
    else
    {
      Serial.println("Error");
    }
    #endif
    return rtn;
  }
  
  bool Start()
  {
    bool rtn = true;
    if(!OLED_setup())
    {
      rtn = false;
    }

    if(!SPIFFS_Start())
    {
      rtn = false;
    }
    OLED_write_start();
    //Conexion WiFi con datos 
    if(WiFi_Configuration())
    {
      OLED_write_done();
    }
    return rtn;
  }  

  int get_Msg_counter()
  {
    int rtn;
    rtn = Msg_Event_counter;
    return rtn;
  }
  
  bool Msg_Event_Creation (void* param)
  {
    //Msg_Event_Object* temp = (Msg_Event_Object*) param;
	
    bool rtn=false;
      
    if(set_Msg_Event())
    {
      //Events_Array[Msg_Event_counter-1] = new Msg_Event_Object(temp->msg_lenght,temp->msg_text,temp->msg_protocol,temp->msg_lines);
      Msg_Events[Msg_Event_counter-1]=param;
      rtn = true;
    }
    #ifdef DEBUG
      Serial.printf("Protocolo Creation: %i \n", temp->msg_protocol);
    #endif
    return rtn;
 }

  void Process_Msg_Event()
  {
    Msg_Event_Object* temp = (Msg_Event_Object*) get_Msg_Event();
    //Msg_Processing(temp,&MIB_Msg);
    //MIB_Msg.print_OID_data();
    Attended_Msg_Event(temp);
  }

 void* get_Msg_Event ()
 {   
   if(clear_Msg_Event())
   {
    return Msg_Events[Msg_Event_attended];
    //Msg_Event_Object* temp = Events_Array[Msg_Event_attended];
    /*
    #ifdef DEBUG
      Serial.printf("Protocolo Recover: %i", temp->msg_protocol);
    #endif
    */
    //return (void*) temp;     
   }
   return NULL;
 }
 
 void Attended_Msg_Event(Msg_Event_Object* Event)
 {
  #ifdef DEBUG
    Serial.println("Memory Freed");
  #endif
   Msg_Event_attended ++;
   Event->clear_n_lines();
   Event->clear_n_oids();
   if(Msg_Event_attended >= Msg_Event_counter) 
   {
      #ifdef TEST
      Serial.println("Todos los eventos han sido atendidos");
      #endif
      Msg_Event_attended = 0;
      Msg_Event_counter = 0;
   }
 }
  
  int pending_events()
 {
   int rtn = 0;
   rtn = Msg_Event_counter - Msg_Event_attended;
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
   bool rtn=false;
   rtn = WiFi_Configuration();
   return rtn;
 }

 void WiFi_Attention_OLED()
 {
   WiFi_Attention();
 }

};


void Protocol_Decode(char* SNMP_Data, int * protocol, char* n_lines)
{
  String temp;

  temp = SNMP_Data;

  n_lines[0] = temp[0]-'0';
  protocol[0] = temp.toInt()-1000*(int)n_lines[0];
}

#endif
