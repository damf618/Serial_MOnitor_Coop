#ifndef Firebase_Comm_h
#define Firebase_Comm_h

#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <SPIFFS_Serial_Monitor.h>
//#include <OLED.h>
#include <Simplex_Protocol.h>

#define API_KEY "AIzaSyA7wXtS7zdsoGu7d5YBviLhCLpmWhPisU4"
#define FIREBASE_HOST "sistema-de-monitoreo-72a42-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "N6jkN9OnR7bC8wHNw3VsPl8afBikm0ZzIYihTPIM"
#define USER_FLAG "User Created"
#define ENABLE_PATH "/Enable"
#define INT_UPLOAD_PATH "/Abnormal_Devices/"
#define UPLOAD_DEVICE "Device"
#define FIREBASE_TRIES 70

static FirebaseAuth auth;
static FirebaseConfig config;
static FirebaseData fbdo;

char enable_path[100] = "";
char path_dataup[100] = "";

String path = "";
int count = 0;
int count1 = 0;

void prepareDatabaseRules(const char *var, const char *readVal, const char *writeVal)
{
  char path_push[100] = "";
  //We will sign in using legacy token (database secret) for full RTDB access
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //path = readFile(SPIFFS, "/FACILITY.txt");
  strcpy(path_push, readFile(SPIFFS, "/FACILITY.txt").c_str());

//String Firebase_Div = "/";
//path = auth.token.uid.c_str()+Firebase_Div+readFile(SPIFFS, "/FACILITY.txt");
#ifdef TEST
  Serial.println(F("------------------------------------"));
  Serial.println(F("Read database ruless..."));
#endif
  if (Firebase.getRules(fbdo))
  {
    FirebaseJsonData result;
    FirebaseJson &json = fbdo.jsonObject();
    bool rd = false, wr = false;

    String _path = "rules";
    if (path[0] != '/')
      _path += "/";
    _path += path;
    _path += "/";
    _path += var;

    if (strlen(readVal) > 0)
    {
      rd = true;
      json.get(result, _path + "/.read");
      if (result.success)
        if (strcmp(result.stringValue.c_str(), readVal) == 0)
          rd = false;
    }

    if (strlen(writeVal) > 0)
    {
      wr = true;
      json.get(result, _path + "/.write");
      if (result.success)
        if (strcmp(result.stringValue.c_str(), writeVal) == 0)
          wr = false;
    }

    //modify if the rules changed or does not exist.
    if (wr || rd)
    {
      FirebaseJson js;
      std::string s;
      if (rd)
        js.add(".read", readVal);

      if (wr)
        js.add(".write", writeVal);

#ifdef TEST
      Serial.println(F("Set database rules..."));
#endif

      json.set(_path, js);
      String rules = "";
      json.toString(rules, true);
#ifdef TEST
      if (!Firebase.setRules(fbdo, rules))
      {
        Serial.println("Failed to edit the database rules, " + fbdo.errorReason());
      }
#else
      Firebase.setRules(fbdo, rules);
#endif
    }

    json.clear();
  }
#ifdef TEST
  else
  {
    Serial.println("Failed to read the database rules, " + fbdo.errorReason());
  }
#endif
}

/* The helper function to get the token type string */
String getTokenType(struct token_info_t info)
{
  switch (info.type)
  {
  case token_type_undefined:
    return "undefined";

  case token_type_legacy_token:
    return "legacy token";

  case token_type_id_token:
    return "id token";

  case token_type_custom_token:
    return "custom token";

  case token_type_oauth2_access_token:
    return "OAuth2.0 access token";

  default:
    break;
  }
  return "undefined";
}

/* The helper function to get the token status string */
String getTokenStatus(struct token_info_t info)
{
  switch (info.status)
  {
  case token_status_uninitialized:
    return "uninitialized";

  case token_status_on_signing:
    return "on signing";

  case token_status_on_request:
    return "on request";

  case token_status_on_refresh:
    return "on refreshing";

  case token_status_ready:
    return "ready";

  case token_status_error:
    return "error";

  default:
    break;
  }
  return "uninitialized";
}

