#ifndef MIB_h
#define MIB_h

#include <Arduino_SNMP.h>

#define BASE_OID ".1.3.6.1.4.1.6"
#define VECTOR_LENGTH 36
#define MAX_N_LINES 4 
#define MAX_N_OIDS 1
#define MAX_SNMP_LENGTH 32


//SNMP Configurations
int      VAR_OID1 = 0;
char*    VAR_OID3 = "2555";         //MAX 31 caracteres
int      VAR_OID2 = 1;

int tensOfMillisCounter = 0;

//ValueCallback*     OID1_TRAP;
//ValueCallback*     OID2_TRAP;
//ValueCallback*     OID3_TRAP;
ValueCallback*     A_TRAP;
ValueCallback*     B_TRAP;
ValueCallback*     C_TRAP;
ValueCallback*     D_TRAP;
ValueCallback*     E_TRAP;
ValueCallback*     F_TRAP;
ValueCallback*     G_TRAP;
ValueCallback*     H_TRAP;
ValueCallback*     I_TRAP;
ValueCallback*     J_TRAP;
TimestampCallback* timestampCallbackOID;

WiFiUDP udp;
SNMPAgent snmp = SNMPAgent("public"); // Starts an SMMPAgent instance with the community string 'public'
SNMPTrap* settableNumberTrap = new SNMPTrap("public", 0);

//OIDs 
#define OID_1 ".1.3.6.1.4.1.5.0"
#define OID_2 ".1.3.6.1.4.1.5.1"
#define OID_3 ".1.3.6.1.4.1.5.2"
#define OID_A ".1.3.6.1.4.1.7.0"
#define OID_B ".1.3.6.1.4.1.7.1"
#define OID_C ".1.3.6.1.4.1.7.2"
#define OID_D ".1.3.6.1.4.1.7.3"
#define OID_E ".1.3.6.1.4.1.7.4"
#define OID_F ".1.3.6.1.4.1.7.5"
#define OID_G ".1.3.6.1.4.1.7.6"
#define OID_H ".1.3.6.1.4.1.7.7"
#define OID_I ".1.3.6.1.4.1.7.8"
#define OID_J ".1.3.6.1.4.1.7.9"
#define OID_TRAP ".1.3.6.1.2.1.33.2"
#define OID_TIME ".1.3.6.1.2.1.1.3.0"

//Trap IP
#define IP_1 192
#define IP_2 168
#define IP_3 0
#define IP_4 224

IPAddress destinationIP = IPAddress(IP_1,IP_2,IP_3,IP_4);

class MIB_MSG_Object
{ 
  private:
    char*  base;
    int    counter;
    char*  VAR_OIDS_L1[2];
    char*  VAR_OIDS_L2[2];
    char*  VAR_OIDS_L3[2];
    char*  VAR_OIDS_L4[2];
    char*  VAR_OIDS_L5[2];
    char** DATA_OIDS[5];
  public:   
    char   OID1[32];
    char   OID2[32];
    char   OID3[32];
    char   OID4[32];
    char   OID5[32];
    char   OID6[32];
    char   OID7[32];
    char   OID8[32];
    char   OID9[32];
    char   OID10[32];
    char * temp1= OID1;
    char * temp2= OID2;
    char * temp3= OID3;
    char * temp4= OID4;
    char * temp5= OID5;
    char * temp6= OID6;
    char * temp7= OID7;
    char * temp8= OID8;
    char * temp9= OID9;
    char * temp10= OID10;



  MIB_MSG_Object()
  {
    base=BASE_OID;
    counter=0;
    char*  TVAR_OIDS_L1[2]={OID1,OID2};
    char*  TVAR_OIDS_L2[2]={OID3,OID4};
    char*  TVAR_OIDS_L3[2]={OID5,OID6};
    char*  TVAR_OIDS_L4[2]={OID7,OID8};
    char*  TVAR_OIDS_L5[2]={OID9,OID10};
    char** TDATA_OIDS[]={VAR_OIDS_L1,VAR_OIDS_L2,VAR_OIDS_L3,VAR_OIDS_L4,VAR_OIDS_L5};

    memcpy(VAR_OIDS_L1,TVAR_OIDS_L1,sizeof(TVAR_OIDS_L1));
    memcpy(VAR_OIDS_L2,TVAR_OIDS_L2,sizeof(TVAR_OIDS_L2));
    memcpy(VAR_OIDS_L3,TVAR_OIDS_L3,sizeof(TVAR_OIDS_L3));
    memcpy(VAR_OIDS_L4,TVAR_OIDS_L4,sizeof(TVAR_OIDS_L4));
    memcpy(VAR_OIDS_L5,TVAR_OIDS_L5,sizeof(TVAR_OIDS_L5));
    memcpy(DATA_OIDS,TDATA_OIDS,sizeof(DATA_OIDS));
  }

