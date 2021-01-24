#ifndef MSG_Events_h
#define MSG_Events_h

#include <Coop_System.h>
#include <MIB.h>

#define MAX_MSGS_EVENTS 100
#define FULL_BUFFER 320
#define SNMP_LENGTH 32
#define POINTER_LENGTH 36
#define SPACE 32

class Msg_Event_Object
{ 
  private:
    int n_lines;
    int n_oids;  
  public:
    int     msg_lenght;        
    String  msg_text;
    int     msg_protocol;
    int     msg_lines;

  Msg_Event_Object(int largo, String text, int protocol, int lines)
  {
    msg_lenght     = largo;        
    msg_text       = text;
    msg_protocol   = protocol;
    msg_lines      = lines;
    n_lines        = 0;
    n_oids         = 0;
  }
  int get_n_nlines()
  {
    int rtn;
    rtn = n_lines;
    return rtn;
  }
  void add_n_nlines()
  {
    n_lines++;
  }
  int get_n_oids()
  {
    int rtn;
    rtn = n_oids;
    return rtn;
  }
  void add_n_oids()
  {
    n_oids++;
  }
  void clear_n_oids()
  {
    n_oids=0;
  }
  void clear_n_lines()
  {
    n_lines=0;
  }

};


int Data_Text(int init_char,int final_char,char* Data,char* Extract_Data){
  int rtn =0;
  char temp[SNMP_LENGTH];
  int j=0;
  for (int i=init_char;i<=final_char ;i++){
    if(Data[i]=='\n')
    {
      #ifdef DEBUG
      Serial.println("\n New OID \n");
      #endif
      rtn = i;
      break;
    }
    else
    {
      temp[i-init_char]=Data[i];
    }
  }
  if(0==rtn)
  {
    j=final_char+1;
  }
  else
  {
    j=rtn;
  }
  for (j;j-init_char<=POINTER_LENGTH;j++)
  {
    temp[j-init_char]=SPACE;
  }
  strcpy(Extract_Data,temp);
  return rtn;
}

void Msg_Processing(Msg_Event_Object* Event, MIB_MSG_Object* DATA_OIDS )
{  
  char* ptr;
  int temp_int=0;
  int count=31;
  int index=0;
  
  #ifdef DEBUG
  Serial.printf("Protocolo: %i", Event->msg_protocol);
  #endif
  
  switch(Event->msg_protocol)
  {
    case 555:
      #ifdef TEST
      Serial.println("Protocolo Simplex Seleccionado");
      #endif
      char temp[FULL_BUFFER];
      Event->msg_text.toCharArray(temp,Event->msg_lenght);
      #ifdef DEBUG
      Serial.println("Char converssion: ");
      Serial.println(temp);
      #endif
        
      while (Event->msg_lenght > index)
      {
        ptr = (char*)malloc(sizeof(char));
        if (ptr==NULL) {
          Serial.println("No memory available");
        }
        else
        {
          #ifdef DEBUG
            Serial.printf("From index: %i to index: %i \n",index,count);
          #endif
          temp_int = Data_Text(index,count,temp,ptr);
          #ifdef DEBUG
            Serial.printf("OID Data: %s \n",ptr);
	  #endif
          //DATA ASSIGNATION
          DATA_OIDS->MIB_Save_Data(Event->get_n_nlines(),Event->get_n_oids(),ptr);
          #ifdef DEBUG
            Serial.printf("number of lines: %i, number of oids: %i \n",Event->get_n_nlines(),Event->get_n_oids());
          #endif
          Event->add_n_oids(); 
          if(0 != temp_int)
          {
            index = temp_int+1;
            Event->add_n_nlines();
            Event->clear_n_oids();
          }
          else
          {
            //index = count+1;
            index = count;
          }
	  count = index + 31;
          if(Event->msg_lenght < count)
          {
            count = Event->msg_lenght;
          }
          free(ptr);
        }
      }
      break;
    default:
      Serial.println("Se debe selecccionar un protocolo adecuado");
  }
}

#endif