/* The helper function to get the token error string */
String getTokenError(struct token_info_t info)
{
  String s = "code: ";
  s += String(info.error.code);
  s += ", message: ";
  s += info.error.message.c_str();
  return s;
}

bool Firebase_Set_Up()
{
  bool signupOK = false;
  char username[100] = "";
  char user_psw[100] = "";
  char user_created_c[100] = "";
  char aux[100] = "";
  char var[10] = "$user";
  char val[25] = "(auth.uid === $user)";
  //String user_created;

  strcpy(username, readFile(SPIFFS, "/USERNAME.txt").c_str());
  strcpy(user_psw, readFile(SPIFFS, "/USER_PSW.txt").c_str());
  strcpy(user_created_c, readFile(SPIFFS, "/USER.txt").c_str());
  strcpy(aux, USER_FLAG);

#ifdef TEST
  Serial.print(F("Username: "));
  Serial.println(username);
  Serial.print(F("Password: "));
  Serial.println(user_psw);
  Serial.print(F("User created?: "));
  Serial.println(user_created_c);
  Serial.print(F("Aux: "));
  Serial.println(aux);
#endif

#ifdef TEST
  Serial.println(F("Firebase config"));
#endif

  config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

#ifdef TEST
  Serial.println(F("Firebase String"));
#endif

#ifdef TEST
  Serial.println(F("Firebase auth"));

  if (0 != memcmp(aux, user_created_c, sizeof(aux)))
  {
    Serial.println(F("Not the same message!"));
  }
#endif

  auth.user.email = username;
  auth.user.password = user_psw;

#ifdef TEST
  Serial.println(F("Firebase wifi"));
#endif
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  //fbdo.setBSSLBufferSize(256, 256);
  fbdo.setBSSLBufferSize(128, 128);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  //fbdo.setResponseSize(256);
  fbdo.setResponseSize(128);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 5);
  //Firebase.setReadTimeout(fbdo, 1000 * 30);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "small");

  //OLED_write_Firebase_Start();

#ifdef TEST
  Serial.println(F("Firebase signup"));
  Serial.println(F("THINK1"));
#endif

  //OLED_write_Firebase_Think();

  if (0 != memcmp(aux, user_created_c, sizeof(aux)))
  {
    if (Firebase.signUp(&config, &auth, username, user_psw))
    {
#ifdef TEST
      Serial.println(F("Success"));
#endif
      writeFile(SPIFFS, "/USER.txt", USER_FLAG);
    }
#ifdef TEST
    else
    {
      Serial.printf("Failed, %s\n", config.signer.signupError.message.c_str());
    }
#endif

#ifdef TEST
    Serial.println(F("THINK2"));
#endif

    //OLED_write_Firebase_Think();

    prepareDatabaseRules(var, val, val);
  }
  else
  {

    //OLED_write_Firebase_Think();

#ifdef TEST
    Serial.println(F("User already created"));
#endif
  }

#ifdef TEST
  Serial.println(F("THINK3"));
#endif

  //OLED_write_Firebase_Think();

#ifdef TEST
  Serial.println(F("THINK4"));
#endif

  //OLED_write_Firebase_Think();

#ifdef TEST
  Serial.println(F("THINK5"));
#endif

  Firebase.begin(&config, &auth);

#ifdef TEST
  Serial.println(F("THINK7"));
#endif
  path = auth.token.uid.c_str();
  path += "/";
  path += readFile(SPIFFS, "/FACILITY.txt");

#ifdef TEST
  Serial.print(F("Facility_Name: "));
  Serial.println(path);
#endif

  signupOK = true;
#ifdef TEST
  Serial.println(F("THINK6"));
#endif

  //OLED_write_Firebase_Done();

  strcpy(enable_path, path.c_str());
  strcat(enable_path, ENABLE_PATH);

#ifdef TEST
  Serial.print(F("Enable_Path: "));
  Serial.println(enable_path);
#endif

  strcpy(path_dataup, path.c_str());
  strcat(path_dataup, INT_UPLOAD_PATH);

#ifdef TEST
  Serial.print(F("Upload_Path: "));
  Serial.println(path_dataup);
#endif
  return signupOK;
}

