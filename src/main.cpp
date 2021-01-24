#include <Arduino.h>

// Include CoopTask since we want to manage multiple tasks.
#include <CoopTask.h>
#include <CoopSemaphore.h>
#include <Coop_Serial_System.h>

#if defined(ARDUINO_AVR_MICRO)
#define STACKSIZE_8BIT 92
#else
#define STACKSIZE_8BIT 40
#endif

CoopSemaphore taskSema(1, 1);
int taskToken = 1;

Coop_System System;

// Task no.1: blink LED with 1 second delay.
void loop1() {
    for (;;) // explicitly run forever without returning
    {
        taskSema.wait();
        if (1 != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }

        if(System.Wi_Fi_Status())
        {
            if(System.Wi_Fi_Connection())
            {
                taskToken = 2;
            }
        }
        else
        {
            taskToken = 1;
        }


        for (int i = 0; i < 3; ++i)
        {
            digitalWrite(LED_BUILTIN, HIGH);

            // IMPORTANT:
            // When multiple tasks are running 'delay' passes control to
            // other tasks while waiting and guarantees they get executed.
            delay(1000);

            digitalWrite(LED_BUILTIN, LOW);
            delay(1000);
        }
        //taskToken = 2;
        taskSema.post();
    }
}

// Task no.2: blink LED with 0.25 second delay.
void loop2() {
    for (;;) // explicitly run forever without returning
    {
        taskSema.wait();
        if (2 != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }
        for (int i = 0; i < 6; ++i)
        {
            digitalWrite(LED_BUILTIN, HIGH);

            // IMPORTANT:
            // When multiple tasks are running 'delay' passes control to
            // other tasks while waiting and guarantees they get executed.
            delay(500);

            digitalWrite(LED_BUILTIN, LOW);
            delay(500);
        }
        taskToken = 3;
        taskSema.post();
    }
}

// Task no.3: blink LED with 0.05 second delay.
void loop3() {
    for (;;) // explicitly run forever without returning
    {
        taskSema.wait();
        if (3 != taskToken)
        {
            taskSema.post();
            yield();
            continue;
        }
        for (int i = 0; i < 6; ++i)
        {
            digitalWrite(LED_BUILTIN, HIGH);

            // IMPORTANT:
            // When multiple tasks are running 'delay' passes control to
            // other tasks while waiting and guarantees they get executed.
            delay(250);

            digitalWrite(LED_BUILTIN, LOW);
            delay(250);
        }
        taskToken = 1;
        taskSema.post();
    }
}

BasicCoopTask<CoopTaskStackAllocatorAsMember<sizeof(unsigned) >= 4 ? 800 : STACKSIZE_8BIT>> task1("l1", loop1);
BasicCoopTask<CoopTaskStackAllocatorAsMember<sizeof(unsigned) >= 4 ? 800 : STACKSIZE_8BIT>> task2("l2", loop2);
BasicCoopTask<CoopTaskStackAllocatorFromLoop<sizeof(unsigned) >= 4 ? 800 : STACKSIZE_8BIT>> task3("l3", loop3, sizeof(unsigned) >= 4 ? 800 : STACKSIZE_8BIT);

void setup() {
    Serial.begin(115200);
    // Setup the 3 pins as OUTPUT
    pinMode(LED_BUILTIN, OUTPUT);

    if(!System.Start())
    {
        Serial.println(F("Error durante la inicializacion"));
        digitalWrite(LED_BUILTIN, LOW);
        for(;;);
    }
    #ifdef DEBUG
    Serial.println(F("Sistema Inicializado Correctamente"));
    #endif
    // Add "loop1", "loop2" and "loop3" to CoopTask scheduling.
    // "loop" is always started by default, and is not under the control of CoopTask. 
    task1.scheduleTask();
    task2.scheduleTask();
    task3.scheduleTask();



}

void loop() {
    // loops forever by default
    runCoopTasks();

}