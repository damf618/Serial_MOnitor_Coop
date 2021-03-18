#include <Arduino.h>

#define TEST

#include <Display_OLED.h>
#include <CoopTask.h>
#include <CoopSemaphore.h>
#include <CoopMutex.h>
#include <Coop_Serial_System.h>


#if defined(ARDUINO_AVR_MICRO)
#define STACKSIZE_8BIT 92
#else
#define STACKSIZE_8BIT 40
#endif


#define LED_PERIOD 500
#define FIREB_VAL  1
#define SERIAL_REQ 4
#define OLED_DISP  2
#define FIREB_UP   3
#define SERIAL_TIME 7500
//#define SERIAL_TIME 100

#define MSG_CLEAN      ""
#define MSG_INIT       "Sistema en Arrranque"
#define MSG_WIFI_INIT  "Intentando conexion WiFi"
#define MSG_WIFI_OK    "Conexion WiFi Correcta"
#define MSG_WIFI_FAIL  "Conexion WiFi Erronea"
#define MSG_WIFI_AP    "Por favor ingresar a la red WiFi: "
#define MSG_ALARM      "Consulta de Listado de Alarmas"
#define MSG_FAILS      "Consulta de Listado de Fallas"
#define FBASE_TRIES    50
//#define LOOP1_DELAY    24UL*60UL*60UL*1000UL
#define LOOP1_DELAY    3000
//#define LOOP4_DELAY    30
#define LOOP4_DELAY    3000

#define USE_BUILTIN_TASK_SCHEDULER

CoopSemaphore taskSema(1, 1);
CoopSemaphore valSema (1, 1);
//CoopSemaphore OLEDSema(1, 1);
//CoopMutex OLEDMutex;
Coop_System System;

int taskToken       = FIREB_VAL;
bool led            = false;
bool Serial_F_Event = false;
bool Serial_A_Event = false;
bool WiFi_Con       = false;
bool msg            = false;
Central_State CAII  = NORMAL;
char messageb[50]  = "";
int firebase_tries = 0;


unsigned long time1,time2,time3,time_rev;
String inputString;

void Blinky_Blinky()
{
    if (millis() - time_rev >= LED_PERIOD)
    {
        time_rev = millis();
        if (!led)
        {
            digitalWrite(LED_BUILTIN, HIGH);
        }
        else
        {
            digitalWrite(LED_BUILTIN, LOW);
        }
        led = !led;
    }
}

// Task no.1: blink LED with 1 second delay.
void loop1()
{
    
    for (;;) // explicitly run forever without returning
    {
        
        valSema.wait();        
        
        #ifdef TEST
            Serial.println(F("Firebase Val"));
            delay(500);
        #endif

        if (!System.Firebase_enable())
        //if (1!=1)
        {
            WiFi_Con = false;

            if (WiFi.status() == WL_CONNECTED)
            {
               #ifdef TEST
                Serial.println(F("Firebase Connection -FAILED-"));    
               #endif
               //ESP.reset();
            }
            else
            {                
               #ifdef TEST
                Serial.println(F("Wi-Fi Connection DOWN!"));    
               #endif
               //ESP.reset();
            } 
        }
        else
        {
            WiFi_Con = true;
            taskToken = SERIAL_REQ;
            //taskSema.post();
        }
        #ifdef TEST
            Serial.print(F("El estado de WiFi_Con es: "));
            Serial.println(WiFi_Con);
        #endif
        delay(LOOP1_DELAY);
    }
}

// Task no.2: blink LED with 0.25 second delay.
void loop2()
{
    for (;;) // explicitly run forever without returning
    {
        /*
        #ifdef TEST
            Serial.println(F("OLED Disp"));
        #endif
        */

        //OLEDSema.wait();
        //{
            //CoopMutexLock serialLock(OLEDMutex);
            System.Print_OLED(WiFi_Con,CAII,messageb);
        //}
        //OLEDSema.post();
        delay(25);
        //yield();
    }
}

// Task no.3: blink LED with 0.05 second delay.
void loop3()
{
    for (;;) // explicitly run forever without returning
    {
        taskSema.wait();
        if (FIREB_UP != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }
        #ifdef TEST
            Serial.print(F("Task 3 In\n"));
        #endif

        #ifdef TEST
            if (!System.Firebase_upload(0))
            {
                Serial.println(F("Firebase Upload -FAILED- Server not reacheable"));
                WiFi_Con = 0;
            }
        #else
            System.Firebase_upload();
        #endif

        #ifdef TEST
            Serial.print(F("Task 3 Out\n"));
        #endif

        taskSema.post();
        //taskToken = FIREB_VAL;
        taskToken = SERIAL_REQ;
        yield();
    }
}