bool Firebase_First_Push()
{

  bool rtn = false;

  struct token_info_t info = Firebase.authTokenInfo();

#ifdef TEST
  Serial.println(F("------------------------------------"));
  if (info.status == token_status_error)
  {
    Serial.printf("Token info: type = %s, status = %s\n", getTokenType(info).c_str(), getTokenStatus(info).c_str());
    Serial.printf("Token error: %s\n\n", getTokenError(info).c_str());
  }
  else
  {
    Serial.printf("Token info: type = %s, status = %s\n\n", getTokenType(info).c_str(), getTokenStatus(info).c_str());
  }

  Serial.println(F("------------------------------------"));
  Serial.println(F("Set int test..."));
  Serial.print(F("Set Path: "));
  Serial.println(path);
//CUIDADO SI YA EXISTE GENERA FALLAS EN EL SISTEMA
#endif

  if (Firebase.setString(fbdo, path + "/Usuario", auth.token.uid.c_str()))
  {
    rtn = true;
#ifdef TEST
    Serial.println(F("PASSED"));
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    Serial.println("ETag: " + fbdo.ETag());
    Serial.print(F("VALUE: "));
    //printResult(fbdo);
    Serial.println(F("------------------------------------"));
    Serial.println();
#endif
  }
#ifdef TEST
  else
  {
    Serial.println(F("FAILED"));
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println(F("------------------------------------"));
    Serial.println();
  }
#endif
  return rtn;
}

int Firebase_Enable()
{
  int rtn = false;
  count++;
  if (count < FIREBASE_TRIES)
  {
    if (Firebase.getInt(fbdo, enable_path))
    {
      if (fbdo.dataType() == "int")
      {
#ifdef TEST
        Serial.print(F("DATA READ from Firebase: "));
        Serial.println(fbdo.intData());
        Serial.print(F("VALUE: "));
        Serial.println(count);
#endif
        rtn = fbdo.intData();
      }
      else
      {
#ifdef TEST
        Serial.print(F("Invalid Data Received from Firebase"));
#endif
        rtn = 1;
      }
    }
#ifdef TEST
    else
    {
      Serial.println(F("FAILED"));
    }
#endif
  }
  return rtn;
}

bool Firebase_Check_Conn()
{
  bool rtn = false;
  #ifdef TEST
  if (FIREBASE_TRIES <= count)
  {
    Serial.println(F("Recuperacion Periodica"));
  }
  else
  {
    Serial.println(F("Recuperacion ForXSada"));  
  }
  #endif

    rtn = fbdo.httpConnected();
#ifdef TEST
    Serial.println(F("Validacion de Conexion HTTP"));
    if (rtn)
    {
      Serial.println(F("Conexion HTTP OK!"));
    }
#endif
    rtn = fbdo.pauseFirebase(1);
    if (rtn)
    {
      Serial.println(F("Conexion Firebase Pausada"));
      delay(1000);
      rtn = fbdo.pauseFirebase(0);
      if (rtn)
      {
        Serial.println(F("Conexion Firebase Reiniciada"));
        count = 0;
        Firebase.begin(&config, &auth);
        delay(500);
        if (Firebase_Enable())
        {
          Serial.println(F("Conexion Restablecida"));
        }
        else
        {
          Serial.println(F("Conexion Erronea"));
        }
      }
    }
    else
    {
      Serial.println(F("Conexion NO pudo ser Pausada"));
    }

  return rtn;
}

bool dataupload()
{
  bool rtn = false;
#ifdef TEST
  Serial.println(F("Firebase Data Upload"));
#endif
  count++;
  count1++;
  if (count <= FIREBASE_TRIES)
  {
    if (Firebase.setInt(fbdo, path_dataup, count1))
    {
      rtn = true;

#ifdef TEST
      Serial.println(F("PASSED"));
      Serial.print(F("VALUE: "));
      Serial.println(count);
#endif
    }
#ifdef TEST
    else
    {
      Serial.println(F("FAILED"));
      Serial.println("REASON: " + fbdo.errorReason());
      Serial.println(F("------------------------------------"));
      Serial.println();
    }
#endif
  }
  /*
  else
  {
    Firebase_Check_Conn();
  }
  */
  return rtn;
}

