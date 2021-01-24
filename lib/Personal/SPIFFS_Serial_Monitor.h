#ifndef SPIFFS_Serial_h
#define SPIFFS_Serial_h

#include <FS.h>

bool SPIFFS_Start()
{
  bool rtn = true;
  if(!SPIFFS.begin())
  {
    Serial.println(F("Error al inciar SPIFFS"));
    rtn = false;
  }
  return rtn; 
}

String readFile(fs::FS &fs, const char * path){
  #ifdef DEBUG
  Serial.printf("Reading file: %s\r\n", path);
  #endif
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  #ifdef DEBUG
  Serial.println("- read from file:");
  #endif
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  #ifdef DEBUG
  Serial.println(fileContent);
  #endif
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  #ifdef DEBUG
  Serial.printf("Writing file: %s\r\n", path);
  #endif
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    #ifdef DEBUG
    Serial.println("- file written");
    #endif
  } else {
    Serial.println("- write failed");
  }
}

#endif
