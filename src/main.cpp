#include <Arduino.h>

#define TEST

#include <CoopTask.h>
#include <CoopSemaphore.h>
#include <CoopMutex.h>
#include <Coop_Serial_System.h>

/*
#if defined(ARDUINO_AVR_MICRO)
#define STACKSIZE_8BIT 92
#else
#define STACKSIZE_8BIT 40
#endif
*/

#define LED_PERIOD 500
#define FIREB_VAL  1
#define SERIAL_REQ 4
#define OLED_DISP  2
#define FIREB_UP   3
#define ERROR_COM  5

#define USE_BUILTIN_TASK_SCHEDULER

CoopSemaphore taskSema(1, 1);
CoopSemaphore OLEDSema(0, 1);
CoopMutex OLEDMutex;
Coop_System System;

int taskToken     = FIREB_VAL;
bool led          = false;
bool Serial_Event = false;
unsigned long time1,time2,time3,time_rev;

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
        {
            if (WiFi.status() != WL_CONNECTED)
            {
               #ifdef TEST
                Serial.println(F("Firebase Validation -FAILED- Firebase Connection DOWN!"));    
               #endif
               //TRY TO CONNECT TO FIREBASE ONCE MORE 
               taskToken = ERROR_COM;
            }
            else
            {
               #ifdef TEST
                Serial.println(F("Firebase Validation -FAILED- Wi-Fi Connection DOWN!"));    
               #endif
               //RESET SYSTEM
               taskToken = ERROR_COM;
            }
        }
        else
        {
            taskToken = SERIAL_REQ;
        }
        taskSema.post();
        delay(10000);
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
            Serial.println(F("Sepuku"));
        #endif    
        for(;;){ }
    }
}


void SerialEvent()
{
    if (Serial_Event)
    {
        #ifdef TEST
            if (millis() - time1 >= 1000)
            {
                time1 = millis();
                Serial.print(F("."));
            }
            if (Serial.available())
            {
                Serial.print((char)Serial.read());
            }
        #endif
        //En caso de no ser TEST o incluso si es un TEST Contrario almacenar la info para su procesamiento!
        if (millis() - time2 >= 3000)
        {
            taskSema.post();
            Serial_Event = false;
            Serial.println(F("."));
        }
    }
}

CoopTask<void>* task1;
CoopTask<void>* task2;
CoopTask<void>* task3;
CoopTask<void>* task4;
CoopTask<void>* task5;


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
    printStackReport(task5);
    Serial.println(F("---------------------------------"));
};

void setup()
{
    bool Wifi_Conn    = false;
    bool fire_connect = false;
    bool Serial_set   = false;
    char int_c;
    Serial.begin(115200);
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
    while(!Serial_set)
    {
        System.FACP_Setup();
        delay(500);
        if(Serial.available())
        {
            Serial_set = true;
            while(Serial.available())
            {
                int_c=(char)Serial.read();  
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
    
    task1 = new CoopTask<void>(F("1- Firebase Validation"), loop1,0x7D0);
    if (!*task1) {Serial.printf("CoopTask %s out of stack\n", task1->name().c_str());}
    
    task2 = new CoopTask<void>(F("2- OLED Display"),        loop2,0x2EE);
    if (!*task2) {Serial.printf("CoopTask %s out of stack\n", task2->name().c_str());}
    
    task3 = new CoopTask<void>(F("3- Firebase Update"),     loop3,0xDAC);
    if (!*task3) {Serial.printf("CoopTask %s out of stack\n", task3->name().c_str());}
    
    task4 = new CoopTask<void>(F("4- Serial Request"),      loop4,0x4B0);
    if (!*task4) {Serial.printf("CoopTask %s out of stack\n", task4->name().c_str());}
    
    task5 = new CoopTask<void>(F("5- Comm Error Handler"),  loop5,0x3E8);
    if (!*task5) {Serial.printf("CoopTask %s out of stack\n", task5->name().c_str());}
    
    // Add "loop1", "loop2" and "loop3" to CoopTask scheduling.
    // "loop" is always started by default, and is not under the control of CoopTask.
    
    if (!task1->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task1->name().c_str()); }
    
    if (!task2->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task2->name().c_str()); }
    
    if (!task3->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task3->name().c_str()); }
    
    if (!task4->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task4->name().c_str()); }
    
    if (!task5->scheduleTask()) { Serial.printf("Could not schedule task %s\n", task5->name().c_str()); }

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