  void MIB_Save_Data(int nlines,int noids, char* data)
  { 
    for(int i=0;i<=VECTOR_LENGTH;i++)
    {
      DATA_OIDS[nlines][noids][i]=data[i];
    }
  }

  void clean_data()
  {
    
    char Clean[32]= "                               " ;
    for(int j=0;j<=MAX_N_LINES;j++)
    {
      for(int i=0;i<=MAX_N_OIDS;i++)
      {
        #ifdef DEBUG  
        Serial.println(DATA_OIDS[j][i]);
        #endif
        memcpy(DATA_OIDS[j][i],Clean,MAX_SNMP_LENGTH); 
        #ifdef DEBUG
        Serial.println("Cleaned");
        Serial.println(DATA_OIDS[j][i]);   
        #endif
      }
    }
  }
  
  int get_SNMP_state_()
  {
    return VAR_OID1;
  }

  int get_SNMP_enable()
  {
    return VAR_OID2;
  }

  char * get_SNMP_protocol()
  {
    return VAR_OID3;
  }

  char* OID_data(int nlines, int noids)
  {
    return DATA_OIDS[nlines][noids];
  }

  void print_OID_data()
  {
    int count=0;
    for(int i=0;i<=4;i++)
    {
      for(int j=0;j<=1;j++)
      {
        count++;
        #ifdef DEBUG
        Serial.printf("\n i: %i \n j: %i \n OID%i: \n %s",i,j,count,DATA_OIDS[i][j]);
        #else
          #ifdef TEST
          Serial.printf("OID%i: \n %s \n",count,DATA_OIDS[i][j]);
          #endif
        #endif
      }
    }
  }

void SNMP_Config()
{
  // give snmp a pointer to the UDP object
    snmp.setUDP(&udp);
    snmp.begin();
    
    //SNMP Protocol Configuration
    timestampCallbackOID = (TimestampCallback*)snmp.addTimestampHandler(OID_TIME, &tensOfMillisCounter);

    snmp.addIntegerHandler(OID_1,&VAR_OID1, true);
    snmp.addIntegerHandler(OID_2,&VAR_OID2, true);
    snmp.addStringHandler (OID_3,&VAR_OID3, true);
    A_TRAP = snmp.addStringHandler(OID_A,&temp1);
    B_TRAP = snmp.addStringHandler(OID_B,&temp2);
    C_TRAP = snmp.addStringHandler(OID_C,&temp3);
    D_TRAP = snmp.addStringHandler(OID_D,&temp4);
    E_TRAP = snmp.addStringHandler(OID_E,&temp5);
    F_TRAP = snmp.addStringHandler(OID_F,&temp6);
    G_TRAP = snmp.addStringHandler(OID_G,&temp7);
    H_TRAP = snmp.addStringHandler(OID_H,&temp8);
    I_TRAP = snmp.addStringHandler(OID_I,&temp9);
    J_TRAP = snmp.addStringHandler(OID_J,&temp10);

    settableNumberTrap->setUDP(&udp); // give a pointer to our UDP object
    settableNumberTrap->setTrapOID(new OIDType(OID_TRAP)); // OID of the trap
    settableNumberTrap->setSpecificTrap(1); 

    settableNumberTrap->setUptimeCallback(timestampCallbackOID);

    // Set some previously set OID Callbacks to send these values with the trap
    settableNumberTrap->addOIDPointer(A_TRAP);
    settableNumberTrap->addOIDPointer(B_TRAP);
    settableNumberTrap->addOIDPointer(C_TRAP);
    settableNumberTrap->addOIDPointer(D_TRAP);
    settableNumberTrap->addOIDPointer(E_TRAP);
    settableNumberTrap->addOIDPointer(F_TRAP);
    settableNumberTrap->addOIDPointer(G_TRAP);
    settableNumberTrap->addOIDPointer(H_TRAP);
    settableNumberTrap->addOIDPointer(I_TRAP);
    settableNumberTrap->addOIDPointer(J_TRAP);

    settableNumberTrap->setIP(WiFi.localIP()); // Set our Source IP

    snmp.sortHandlers();
}



};

MIB_MSG_Object MIB_Msg;

void SNMP_Trap_Active()
{
      #ifdef TEST
      if(settableNumberTrap->sendTo(destinationIP)){ // Send the trap to the specified IP address
      Serial.println("Sent SNMP Trap");
      } else {
         Serial.println("Couldn't send SNMP Trap");
      }
      #else
      settableNumberTrap->sendTo(destinationIP);
      #endif
      

}




#endif
