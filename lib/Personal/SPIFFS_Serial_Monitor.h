#ifndef SPIFFS_Serial_h
#define SPIFFS_Serial_h

#include <FS.h>

bool SPIFFS_Start()
{
  bool rtn = true;
  
  if(!SPIFFS.begin())
  {
    #ifdef TEST
    Serial.println(F("Error al inciar SPIFFS"));
    #endif
    rtn = false;
  }
  return rtn; 
}

String readFile(fs::FS &fs, const char * path){
  #ifdef TEST
  Serial.printf("Reading file: %s\r\n", path);
  #endif
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    #ifdef TEST
    Serial.println("- empty file or failed to open file");
    #endif
    return String();
  }
  #ifdef TEST
  Serial.println("- read from file:");
  #endif
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  #ifdef TEST
  Serial.println(fileContent);
  #endif
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  #ifdef TEST
  Serial.printf("Writing file: %s\r\n", path);
  #endif
  File file = fs.open(path, "w");
  if(!file){
    #ifdef TEST
    Serial.println("- failed to open file for writing");
    #endif
    return;
  }
  #ifdef TEST
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  #else
  file.print(message);
  #endif
}
#endif
