#include <Arduino.h>

#define TEST

// Include CoopTask since we want to manage multiple tasks.
#include <CoopTask.h>
#include <CoopSemaphore.h>
#include <CoopMutex.h>
#include <Coop_Serial_System.h>

#if defined(ARDUINO_AVR_MICRO)
#define STACKSIZE_8BIT 92
#else
#define STACKSIZE_8BIT 40
#endif

CoopSemaphore taskSema(1, 1);
CoopSemaphore FireSema(0, 1);
CoopSemaphore OLEDSema(0, 1);
CoopMutex OLEDMutex;

int taskToken = 1;

Coop_System System;
unsigned long time_rev = 0;
bool led = false;
bool Serial_Event=false;
unsigned long time1;

void Blinky_Blinky()
{
    if (millis() - time_rev >= 500)
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
        if (1 != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }

        if (System.Wi_Fi_Status())
        {
            if (System.Wi_Fi_Connection())
            {
                taskToken = 3;
            }
        }
        else
        {
            System.WiFi_Attention_OLED();
        }
        taskSema.post();
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
        yield();
        //delay(1000);
        //taskToken = 3;
        //taskSema.post();
    }
}

// Task no.3: blink LED with 0.05 second delay.
void loop3()
{
    bool fire_connect = false;

    while(!fire_connect)
    {
        taskSema.wait();
        if (3 != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }
        if(System.Firebase_Set_up())
        {
            fire_connect=true;
            taskSema.post();
        }
    }

    for (;;) // explicitly run forever without returning
    {
        taskSema.wait();
        if (3 != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }

        if (fire_connect)
        {
            if (System.Firebase_enable())
            {
#ifdef TEST
                if (!System.Firebase_upload())
                {
                    Serial.println("Server not reacheable");
                }
#else
                System.Firebase_upload();
#endif
            }
            else
            {
                #ifdef TEST
                Serial.println("Server not enabled");
                #endif
                fire_connect=false;
            }

            OLEDSema.post();
            FireSema.post();
            //delay(10000);
        }
        else
        {
            if (System.Firebase_Set_System_up())
            {
                fire_connect = true;
            }
            else
            {
#ifdef TEST
                Serial.println("Error en Firebase");
                if(WiFi.status() != WL_CONNECTED)
                {
                    Serial.println("No hay conexion");
                }
#endif
                delay(3000);
            }
            taskSema.post();
        }
        //taskSema.post();
        yield();
    }
}

void loop4()
{
    FireSema.wait();
    System.FACP_Setup();
    FireSema.post();
    for (;;) // explicitly run forever without returning
    {
        FireSema.wait();
#ifdef TEST
        Serial.print("- Fails -\n");
#endif
        System.get_Fails();
        Serial_Event=true;
        delay(2000);
        Serial_Event=false;
        /*
        {
            CoopMutexLock serialLock(OLEDMutex);
            System.OLED_Events();
        }
        */

#ifdef TEST
        Serial.print("- Alarms -\n");
#endif
        System.get_Alarms();
        Serial_Event=true;
        delay(3000);
        Serial_Event=false;
        /*
        {
            CoopMutexLock serialLock(OLEDMutex);
            System.OLED_Events();
        }
        */
        taskSema.post();
        yield();
    }
}

void SerialEvent()
{
    if (Serial_Event)
    {
        if (millis() - time1 >= 1000)
        {
            time1 = millis();
            Serial.print(".");
        }
    if (Serial.available())
    {
        Serial.print((char)Serial.read());
    }
    
    
    }
}

BasicCoopTask<CoopTaskStackAllocatorAsMember<sizeof(unsigned) >= 4 ? 2400 : STACKSIZE_8BIT>> task4("l4", loop4);
BasicCoopTask<CoopTaskStackAllocatorAsMember<sizeof(unsigned) >= 4 ? 1600 : STACKSIZE_8BIT>> task1("l1", loop1);
BasicCoopTask<CoopTaskStackAllocatorAsMember<sizeof(unsigned) >= 4 ? 2000 : STACKSIZE_8BIT>> task2("l2", loop2);
BasicCoopTask<CoopTaskStackAllocatorAsMember<sizeof(unsigned) >= 4 ? 6000 : STACKSIZE_8BIT>> task3("l3", loop3);
//2000
//1200
//1200
//4000
//BasicCoopTask<CoopTaskStackAllocatorFromLoop<sizeof(unsigned) >= 4 ? 4000 : STACKSIZE_8BIT>> task3("l3", loop3, sizeof(unsigned) >= 4 ? 4000 : STACKSIZE_8BIT);

//INtenta ejecutar el Firebase Negin en el ciclo principal de esa forma no se requiere memoria de stack para ello.

void setup()
{
    Serial.begin(115200);
    //Serial.swap(); NUevos pines y nos libramos del SerialDEBUG
    /*
Serial utiliza UART0, el cual está mapeado a los pines GPIO1 (TX) y GPIO3 (RX).
El Serial puede ser redirigido a GPIO15 (TX) y GPIO13 (RX) llamando a Serial.swap()
después de Serial.begin.
*/

    // Setup the 3 pins as OUTPUT
    pinMode(LED_BUILTIN, OUTPUT);

    if (System.Start())
    {
        taskToken = 3;
    }
#ifdef TEST
    Serial.println(F("Sistema Inicializado Correctamente"));
    Serial.print(F("TaskToken: "));
    Serial.println(taskToken);
#endif
    // Add "loop1", "loop2" and "loop3" to CoopTask scheduling.
    // "loop" is always started by default, and is not under the control of CoopTask.
    task1.scheduleTask();
    task2.scheduleTask();
    task3.scheduleTask();
    task4.scheduleTask();
    time1 = millis();
}

void loop()
{
    // loops forever by default
    runCoopTasks();
    Blinky_Blinky();
    SerialEvent();
    /*
    if (5 == taskToken)
    {
#ifdef TEST
        Serial.println("Firebase");
#endif
        if (System.Firebase_Set_up())
        {
            taskToken = 3;
        }
#ifdef TEST
        else
        {
            Serial.println("Error en Firebase");
        }
#endif

    }
    */
}