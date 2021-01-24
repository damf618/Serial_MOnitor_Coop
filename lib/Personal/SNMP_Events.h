#ifndef SNMP_Events_h
#define SNMP_Events_h

class SNMP_Event_Object
{ 
  private:
    int  protocol_OID;
    int output;
    char n_lines;
    bool pro_upd;
    bool out_upd;
    bool nlines_upd;
  public:
  SNMP_Event_Object(int protocol, int state, char nlines)
  {
    protocol_OID  = protocol;
    output        = state;
    n_lines       = nlines;
    pro_upd       = false;
    out_upd       = false;
    nlines_upd    = false;
  }
  void Update_SNMP_Event(int protocol, int state, char nlines)
  {
    if(n_lines != nlines)
    {
      n_lines = nlines;
      nlines_upd = true;
      pro_upd = true;
      #ifdef TEST
      Serial.println("Numero de lineas: Actualizado");
      #endif
    }
    #ifdef TEST
    else
    {
      Serial.println("Numero de lineas: No se realizan cambios");
    }
    #endif

    if(protocol_OID != protocol)
    {
      protocol_OID = protocol;
      pro_upd = true;
      #ifdef TEST
      Serial.println("Protocolo: Actualizado");
      #endif
    }
    #ifdef TEST
    else
    {
      Serial.println("Protocolo: No se realizan cambios");
    }
    #endif

    if(output != state)
    {
      output = state;
      out_upd = true;
      #ifdef TEST
      Serial.println("Salida: Actualizado");
      #endif
    }
    #ifdef TEST
    else
    {
      Serial.println("Salida: No se realizan cambios");
    }
    #endif
  }
  bool SNMP_state_Event()
  {
    bool rtn=false;
    if(out_upd)
    {
      rtn = true;
      out_upd = false;
    }
    return rtn;
  } 
  bool SNMP_protocol_Event()
  {
    bool rtn=false;
    if(pro_upd)
    {
      rtn = true;
      pro_upd = false;
      nlines_upd = false;
    }
    return rtn;
  }
  int get_protocol_SNMP()
  {
    return protocol_OID;
  }
  int get_state_SNMP()
  {
    return output;
  }
  char get_nlines_SNMP()
  {
    return n_lines;
  }

};


#endif
