
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "Timer.h"
#include "uart.h"
#include "lcd.h"
#include "adc.h"
#include "driverlib/interrupt.h"
#include "open_interface.h"
#include "movement.h"
#include "servo.h"
#include "ping.h"

volatile int doSomething;
volatile bool mode = true;
volatile short hCount;
volatile char input;
volatile bool foundSmallest = false;


struct someObject{
    short start;
    short finish;
    short size;
    short angle;
    bool fr;
};

void runScan(float scanData[], bool state, short totalScans) {

    double distance;
    double denom = 14937.0;
    double exponent = -1 / 0.933;

    hCount = 1;
    char str[] = "\r\nDegrees   Distance(cm)\r\n";
    uart_sendStr(str);

    short i, scans;
    char data[40];
    scan(0);
    timer_waitMillis(100);





    for (i = 0; i <= 90; i++) {
        if (mode != state) {
            break;
        }

        scan(i * 2);

        distance = 0;
        for (scans = 0; scans < totalScans; scans++) {
            distance += adc_read();
        }
        distance /= totalScans * denom;

        distance = pow(distance, exponent);
        if (distance - 5 <= 0) {
            foundSmallest = true;
            break;
        }
        scanData[i] = distance;


        sprintf(data, "\r\n%-10d%.2f", i * 2, distance);
        uart_sendStr(data);


        timer_waitMillis(20);
    }

    uart_sendChar('\n');


}

void displayObjects(float scanData[], struct someObject objects[]) {
    unsigned short i;
    unsigned short j;
    unsigned short k;
    bool printed = false;


    for(k = 0; k < 7; k++) {
        uart_sendChar('\r');
        uart_sendChar('\n');
        for(i = 90; i > 0; i--) {
            for(j = 0; j < 5; j++) {
                if(i >= objects[j].start && i <= objects[j].finish ) {
                   uart_sendChar('#');
                   printed = true;
                   break;
                }
            }
            if(printed) {
                printed = false;
            } else {
                uart_sendChar(' ');
            }
        }
    }
}

void createObjects(float scanData[], struct someObject objects[]) {
    short scanDataIterate;
    float currScan = 0.0;
    float lastScan = 0.0;
    short objectCount = 0;
    short points = 0;
    short start = -1;

    float range = 4;

    int i;
    for(i = 0; i < 5; i++) {
        objects[i].start = 181;
        objects[i].finish = 181;
        objects[i].size = 0;
        objects[i].angle = 0;
        objects[i].fr = false;
    }


    bool canWrite = false; // Helps determine if an object can be created at the moment
    for(scanDataIterate = 1; scanDataIterate < 91; scanDataIterate++) { // iterate through scanData values
        currScan = scanData[scanDataIterate]; // Current scanData value
        lastScan = scanData[scanDataIterate - 1]; // value behind current scanData value


        if(currScan <= lastScan + range && currScan >= lastScan - range && currScan < 60 && scanDataIterate < 90) { // If current value is in the range of the last value add points
            if(start == -1) {
                start = scanDataIterate;
                canWrite = true;
            }
            points++; // If points >= 5 then its probably an object
        } else if (points >= 4 && canWrite == true) { // probably and object and can write object then create a struct
            points = 1;
            objects[objectCount].start = start - 1;
            objects[objectCount].finish = scanDataIterate;
            objects[objectCount].size = objects[objectCount].finish - objects[objectCount].start;
            objects[objectCount].angle =  (objects[objectCount].finish + objects[objectCount].start);
            objects[objectCount].fr = true;
            objectCount++;
            start = -1; // reset values
            canWrite = false;
        } else {
            start = -1; // if not within range then reset all values
            points = 1;
            canWrite = false;
        }
    }
}

short determineSmallest(struct someObject objects[]) {
    char data[40];
    short z;
    short smallestObject = 0;
    for(z = 0; z < 5; z++) {
        if(objects[z].size < objects[smallestObject].size && objects[z].fr == true && objects[z].size < 10) {
            smallestObject = z;
        }
        if(objects[z].fr == true) {
            sprintf(data, "\r\nObj:%d s:%hd f:%hd size:%hd", z + 1, objects[z].start * 2, objects[z].finish * 2, objects[z].size);
            uart_sendStr(data);
        }
    }

    free(data);
    return smallestObject;
}


int main(void)
{

    lcd_init();
    adc_init();
    uart_init();
    uart_interrupt_init();
    servo_init();
    ping_init();


    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);


    float scanData[91];







    short totalScans = 10;
    bool state = false;
    struct someObject objects[5];
    short smallestObjIndex;
    short smallestObjAngle;



    while(1) {
//        input = uart_receive();
//        uart_sendChar(input);
        uart_sendChar(doSomething);
        switch(doSomething) {

        case 1:
            doSomething = 0;
            state = mode;
            runScan(scanData, state, totalScans);
            scan(179);
            createObjects(scanData, objects);
            smallestObjIndex = determineSmallest(objects);
            smallestObjAngle = objects[smallestObjIndex].angle;
            scan(smallestObjAngle);

            displayObjects(scanData, objects);

            break;
        case 2:
            doSomething = 0;
            move_forward(sensor_data, 50);
            break;
        case 3:
            doSomething = 0;
            move_backwards(sensor_data, 50);
            break;
        case 4:
            doSomething = 0;
            turn_left(sensor_data, 20);
            break;
        case 5:
            doSomething = 0;
            turn_right(sensor_data, 20);
            break;
        }
    }
}



