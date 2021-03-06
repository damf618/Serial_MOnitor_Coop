#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   13
#define LOGO_WIDTH    17

#define LOGO_HEIGHT2   10
#define LOGO_WIDTH2    37

#define LOGO_HEIGHT3   20
#define LOGO_WIDTH3    20

static const unsigned char PROGMEM logo_isolse_bmp[] =
{ // 'Isolse Logo2', 128x32px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x03, 0x80, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x87, 0xc0, 0x00,
0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xcf, 0xe0, 0x00,
0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xf0, 0x00,
0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xcf, 0xf8, 0x00,
0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x87, 0xfc, 0x00,
0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x03, 0xfe, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x01, 0xff, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0xff, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0,
0x3f, 0xe0, 0x1f, 0x00, 0x1f, 0x80, 0x70, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0x0c, 0x01, 0xff, 0xe0,
0x3f, 0xe0, 0xff, 0xc0, 0x7f, 0xe0, 0x70, 0x01, 0xff, 0x01, 0xff, 0x00, 0x1e, 0x03, 0xff, 0xf0,
0x3f, 0xe1, 0xff, 0xe0, 0xff, 0xf0, 0x70, 0x03, 0xff, 0x83, 0xff, 0x80, 0x3f, 0x07, 0xff, 0xf8,
0x01, 0xe1, 0xc0, 0xe1, 0xe0, 0x78, 0x70, 0x07, 0x83, 0xc7, 0x83, 0xc0, 0x7f, 0x8f, 0xff, 0xfc,
0x01, 0xe1, 0xc0, 0xe1, 0xc0, 0x3c, 0x70, 0x07, 0x01, 0xcf, 0x01, 0xe0, 0x3f, 0xdf, 0xff, 0xf8,
0x01, 0xe1, 0xc0, 0x03, 0xc0, 0x3c, 0x70, 0x07, 0x00, 0x0e, 0x00, 0xe0, 0x1f, 0xff, 0xff, 0xf0,
0x01, 0xe1, 0xe0, 0x03, 0x80, 0x1c, 0x70, 0x07, 0x80, 0x0e, 0x00, 0xe0, 0x0f, 0xff, 0xff, 0xe0,
0x01, 0xe0, 0xfe, 0x03, 0x80, 0x1e, 0x70, 0x03, 0xf8, 0x0f, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xc0,
0x01, 0xe0, 0x7f, 0xc3, 0x80, 0x1e, 0x70, 0x01, 0xff, 0x1f, 0xff, 0xe0, 0x03, 0xff, 0xff, 0x80,
0x01, 0xe0, 0x1f, 0xe3, 0x80, 0x1e, 0x70, 0x00, 0x7f, 0x9e, 0x00, 0x00, 0x01, 0xff, 0xff, 0x00,
0x01, 0xe0, 0x01, 0xf3, 0x80, 0x1c, 0x70, 0x00, 0x07, 0xce, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00,
0x01, 0xe0, 0x00, 0x73, 0xc0, 0x3c, 0x70, 0x00, 0x01, 0xce, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x00,
0x01, 0xe0, 0x00, 0x71, 0xc0, 0x3c, 0x78, 0x00, 0x01, 0xcf, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x00,
0x01, 0xe3, 0xc0, 0xf1, 0xe0, 0xf8, 0x3c, 0x07, 0x01, 0xc7, 0x81, 0xc0, 0x00, 0x1f, 0xf0, 0x00,
0x01, 0xe1, 0xff, 0xe0, 0xff, 0xf0, 0x3f, 0xc7, 0xff, 0xc7, 0xef, 0xc0, 0x00, 0x0f, 0xe0, 0x00,
0x01, 0xe1, 0xff, 0xe0, 0x7f, 0xe0, 0x1f, 0xc3, 0xff, 0x83, 0xff, 0x80, 0x00, 0x07, 0xc0, 0x00,
0x01, 0xe0, 0x7f, 0x80, 0x1f, 0x80, 0x07, 0xc1, 0xfe, 0x00, 0xff, 0x00, 0x00, 0x03, 0x80, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static const unsigned char PROGMEM logo_bmp[] =
{ 0x03, 0xe0, 0x00, 0x1f, 0xfc, 0x00, 0x7c, 0x1f, 0x00, 0xf0, 0x07, 0x80, 0x47, 0xf1, 0x00, 0x1f,
0x7c, 0x00, 0x38, 0x0c, 0x00, 0x13, 0xe4, 0x00, 0x07, 0xf0, 0x00, 0x06, 0x30, 0x00, 0x00, 0x80,
0x00, 0x01, 0xc0, 0x00, 0x00, 0x80, 0x00  };

static const unsigned char PROGMEM logo1_bmp[] =
{ // '3', 17x13px
0x00, 0x18, 0x00, 0x0f, 0xd8, 0x00, 0x38, 0x1a, 0x00, 0x67, 0xdb, 0x00, 0xde, 0x59, 0x80, 0x39,
0xde, 0x00, 0x27, 0xda, 0x00, 0x0c, 0x18, 0x00, 0x03, 0xd8, 0x00, 0x06, 0x78, 0x00, 0x00, 0x98,
0x00, 0x01, 0xc0, 0x00, 0x00, 0x90, 0x00  };

static const unsigned char PROGMEM logo2_bmp[] =
{ // '3', 17x13px
0x00, 0x00, 0x00, 0x00, 0x00, 0xfd, 0xef, 0xb0, 0x31, 0xf0, 0xfd, 0xef, 0xb0, 0x31, 0xf0, 0x30,
0x0c, 0x30, 0x79, 0xb8, 0x33, 0xef, 0xb0, 0x79, 0xf0, 0x33, 0xef, 0xb0, 0x79, 0xf0, 0x33, 0x81,
0xb0, 0xfd, 0xb8, 0x31, 0xef, 0xbe, 0xfd, 0xf8, 0x31, 0xef, 0x9e, 0xcd, 0xf0, 0x00, 0x00, 0x00,
0x00, 0x00};

static const unsigned char PROGMEM logo3_bmp[] =
{
// 'cc', 20x20px
0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x0e, 0x07, 0x00, 0x18, 0x01, 0x80, 0x30, 0xf0, 0xc0, 0x33,
0xfc, 0x40, 0x66, 0x06, 0x60, 0x44, 0x62, 0x20, 0x4c, 0xf3, 0x20, 0x4d, 0xff, 0xe0, 0x4d, 0xff,
0xe0, 0x4c, 0xf3, 0x20, 0x44, 0x22, 0x20, 0x66, 0x06, 0x60, 0x33, 0xfc, 0x40, 0x30, 0xf0, 0xc0,
0x1c, 0x01, 0x80, 0x0f, 0x07, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00};


#define OFFTIME 650
#define ONTIME  1050

typedef enum
{
  NORMAL,
  FAILURE,
  ALARM,
  BOTH
} Central_State ;

class Screen_Format
{
  private:
  int Msg_pos;
  int Msg_len;
  unsigned long blinky_time;
  bool WiFi_Conn;
  Central_State CAI;
  char message[500];
  public:

  void Screen_Init()
  {
    Msg_pos= display.width();
    Msg_len=0;
    blinky_time = millis();
    WiFi_Conn =false;
    CAI = NORMAL;
    strcpy(message,"");
  }

  void Screen_Refresh(bool WiFi_Con, Central_State CAII, char * messagee )
  {
    Msg_len =  -6*strlen(message);
    WiFi_Conn = WiFi_Con;
    CAI = CAII;
    strcpy(message,messagee);
  }

  void Screen_Set()
  {
    display.clearDisplay();
    if(WiFi_Conn)
    {
      display.drawBitmap(display.width()-LOGO_WIDTH,0,logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);    
    }
    else
    {
      if(millis()-blinky_time>=OFFTIME)
      {
        display.drawBitmap(display.width()-LOGO_WIDTH,0,logo1_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);  
        if(millis()-blinky_time>=ONTIME)
        {
          blinky_time=millis();
        }
      }
    }
    display.setTextSize(1); // Draw 2X-scale text
    display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
    display.setCursor(0,14);
    display.print(F("   Central: "));

    switch(CAI)
    {
      case NORMAL:
        display.println(F("Normal   "));
        break;
      case FAIL:
        display.println(F("Falla    "));
        break;
      case ALARM:
        display.println(F("Alarma   "));
        break;
      case BOTH:
        display.println(F("Ala / Fal"));
        break;
      default :  
        display.println(F("---------"));
    }
    
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(Msg_pos,24);
    display.print(message);
    display.drawBitmap(0,0,logo2_bmp, LOGO_WIDTH2, LOGO_HEIGHT2, 1);
    //Msg_pos-=1;
    Msg_pos-=2;
    if(Msg_pos<Msg_len)
    {
      Msg_pos=display.width();
    }
    display.display();
  }

};

Screen_Format OLED_Display;

unsigned long timey = 0;

void OLED_Start()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.drawBitmap(0,0,logo_isolse_bmp, 128, 32, 1);
  display.display();
  delay(4000); // Pause for 2 seconds
  
  OLED_Display.Screen_Init();
}


void testdrawbitmap() {

  bool Connectivity;
  Central_State CAI_S;
  char message[300]="";
  char messagea[100]="";
  char messageb[100]="";

  if(millis()-timey>=30000){
      timey=millis();
    }
 
  if((millis()-timey<=10000)||(millis()-timey>=22000))
  {
      Connectivity = true;
      strcpy(messagea,"Prueba de Conectividad Firebase OK");
  }
  else
  {
      Connectivity = false;
      strcpy(messagea,"Prueba de Conectividad Firebase ERROR");
  }
  if(millis()-timey<=9000)
  {
    CAI_S= ALARM;
    strcpy(messageb," - Prueba de Central en estado de Alarma");
  }
  else if(millis()-timey<=19000)
  {
    CAI_S= FAILURE;
    strcpy(messageb," - Prueba de Central en estado de Falla");
  }
  else
  {
    CAI_S= NORMAL;
    strcpy(messageb," - Prueba de Central en estado Normal");
  }
  strcat(message,messagea);
  strcat(message,messageb);

  OLED_Display.Screen_Refresh(Connectivity, CAI_S,message);
 
}