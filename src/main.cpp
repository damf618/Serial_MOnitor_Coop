#include <Arduino.h>

// Include CoopTask since we want to manage multiple tasks.
#include <CoopTask.h>
#include <CoopSemaphore.h>
#include <Coop_Serial_System.h>

#define TEST

#if defined(ARDUINO_AVR_MICRO)
#define STACKSIZE_8BIT 92
#else
#define STACKSIZE_8BIT 40
#endif

CoopSemaphore taskSema(1, 1);
int taskToken = 1;

Coop_System System;
unsigned long time_rev = 0;
bool led=false;

void Blinky_Blinky()
{
    if (millis() - time_rev >= 300)
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
        led=!led;
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
        taskSema.wait();
        if (2 != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }

        System.Print_OLED();

        taskToken = 3;
        taskSema.post();
    }
}

// Task no.3: blink LED with 0.05 second delay.
void loop3()
{
    taskSema.wait();
    if (3 != taskToken)
    {
        taskSema.post();
        yield();
    }
    bool fire_connect = false;

#ifdef TEST
    Serial.println("Firebase");
#endif
    if (System.Firebase_Set_up())
    {
        fire_connect = true;
    }
#ifdef TEST
    else
    {
        Serial.println("Error en Firebase");
    }
#endif
    taskSema.post();
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
#ifdef TEST
            else
            {
                Serial.println("Server not enabled");
            }
#endif
        }
        taskToken = 2;
        //taskToken = 4;
        taskSema.post();
        yield();
    }
}

void loop4()
{
    taskSema.wait();
    if (4 != taskToken)
    {
        taskSema.post();
        yield();
    }
    Serial.print("Here we are");
    System.FACP_Setup();
    Serial.print("There we were");
    taskSema.post();
    for (;;) // explicitly run forever without returning
    {
        taskSema.wait();
        if (4 != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }

        System.get_Fails();
        delay(5000);
        System.OLED_Events();

        System.get_Alarms();
        delay(5000);
        System.OLED_Events();

        taskToken = 2;
        taskSema.post();
        yield();
    }
}

BasicCoopTask<CoopTaskStackAllocatorAsMember<sizeof(unsigned) >= 4 ? 1200 : STACKSIZE_8BIT>> task4("l4", loop4);
BasicCoopTask<CoopTaskStackAllocatorAsMember<sizeof(unsigned) >= 4 ? 1600 : STACKSIZE_8BIT>> task1("l1", loop1);
BasicCoopTask<CoopTaskStackAllocatorAsMember<sizeof(unsigned) >= 4 ? 1200 : STACKSIZE_8BIT>> task2("l2", loop2);
BasicCoopTask<CoopTaskStackAllocatorAsMember<sizeof(unsigned) >= 4 ? 4000 : STACKSIZE_8BIT>> task3("l3", loop3);

//BasicCoopTask<CoopTaskStackAllocatorFromLoop<sizeof(unsigned) >= 4 ? 4000 : STACKSIZE_8BIT>> task3("l3", loop3, sizeof(unsigned) >= 4 ? 4000 : STACKSIZE_8BIT);

void setup()
{
    Serial.begin(115200);
    // Setup the 3 pins as OUTPUT
    pinMode(LED_BUILTIN, OUTPUT);

    if (System.Start())
    {
        taskToken = 3;
    }
#ifdef TEST
    Serial.println(F("Sistema Inicializado Correctamente"));
#endif
    // Add "loop1", "loop2" and "loop3" to CoopTask scheduling.
    // "loop" is always started by default, and is not under the control of CoopTask.
    task1.scheduleTask();
    task2.scheduleTask();
    task3.scheduleTask();
    task4.scheduleTask();
}

void loop()
{
    // loops forever by default
    runCoopTasks();
    Blinky_Blinky();
}