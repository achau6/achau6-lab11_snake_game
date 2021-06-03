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
#include <avr/interrupt.h>
#include "timer.h"
#include "keypad.h"
#include "schedule.h"
#include "bit.h"
#include "pwn.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

/*
board = 	0x80	0x40	0x20	0x10	0x80	0x40	0x20	0x01
	0xFE

	0xFD

	0xFB

	0xF7

	0xEF




*/
unsigned char snake_start_col = 0x02;
unsigned char snake_start_row = 0xFB;
unsigned char bait_row = 0xFB;
unsigned char bait_col = 0x08;
unsigned char snake_row[20] = { 0xFB, 0xFB }; //chose max size
unsigned char snake_col[20] = { 0x02, 0x01 };
int size_of_snake = 2;
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
			if(snake_start_row == 0xFE){
                                snake_start_row = snake_start_row;
                        } else if(snake_start_row != 0xFE){
                        	snake_start_row = (snake_start_row >> 1) | 0x80;
				if(size_of_snake > 1) {
                                        //unsigned char old_pos_row = snake_row[size_of_snake];
                                        for(int i = size_of_snake; i > 0; i --) {
                                                snake_row[i] = snake_row[i-1];
                                                snake_col[i] = snake_col[i-1];
                                        }
                                        snake_row[0] = snake_start_row;
                                        snake_col[0] = snake_start_col;
                                }
			}
                break;

                case down:
			if(snake_start_row != 0xEF) {
				snake_start_row = (snake_start_row << 1) | 0x01;
				if(size_of_snake > 1) {
					//unsigned char old_pos_row = snake_row[size_of_snake];
					for(int i = size_of_snake; i > 0; i --) {
						snake_row[i] = snake_row[i-1];
						snake_col[i] = snake_col[i-1];
					}
					snake_row[0] = snake_start_row;
					snake_col[0] = snake_start_col;
				} 
			}
                break;

                case left:
                        if(snake_start_col != 0x80){
                                snake_start_col <<= 1;
				if(size_of_snake > 1) {
                                        for(int i = size_of_snake; i > 0; i --) {
                                                snake_col[i] = snake_col[i-1];
						snake_row[i] = snake_row[i-1];
						
                                        }
					snake_row[0] = snake_start_row;
                                        snake_col[0] = snake_start_col;
                                }
                        }
                break;
                case right:
                        if(snake_start_col != 0x01){
                                snake_start_col >>= 1;
				if(size_of_snake > 1) {
                                        for(int i = size_of_snake; i > 0; i --) {
                                                snake_col[i] = snake_col[i-1];
                                                snake_row[i] = snake_row[i-1];
                                        }
                                        snake_row[0] = snake_start_row;
                                        snake_col[0] = snake_start_col;
                                }
                        }
                break;

                default:
                break;
        }
	 unsigned char old_pos_row = snake_start_row;
        /*if((snake_start_col == bait_col) && (snake_start_row == bait_row)) {
                //fruit eaten +1
                size_of_snake += 1;
                //adds one to tail end with the old position
                snake_row[size_of_snake] = old_pos_row;
                bait_row = 0x00;
                bait_col = 0x00;
        } */      

	unsigned char snake_rows = snake_start_row;
	for(int i = 0; i < size_of_snake; i ++){
		//in case array contains empty elements
		if(snake_row[i] != 0x00) {
			snake_rows &= snake_row[i];
		}
	}

	unsigned char snake_cols = snake_start_col;
        for(int i = 0; i < size_of_snake; i ++){
                //in case array contains empty elements
                if(snake_col[i] != 0x00) {
                       snake_cols |= snake_col[i];
                }
        }


	if(bait_row != 0x00 & bait_col != 0x00) {
		snake_rows = snake_rows & bait_row; //bait
		snake_cols = snake_cols | bait_col;
	}
        PORTC = snake_cols;
        PORTD = snake_rows;
        return state;
}
//			        [can't tail block the way]
//snake = [0][3] = [0xFE][0x08],      [0xF9][0x04] eat the bait OR [0xFB],[0xFD] 
//				[0xFB][0x18]/[0xFB][0x0C] 
//					[0xF3][0x04]
//size_of_snake = 3;
//
//snake_row[20]; += [0xFB], [0xFD], [0xFE]
//PORD = snake_row[i-1] & snake_row[i]
//
//snake_col[20]; += [0x04], [0x04], [0x04]
//
//
//bait = [2][2] = [0xFB][0x04]
//
//left:  col = col << 1 | col;
//right: col = col >> 1 | col;
/*
Board:	  8 7 6 5 4 3 2 1 0
	0
	1
	2
	3
	4



 */
int converter(){
	/*unsigned char new_row, new _col;
	case up:
	break;

	case down:
	break;

	case left:
		if(new_row != snake_start_row && new_col != snake_start_col){
			row = (snake_start_row << 1) | snake_start_row;
		        row = row |= 0x01;	
			col = (snake_start_col << 1) | snake_start_col;
			//col = [0x18];
		}
	break;

	case right:
	break;*/

}

int eat(int state) {
	/*default fruit location
	
	
	*/
	//keeps track of the LAST position when snake is still size 0
	unsigned char old_pos_row = snake_start_row;
	if((snake_start_col == bait_col) && (snake_start_row == bait_row)) {
		//fruit eaten +1
		size_of_snake += 1; 
		//adds one to tail end with the old position
		snake_row[size_of_snake] = old_pos_row;
	       	bait_row = 0x00;
		bait_col = 0x00;	
	}  	 	

	switch(state){

	}

}

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

        while(!TimerFlag);
        TimerFlag = 0;
    }

    return 1;
}