void loop4()
{
    for (;;) // explicitly run forever without returning
    {
        taskSema.wait();
        if (SERIAL_REQ != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }
        
        
        #ifdef TEST
            Serial.print(F("- Fails -\n"));
        #endif

        //*-**-*-*-
        memcpy(messageb,MSG_CLEAN,strlen(MSG_CLEAN)+1);
        strcpy(messageb,MSG_FAILS);

        System.get_Fails();
        Serial_F_Event=1;
        time2 = millis();
        yield();
        
        
        while(Serial_F_Event)
        {
            taskSema.wait();
            if (SERIAL_REQ != taskToken)
            {
                taskSema.post();
                yield();
                continue;
            }
            else
            {
                break;    
            }
        }
        
        //UPLOAD IN OLED_DISPLAY QUEUE
        /*
        {
            CoopMutexLock serialLock(OLEDMutex);
            System.OLED_Events();
        }
        */

        #ifdef TEST
            Serial.print(F("- Alarms -\n"));
        #endif

        //*-**-*-*-
        memcpy(messageb,MSG_CLEAN,strlen(MSG_CLEAN)+1);
        strcpy(messageb,MSG_ALARM);

        System.get_Alarms();
        Serial_A_Event=1;
        time2 = millis();
        yield();
        
        
        while(Serial_A_Event)
        {
            taskSema.wait();
            if (SERIAL_REQ != taskToken)
            {
                taskSema.post();
                yield();
                continue;
            }
            else
            {
                break;    
            }
        }
        

        //UPLOAD IN OLED_DISPLAY QUEUE
        /*
        {
            CoopMutexLock serialLock(OLEDMutex);
            System.OLED_Events();
        }
        */
       //*-**-*-*-
        memcpy(messageb,MSG_CLEAN,strlen(MSG_CLEAN)+1);
        //taskToken = FIREB_UP;
        //taskToken = FIREB_VAL;
        delay(LOOP4_DELAY);
        taskSema.post();
        yield();
    }
}

void SerialEvent()
{
    char inChar;
    bool mode=false;

    if ((1==Serial_F_Event)||(1==Serial_A_Event))
    {
        #ifdef TEST
            if (millis() - time1 >= 1000)
            {
                time1 = millis();
                Serial.print(F("."));
            }
        #endif
            if (Serial.available())
            {
                inChar = (char)Serial.read();
                #ifdef TEST
                    Serial.print(inChar);
                #endif
                inputString += inChar;

                if (((inChar == '\n') || (inputString.length() >= 100))and(count_index<MAXMSGS))
                {
                    #ifdef TEST
                        Serial.print(F("The message detected is: "));
                        Serial.println(inputString.c_str());
                    #endif
                    System.Serial_Msg_Upload(inputString);
                    msg = true;
                    inputString = "";
                }
            }
        //En caso de no ser TEST o incluso si es un TEST Contrario almacenar la info para su procesamiento!
        if (millis() - time2 >= SERIAL_TIME)
        {
            taskSema.post();
            //taskToken = SERIAL_REQ;

            if(Serial_F_Event)
            {
                System.Trouble_Protocol();
                Serial_F_Event = false;
                #ifdef TEST
                    Serial.println(F("Out of Troubles"));
                #endif
                mode = false;
                
                if(msg)
                {
                    CAII  = FAILURE;
                }
                else
                {
                    CAII  = NORMAL;
                }
                msg = false;
            }
            else if(Serial_A_Event)
            {
                System.Fire_Protocol();
                Serial_A_Event = false;
                #ifdef TEST
                    Serial.println(F("Out of Fire"));
                #endif
                mode = true;

                if((msg)&&(FAILURE == CAII))
                {
                    CAII  = BOTH;
                }
                else if(msg)
                {
                    CAII  = ALARM;
                }
                msg = false;
            }

            #ifdef TEST
            Serial.println(F(".\n No more Serial listening"));
            #endif

            //AQUI!!!!!
            #ifdef TEST
                Serial.print(F("The mode selected is: "));
                if(!mode)
                {
                    Serial.println(F("False or Trouble"));
                }
                else
                {
                    Serial.println(F("True or Alarm"));
                    Serial.println(F("True or Alarm"));
                    if(!System.Firebase_upload(mode))
                    {
                        WiFi_Con = 0;
                    }
                    Serial.println(F("Download Complete"));
                }
                /*
                if (!System.Firebase_upload(mode))
                {
                    Serial.println(F("Firebase Upload -FAILED- Server not reacheable"));
                }
                */
            #else
                System.Firebase_upload(mode);
            #endif
            //AQUI!!

        }
    }
    else
    {
        Serial.read();
    }

}

void Fix_Firebase()
{
    bool  rtn =false;

    if(!WiFi_Con)
    {
        #ifdef TEST
            Serial.println(F("Reparacion de Conexion de Firebase "));
        #endif
        rtn = System.WiFi_Val();

            if(rtn)
            {
                #ifdef TEST
                    Serial.println(F("Restaurado correctamente"));
                #endif
                WiFi_Con = 1;
                //taskToken = FIREB_VAL;
                valSema.post();
                firebase_tries = 0;
            }
            else
            {
                #ifdef TEST
                    Serial.println(F("Restauracion fallida"));
                #endif
                firebase_tries++;
            }
        if(FBASE_TRIES<=firebase_tries)
        {
            #ifdef TEST
                Serial.println(F("Maximo numero de Intentos de reparacion"));
            #endif
            ESP.reset();
        }
        
    }
}

