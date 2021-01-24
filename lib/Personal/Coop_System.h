#ifndef COOPSystem_h
#define COOPSystem_h

#include <Msg_Events.h>
#include <MIB.h>
#include <SNMP_Events.h>
#include <SNMP_Config.h>

#define OUTPUT_GPIO 5

//TODO Send to .C
String temp_s;
Msg_Event_Object temp_event(1,temp_s,1,1);
Msg_Event_Object** Events_Array = new Msg_Event_Object*[MAX_MSGS_EVENTS];
SNMP_Event_Object SNMP_Event(555,0,2);
extern MIB_MSG_Object MIB_Msg;
bool update_flag=false;

//Wi-Fi Parameters
//const char* ssid = "Fibertel WiFi654 2.4GHz";
//const char* ssid = "Telecentro-ee00";
const char* ssid = "TeleCentro-14d4";
//const char* password = "0049595647";
//const char* password = "QZYKZMMYMZYX";
const char* password = "HDZ3GTKFJN4G";
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
  
  void Start()
  {
    pinMode(OUTPUT_GPIO, OUTPUT);
     //Wi-Fi Configuration
    WiFi.begin(ssid, password);
    
    Serial.printf("\n Conexion con Red %s \n",ssid);
    
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        #ifdef TEST
        Serial.print(".");
        #endif
    }
    
    Serial.print(" \n Direccion IP: ");
    Serial.println(WiFi.localIP());
    
    MIB_Msg.SNMP_Config(); 
  }  
  int get_SNMP_State()
  {
    return MIB_Msg.get_SNMP_state_();
  }

  char* get_SNMP_Protocol()
  { 
    return MIB_Msg.get_SNMP_protocol();
  }

  int get_Msg_counter()
  {
    int rtn;
    rtn = Msg_Event_counter;
    return rtn;
  }
  
  bool Msg_Event_Creation (void* param)
  {
    Msg_Event_Object* temp = (Msg_Event_Object*) param;
	
    bool rtn=false;
      
    if(set_Msg_Event())
    {
      Events_Array[Msg_Event_counter-1] = new Msg_Event_Object(temp->msg_lenght,temp->msg_text,temp->msg_protocol,temp->msg_lines);
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
    Msg_Processing(temp,&MIB_Msg);
    MIB_Msg.print_OID_data();
    Attended_Msg_Event(temp);
    update_flag= true; 
  }

  void SNMP_Event_Creation (int protocol, int state, char nlineas)
  {
    SNMP_Event.Update_SNMP_Event(protocol,state,nlineas);

    #ifdef DEBUG
    Serial.printf("Event Creation \n");
    #endif
 }

  void Process_SNMP_Event ()
  {
    if(SNMP_Event.SNMP_state_Event())
    {
      Output_Update(SNMP_Event.get_state_SNMP());
    }
    
    if(SNMP_Event.SNMP_protocol_Event())
    {
      #ifdef TEST
      Serial.printf("New Protocol: %i \n",SNMP_Event.get_protocol_SNMP());
      #endif
      MIB_Msg.clean_data();
    }
    #ifdef DEBUG
    Serial.printf("Event Creation \n");
    #endif
  }

  void Output_Update(int state)
  {
    if(state==1)
    {
      #ifdef TEST
      Serial.println("Salida: ON");
      #endif
      digitalWrite(OUTPUT_GPIO, HIGH);
    }
    else
    {
      #ifdef TEST
      Serial.println("Salida: OFF");
      #endif
      digitalWrite(OUTPUT_GPIO, LOW);
    }
  }
 
 void* get_Msg_Event ()
 {   
   if(clear_Msg_Event())
   {
    Msg_Event_Object* temp = Events_Array[Msg_Event_attended];
    #ifdef DEBUG
      Serial.printf("Protocolo Recover: %i", temp->msg_protocol);
    #endif
    return (void*) temp;     
   }
   return NULL;
 }
 
 bool Attended_Msg_Event(Msg_Event_Object* Event)
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

  bool enable()
  {
    bool rtn = true;
    if(1!=MIB_Msg.get_SNMP_enable())
    {
      rtn = false;
    }
    return rtn;
  }
  
};


void Protocol_Decode(char* SNMP_Data, int * protocol, char* n_lines)
{
  String temp;

  temp = SNMP_Data;
  /*
  #ifdef DEBUG
    Serial.printf("Original: %s \nchar: %c\n",SNMP_Data,temp[0]);
  #endif
  */
  n_lines[0] = temp[0]-'0';
  protocol[0] = temp.toInt()-1000*(int)n_lines[0];
  /*
  #ifdef DEBUG
    Serial.printf("Number of lines: %i \nProtocolo: %i\n",n_lines[0],protocol[0]);
  #endif
  
  if(update_flag)
    {
      SNMP_Trap_Active();
      update_flag=false;      
    }
*/

}

void Trap_Update()
{
  if(update_flag)
    {
      SNMP_Trap_Active();
      update_flag=false;      
    }
}
#endif