bool Firebase_Clean()
{
  bool rtn = false;
  count++;
  if (Firebase.set(fbdo, path_dataup, ""))
  {
    rtn = true;
#ifdef TEST
    Serial.println(F("PASSED"));
    Serial.print(F("VALUE: "));
    Serial.println(count);
#endif
  }
#ifdef TEST
  else
  {
    Serial.println(F("FAILED"));
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println(F("------------------------------------"));
    Serial.println();
  }
#endif

  return rtn;
}

bool Firebase_Clean_Node(const char index)
{
  bool rtn = false;

  char aux[100] = "";

  count++;

  strcpy(aux, path_dataup);
  aux[strlen(path_dataup)] = index + '0';

  if (Firebase.deleteNode(fbdo, aux))
  {
    rtn = true;
#ifdef TEST
    Serial.println(F("PASSED"));
    Serial.print(F("VALUE: "));
    Serial.println(count);
#endif
  }
#ifdef TEST
  else
  {
    Serial.println(F("FAILED"));
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println(F("------------------------------------"));
    Serial.println();
  }
#endif

  return rtn;
}

bool dataupload2(FirebaseJsonArray *data, char index)
{
  bool rtn = false;
  char aux[100] = "";

#ifdef TEST
  Serial.println(F("Firebase JSON Upload"));
#endif

  strcpy(aux, path_dataup);
  aux[strlen(path_dataup)] = index + '0';
  //prepara que ocurre en caso de que sean mas de 9.....
  //strcat(aux,index);

#ifdef TEST
  Serial.print(F("path: "));
  Serial.println(aux);
  Serial.print(F("index: "));
  Serial.println(index);
  Serial.print(F("original: "));
  Serial.println(path_dataup);
#endif

  count++;
  if (count <= FIREBASE_TRIES)
  {
    //if (Firebase.setArray(fbdo, path_dataup, data[0]))
    if (Firebase.setArray(fbdo, aux, data[0]))
    {
      rtn = true;

#ifdef TEST
      Serial.println(F("PASSED"));
      Serial.print(F("VALUE: "));
      Serial.println(count);
#endif
    }
#ifdef TEST
    else
    {
      Serial.println(F("FAILED"));
      Serial.println("REASON: " + fbdo.errorReason());
      Serial.println(F("------------------------------------"));
      Serial.println();
    }
#endif
  }
  /*
  else
  {
    Firebase_Check_Conn();
  }
  */

  return rtn;
}

void Json_Setup(String point_name, String name, String type, String status, bool mode, FirebaseJson *json1)
{
  json1[0].set("id", point_name);
  json1[0].set("name", name);
  json1[0].set("type", type);
  json1[0].set("status", status);

  /*
  if(mode)
  {
    json1[0].set("status", "PROBLEMA");
  }
  else
  {
    json1[0].set("status", "ALARMA");
  }
  */
}
/*
FirebaseJson JSON_Conversion(char* Msg_Line, bool mode)
{
  FirebaseJson data;
  message_parser_t rtn; 
  #ifdef TEST
    String jsonStr;
    jsonStr.reserve(100);
  #endif

  rtn = Separator_Search(Msg_Line);
    
  Json_Setup(rtn.point_name,rtn.name,rtn.type,rtn.status,mode,&data);
    
  #ifdef TEST
    Serial.println(" ------------------------------- ");
    data.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println(" ------------------------------- ");
    Serial.println("End of JSON Conversion");
  #endif
  return data;
}
*/

void JSON_Conversion2(char *Msg_Line, bool mode, FirebaseJson *data)
{

  message_parser_t rtn;
#ifdef TEST
  String jsonStr;
  jsonStr.reserve(100);
#endif

  rtn = Separator_Search(Msg_Line, mode);
#ifdef TEST
  Serial.print(F("status: "));
  Serial.println(rtn.status);
#endif

  Json_Setup(rtn.point_name, rtn.name, rtn.type, rtn.status, mode, data);

#ifdef TEST
  Serial.println(F(" ------------------------------- "));
  data[0].toString(jsonStr, true);
  Serial.println(jsonStr);
  Serial.println(F(" ------------------------------- "));
  Serial.println(F("End of JSON Conversion"));
#endif
  //return data;
}

#endif