CoopTask<void>* task1;
CoopTask<void>* task2;
CoopTask<void>* task3;
CoopTask<void>* task4;

void printStackReport(CoopTaskBase* task)
{
    if (!task) return;
    Serial.print(task->name().c_str());
    Serial.print(F(" free stack = "));
    Serial.println(task->getFreeStack());
}

void printReport()
{
    Serial.println(F("---------------------------------"));
    printStackReport(task1);
    printStackReport(task2);
    printStackReport(task3);
    printStackReport(task4);
    Serial.print(F("ESP mem: "));
    Serial.println(ESP.getFreeHeap());
    Serial.println(F("---------------------------------"));
};

void setup()
{
    bool Wifi_Conn    = false;
    bool fire_connect = false;
    bool Serial_set   = false;
    unsigned long timing = 0;

    Serial.begin(115200);   
    inputString.reserve(MAXMSGLENGTH);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    //Serial.swap(); NUevos pines y nos libramos del SerialDEBUG
    
    /*
    Serial utiliza UART0, el cual está mapeado a los pines GPIO1 (TX) y GPIO3 (RX).
    El Serial puede ser redirigido a GPIO15 (TX) y GPIO13 (RX) llamando a Serial.swap()
    después de Serial.begin.
    */

    // Setup the 3 pins as OUTPUT
    pinMode(LED_BUILTIN, OUTPUT);
    #ifdef TEST
        Serial.println(F("Wi-Fi Connection"));
        delay(500);
    #endif
    if (!System.Start())
    {
    //LOOP1 INIT AND SETUP BLOCK
        while(!Wifi_Conn)
        {
            if (System.Wi_Fi_Status())
            {
                if (System.Wi_Fi_Connection())
                {
                    Wifi_Conn = true;
                    WiFi_Con  = true;
                }
            }
            else
            {
                //SHOULDN'T BE ANY DELAY IN THIS FUNCTION 
                System.WiFi_Attention_OLED();
            }
        }
    }
    else
    {
        WiFi_Con  = true;
    }
    
    fire_connect=false;
    /*LOOP3 INIT FIREBASE CONNECTION */
    //TODO VALIDATION
    #ifdef TEST
        Serial.println(F("Firebase Connection"));
        delay(500);
    #endif
    while(!fire_connect)
    {
        if(System.Firebase_Set_up())
        {
            fire_connect=true;
        }
    }
    /*LOOP4 SERIAL INIT SETUP*/
    //TODO VALIDATION
    #ifdef TEST
        Serial.println(F("Serial Connection"));
        delay(500);
    #endif
    System.Serial_Config();

    while(!Serial_set)
    {
        System.FACP_Setup();
        timing=millis();
        while((!Serial.available())&&(millis()-timing<=3000))
        {
            #ifdef TEST
                Serial.print(F("."));
            #endif
        }
        if(Serial.available())
        {
            #ifdef TEST
                Serial.print(F("Conexion establecida"));
            #endif
            Serial_set = true;
            while(Serial.available())
            {
                Serial.read();  
            }
        }
        else
        {
            #ifdef TEST
                Serial.println(F("Connection to FACP is impossible"));
            #endif
            delay(5000);
        }
    } 
    #ifdef TEST
        Serial.println(F("Sistema Inicializado Correctamente"));
    #endif

    #ifdef USE_BUILTIN_TASK_SCHEDULER
        CoopTaskBase::useBuiltinScheduler();
    #endif
    
    task1 = new CoopTask<void>(F("1- Firebase Validation"), loop1,0x898); //E10, C80 
    if (!*task1) {Serial.printf("CoopTask %s out of stack\n", task1->name().c_str());}
    
    task2 = new CoopTask<void>(F("2- OLED Display"),        loop2,0x5DC);//3E8
    if (!*task2) {Serial.printf("CoopTask %s out of stack\n", task2->name().c_str());}
    //DAC
    //task3 = new CoopTask<void>(F("3- Firebase Update"),     loop3,0xF0A);//9FC, CE4,  
    //if (!*task3) {Serial.printf("CoopTask %s out of stack\n", task3->name().c_str());}
    
    task4 = new CoopTask<void>(F("4- Serial Request"),      loop4,0x4B0);//320
    if (!*task4) {Serial.printf("CoopTask %s out of stack\n", task4->name().c_str());}
    
    // Add "loop1", "loop2" and "loop3" to CoopTask scheduling.
    // "loop" is always started by default, and is not under the control of CoopTask.
    
    if (!task1->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task1->name().c_str()); }
    
    if (!task2->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task2->name().c_str()); }
    
    //if (!task3->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task3->name().c_str()); }
    
    if (!task4->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task4->name().c_str()); }

    time1    = millis();
    time3    = time1;
    time_rev = time1;

    #ifdef TEST
        Serial.print(F("El estado de WiFi_Con es: "));
        Serial.println(WiFi_Con);
    #endif
    
}

void loop()
{
    // loops forever by default
    runCoopTasks();
    Blinky_Blinky();
    SerialEvent();
    Fix_Firebase();
    
    #ifdef TEST
        if(millis()-time3>=10000){
            time3 = millis();
            printReport();
        }
    #endif
    
}
