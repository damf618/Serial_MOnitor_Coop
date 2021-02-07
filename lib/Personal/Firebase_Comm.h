#ifndef Firebase_Comm_h
#define Firebase_Comm_h

#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SPIFFS_Serial_Monitor.h>
#include <OLED.h>

#define API_KEY "AIzaSyA7wXtS7zdsoGu7d5YBviLhCLpmWhPisU4"
#define FIREBASE_HOST "sistema-de-monitoreo-72a42-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "N6jkN9OnR7bC8wHNw3VsPl8afBikm0ZzIYihTPIM"
#define USER_FLAG "User Created"

#define TEST

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String path = "";
int count = 0;

void prepareDatabaseRules(const char *path, const char *var, const char *readVal, const char *writeVal)
{
  //We will sign in using legacy token (database secret) for full RTDB access
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  #ifdef TEST
  Serial.println("------------------------------------");
  Serial.println("Read database ruless...");
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
      Serial.println("Set database rules...");
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

void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();
    for (size_t i = 0; i < arr.size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
  else if (data.dataType() == "blob")
  {

    Serial.println();

    for (size_t i = 0; i < data.blobData().size(); i++)
    {
      if (i > 0 && i % 16 == 0)
        Serial.println();

      if (i < 16)
        Serial.print("0");

      Serial.print(data.blobData()[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  else if (data.dataType() == "file")
  {

    Serial.println();

    File file = data.fileStream();
    int i = 0;

    while (file.available())
    {
      if (i > 0 && i % 16 == 0)
        Serial.println();

      int v = file.read();

      if (v < 16)
        Serial.print("0");

      Serial.print(v, HEX);
      Serial.print(" ");
      i++;
    }
    Serial.println();
    file.close();
  }
  else
  {
    Serial.println(data.payload());
  }
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
  String user_created;
  #ifdef TEST
  Serial.println("Firebase config");
  #endif

  config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

  #ifdef TEST
  Serial.println("Firebase String");
  #endif
  String user_firebase = readFile(SPIFFS, "/USERNAME.txt");
  String psw_firebase = readFile(SPIFFS, "/USER_PSW.txt");

  #ifdef TEST
  Serial.println("Firebase auth");
  Serial.println(user_firebase.c_str());
  Serial.println(psw_firebase.c_str());
  #endif

  auth.user.email = user_firebase.c_str();
  auth.user.password = psw_firebase.c_str();

  #ifdef TEST
  Serial.println("Firebase wifi");
  #endif
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  //fbdo.setBSSLBufferSize(1024, 1024);
  fbdo.setBSSLBufferSize(128, 128);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  //fbdo.setResponseSize(1024);
  fbdo.setResponseSize(128);

  //Set database read timeout to 1 minute (max 15 minutes)
  //Firebase.setReadTimeout(fbdo, 1000 * 60);
  Firebase.setReadTimeout(fbdo, 1000 * 30);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "medium");
  
  
  OLED_write_Firebase_Start();

  #ifdef TEST
  Serial.println("Firebase signup");
  Serial.println("THINK1");
  #endif
  OLED_write_Firebase_Think();

  //user created previosuly?
  user_created = readFile(SPIFFS, "/USER.txt");
  String aux = USER_FLAG;
  if ( aux != user_created)
  {
    if (Firebase.signUp(&config, &auth, user_firebase.c_str(), psw_firebase.c_str()))
    {
      #ifdef TEST
      Serial.println("Success");
      #endif
      writeFile(SPIFFS, "/USER.txt", USER_FLAG);
    }
    #ifdef TEST
    else
    {
      Serial.printf("Failed, %s\n", config.signer.signupError.message.c_str());
    }
    #endif
  }
  #ifdef TEST
  else
  {
    Serial.println("User already created");
  }
  #endif

  String Firebase_Div = "/";
  String base_path = Firebase_Div + readFile(SPIFFS, "/FACILITY.txt") + Firebase_Div;
  #ifdef TEST
  Serial.println("Facility_Name");
  Serial.println(base_path);
  #endif
  String var = "$user";
  String val = "(auth.uid === $user)";
  #ifdef TEST
  Serial.println("THINK2");
  #endif
  OLED_write_Firebase_Think();
  prepareDatabaseRules(base_path.c_str(), var.c_str(), val.c_str(), val.c_str());
  #ifdef TEST
  Serial.println("THINK3");
  #endif
  OLED_write_Firebase_Think();

  path = base_path;
  #ifdef TEST
  Serial.println("Path");
  Serial.println(path);
  Serial.println("THINK4");
  #endif
  OLED_write_Firebase_Think();
  Firebase.begin(&config, &auth);
  path += auth.token.uid.c_str();
  signupOK = true;
  OLED_write_Firebase_Done();
  return signupOK;
}

bool Firebase_First_Push()
{
  bool rtn = false;
  struct token_info_t info = Firebase.authTokenInfo();
  
  #ifdef TEST
  Serial.println("------------------------------------");
  if (info.status == token_status_error)
  {
    Serial.printf("Token info: type = %s, status = %s\n", getTokenType(info).c_str(), getTokenStatus(info).c_str());
    Serial.printf("Token error: %s\n\n", getTokenError(info).c_str());
  }
  else
  {
    Serial.printf("Token info: type = %s, status = %s\n\n", getTokenType(info).c_str(), getTokenStatus(info).c_str());
  }

  Serial.println("------------------------------------");
  Serial.println("Set int test...");
  //CUIDADO SI YA EXISTE GENERA FALLAS EN EL SISTEMA
  #endif

  if (Firebase.set(fbdo, path + "/Usuario", auth.token.uid.c_str()))
  {
    rtn = true;
    #ifdef TEST
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    Serial.println("ETag: " + fbdo.ETag());
    Serial.print("VALUE: ");
    printResult(fbdo);
    Serial.println("------------------------------------");
    Serial.println();
    #endif
  }
  #ifdef TEST
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  #endif
  return rtn;
}

int Firebase_Enable()
{
  int rtn = false;
  String enable_path = path + "/Enable";
  if (Firebase.getInt(fbdo, enable_path))
  {
    rtn = fbdo.intData();
  }
  return rtn;
}

bool dataupload()
{
  bool rtn = false;
  struct token_info_t info = Firebase.authTokenInfo();

  if (Firebase.set(fbdo, path + "/int", count++))
  {
    rtn = true;
    #ifdef TEST
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    Serial.println("ETag: " + fbdo.ETag());
    Serial.print("VALUE: ");
    printResult(fbdo);
    Serial.println("------------------------------------");
    Serial.println();
    #endif
  }
  #ifdef TEST
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  #endif
  return rtn;
}

#endif
