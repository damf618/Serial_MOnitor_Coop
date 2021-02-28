#include <Arduino.h>

#define TEST

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
//#define ERROR_COM  5
#define SERIAL_TIME 1000

#define USE_BUILTIN_TASK_SCHEDULER

CoopSemaphore taskSema(1, 1);
CoopSemaphore OLEDSema(0, 1);
CoopMutex OLEDMutex;
Coop_System System;

int taskToken     = FIREB_VAL;
bool led          = false;
bool Serial_Event = false;
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
        taskSema.wait();
        if (FIREB_VAL!= taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }
        
        if (!System.Firebase_enable())
        //if (1!=1)
        {
            if (WiFi.status() == WL_CONNECTED)
            {
               #ifdef TEST
                Serial.println(F("Firebase Connection -FAILED-"));    
               #endif
               //TRY TO CONNECT TO FIREBASE ONCE MORE 
               //taskToken = ERROR_COM;
               /*
               if(!System.WiFi_Val())
               {
                    ESP.reset();
               }
               */
              ESP.reset();
            }
            else
            {                
               #ifdef TEST
                Serial.println(F("Wi-Fi Connection DOWN!"));    
               #endif
               //RESET SYSTEM
               //taskToken = ERROR_COM;
               ESP.reset();
            }
        }
        else
        {
            taskToken = SERIAL_REQ;
        }
        taskSema.post();
        //delay(5000);
    }
}

// Task no.2: blink LED with 0.25 second delay.
void loop2()
{
    for (;;) // explicitly run forever without returning
    {
        OLEDSema.wait();
        {
            CoopMutexLock serialLock(OLEDMutex);
            System.Print_OLED();
        }
        //delay(1000);
        yield();
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
            if (!System.Firebase_upload())
            {
                Serial.println(F("Firebase Upload -FAILED- Server not reacheable"));
            }
        #else
            System.Firebase_upload();
        #endif
        taskSema.post();
        taskToken = FIREB_VAL;
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
        System.get_Fails();
        Serial_Event=true;
        time2 = millis();
        yield();
        
        while(Serial_Event)
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
        System.get_Alarms();
        Serial_Event=true;
        time2 = millis();
        yield();
        
        while(Serial_Event)
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
        
        taskSema.post();
        taskToken = FIREB_UP;
        yield();
        }
}
/*
// Task no.3: blink LED with 0.05 second delay.
void loop5()
{   
    for (;;) // explicitly run forever without returning
    {
        taskSema.wait();
        if (ERROR_COM != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }
        #ifdef TEST
            Serial.println(F("Connection to Firebase is impossible"));
        #endif    
        
        if (System.Wi_Fi_Connection())
        {
            taskToken =FIREB_VAL;
            taskSema.post();
            yield();
        }
        else
        {
            ESP.reset();
        }
        Serial.println(F("Sepuku"));
        ESP.reset();
       
    }
}
*/

void SerialEvent()
{
    char inChar;
    bool Sevent = false;

    if (Serial_Event)
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
                        Serial.print("The message detected is: ");
                        Serial.println(inputString.c_str());
                    #endif
                    System.Serial_Msg_Upload(inputString);
                    inputString = "";
                }
            }
        //En caso de no ser TEST o incluso si es un TEST Contrario almacenar la info para su procesamiento!
        if (millis() - time2 >= SERIAL_TIME)
        {
            taskSema.post();
            Serial_Event = false;
            #ifdef TEST
            Serial.println(F(".\n No more Serial listening"));
            #endif
            Sevent = true;
        }
    }
    else
    {
        Serial.read();
    }
    
    if(Sevent)
    {
        System.Print_Serial_Msg();
    }
    
}

CoopTask<void>* task1;
CoopTask<void>* task2;
CoopTask<void>* task3;
CoopTask<void>* task4;
//CoopTask<void>* task5;


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
    //printStackReport(task5);
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
    /*LOOP1 INIT AND SETUP BLOCK*/
        while(!Wifi_Conn)
        {
            if (System.Wi_Fi_Status())
            {
                if (System.Wi_Fi_Connection())
                {
                    Wifi_Conn = true;
                }
            }
            else
            {
                //SHOULDN'T BE ANY DELAY IN THIS FUNCTION 
                System.WiFi_Attention_OLED();
            }
        }
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
        //delay(10);
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
                Serial.println(F("Seppuku"));
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
    
    task1 = new CoopTask<void>(F("1- Firebase Validation"), loop1,0xE10); //7D0, 9c4,960, E10 
    if (!*task1) {Serial.printf("CoopTask %s out of stack\n", task1->name().c_str());}
    
    task2 = new CoopTask<void>(F("2- OLED Display"),        loop2,0x258);//2EE
    if (!*task2) {Serial.printf("CoopTask %s out of stack\n", task2->name().c_str());}
    //DAC
    task3 = new CoopTask<void>(F("3- Firebase Update"),     loop3,0x9C4);//9C4, e10, 7D0
    if (!*task3) {Serial.printf("CoopTask %s out of stack\n", task3->name().c_str());}
    
    task4 = new CoopTask<void>(F("4- Serial Request"),      loop4,0x3E8);//4B0 , 320
    if (!*task4) {Serial.printf("CoopTask %s out of stack\n", task4->name().c_str());}
    //3e8
    //task5 = new CoopTask<void>(F("5- Comm Error Handler"),  loop5,0x320);//
    //if (!*task5) {Serial.printf("CoopTask %s out of stack\n", task5->name().c_str());}
    
    // Add "loop1", "loop2" and "loop3" to CoopTask scheduling.
    // "loop" is always started by default, and is not under the control of CoopTask.
    
    if (!task1->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task1->name().c_str()); }
    
    if (!task2->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task2->name().c_str()); }
    
    if (!task3->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task3->name().c_str()); }
    
    if (!task4->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task4->name().c_str()); }
    
    //if (!task5->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task5->name().c_str()); }

    time1    = millis();
    time3    = time1;
    time_rev = time1;
    OLEDSema.post();
}

void loop()
{
    // loops forever by default
    runCoopTasks();
    Blinky_Blinky();
    SerialEvent();
    #ifdef TEST
        if(millis()-time3>=10000){
            time3 = millis();
            printReport();
        }
    #endif
    
}
