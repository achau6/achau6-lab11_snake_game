/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "timer.h"
#include "keypad.h"
#include "schedule.h"
#include "bit.h"
#include "pwn.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum Demo_States {shift};
int Demo_Tick(int state) {

        // Local Variables
        static unsigned char pattern = 0x80;    // LED pattern - 0: LED off; 1: LED on
        static unsigned char row = 0xFE;        // Row(s) displaying pattern. 
                                                        // 0: display pattern on row
                                                        // 1: do NOT display pattern on row
        // Transitions
        switch (state) {
                case shift:
                break;
                default:
                        state = shift;
                break;
        }
        // Actions
        switch (state) {
                case shift:
                        if (row == 0xEF && pattern == 0x01) { // Reset demo 
                                pattern = 0x80;
                                row = 0xFE;
                        } else if (pattern == 0x01) { // Move LED to start of next row
                                pattern = 0x80;
                                row = (row << 1) | 0x01;
                        } else { // Shift LED one spot to the right on current row
                                pattern >>= 1;
                        }
                        break;
                default:
        break;
        }
        PORTC = pattern;        // Pattern to display
        PORTD = row;            // Row(s) displaying pattern    
        return state;
}

unsigned char snake_start_col = 0x08;
unsigned char snake_start_row = 0xFE;
enum snakes { start, init, up, down, right, left };
int tick(int state){
        /*
        PA0 = Up
        PA1 = Down
        PA2 = Left
        PA3 = Right

        Boundaries: Column = 0x01, Row = 0xEF
        Pattern is the Column
        To move right
                Pattern >>= 1
        To move left
                Pattern <<= 1
        To move down
                row = (row << 1) | 0x01
         */
        switch(state){
                case start:
                        state = init;
                break;

                case init:
                        if((~PINA & 0x01) == 0x01){
                                state = up;
                        } else if((~PINA & 0x02) == 0x02){
                                state = down;
                        } else if((~PINA & 0x04) == 0x04){
                                state = left;
                        } else if((~PINA & 0x08) == 0x08){
                                state = right;
                        } else {
                                state = init;
                        }
                break;

                case up:
                        if((~PINA & 0x01) == 0x01){
                                state = up;
                        } else if((~PINA & 0x02) == 0x02){
                                state = down;
                        } else if((~PINA & 0x04) == 0x04){
                                state = left;
                        } else if((~PINA & 0x08) == 0x08){
                                state = right;
                        } else {
                                state = init;
                        }

                break;

                case down:
                        /*if((~PINA & 0x08) != 0x00){
                                
                        } else {
                                state = init;
                        }*/
                        if((~PINA & 0x01) == 0x01){
                                state = up;
                        } else if((~PINA & 0x02) == 0x02){
                                state = down;
                        } else if((~PINA & 0x04) == 0x04){
                                state = left;
                        } else if((~PINA & 0x08) == 0x08){
                                state = right;
                        } else {
                                state = init;
                        }

                break;

                case left:
                        if((~PINA & 0x01) == 0x01){
                                state = up;
                        } else if((~PINA & 0x02) == 0x02){
                                state = down;
                        } else if((~PINA & 0x03) == 0x04){
                                state = left;
                        } else if((~PINA & 0x08) == 0x08){
                                state = right;
                        } else {
                                state = init;
                        }

                break;

                case right:
                        if((~PINA & 0x01) == 0x01){
                                state = up;
                        } else if((~PINA & 0x02) == 0x02){
                                state = down;
                        } else if((~PINA & 0x04) == 0x04){
                                state = left;
                        } else if((~PINA & 0x08) == 0x08){
                                state = right;
                        } else {
                                state = init;
                        }

                break;
                default:
                break;
        }
        switch(state){
                case init:
                break;

                case up:
                        snake_start_row = (snake_start_row >> 1) | 0x01;
                break;

                case down:
                        snake_start_row = (snake_start_row << 1) | 0x01;
                break;

                case left:
                        if(snake_start_col != 0x80){
                                snake_start_col <<= 1;
                        }
                break;
                case right:
                        if(snake_start_col != 0x01){
                                snake_start_col >>= 1;
                        }
                break;

                default:
                break;
        }
        PORTC = snake_start_col;
        PORTD = snake_start_row;
        return state;
}
/*unsigned long int findGCD(unsigned long int a, unsigned long in b){
        unsigned long int c;
        while(1){
                c = a%b;
                if(c==0){return b;}
                a=b;
                b=c;
        }
        return 0;
}*/
int main(void) {
    /* Insert DDR and PORT initializations */
        DDRA = 0x00; PORTA = 0xFF;
        DDRB = 0x00; PORTB = 0xFF;
        DDRC = 0xFF; PORTC = 0x00;
        DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
        static task task1;// task2, task3;// task4;
        task *tasks[] = { &task1};// &task2, &task3};// &task4 };
        const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
        const char start = 0;
        task1.state = start;
        task1.period = 100;
        task1.elapsedTime = task1.period;
        task1.TickFct = &tick;

        /*task2.state = startsp;
        task2.period = 100;

        task2.elapsedTime = task2.period;
        task2.TickFct = &tick;
        
        task3.state = startL;
        task3.period = 150;
        task3.elapsedTime = task3.period;
        task3.TickFct = &changeLock;
        
        task4.state = start;
        task4.period = 10;
        task4.elapsedTime = task4.period;
        task4.TickFct = &displaySMTick;*/


        TimerSet(100);
        TimerOn();

        unsigned short i;
    while (1) {
        if((~PINA & 0x01) == 0x01){
                PORTB = 0x01;
        }
        if((~PINA & 0x02) == 0x02){
                PORTB = 0x02;
        }
        if((~PINA & 0x04) == 0x04){
                PORTB = 0x04;
        }
        if((~PINA & 0x08) == 0x08){
                PORTB = 0x08;
        }

        for( i = 0; i < numTasks; i ++) {
                if( tasks[i]->elapsedTime >= tasks[i]->period) {
                        tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                        tasks[i]->elapsedTime = 0;
                }
                tasks[i]->elapsedTime += tasks[i]->period;
        }

        unsigned long int duration = 0;
        while(!TimerFlag);
        TimerFlag = 0;
    }

    return 1;
}
