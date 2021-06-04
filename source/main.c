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





*/
unsigned char snake_start_col = 0x08;
unsigned char snake_start_row = 0xFB;
unsigned char tail_col = 0x01;
unsigned char tail_row = 0x02;
unsigned char bait_row = 0xFB;
unsigned char bait_col = 0x08;
unsigned char snake_row[20] = { 0xFB, 0xFB, 0xFB, 0xFB }; //chose max size
unsigned char snake_col[20] = { 0x08, 0x04, 0x02, 0x01 };
unsigned char snake_array[5][8] = { 
				    0, 0, 0, 0, 0, 0, 0, 0,
				    0, 0, 0, 0, 0, 0, 0, 0,
				    0, 0, 0, 0, 1, 1, 1, 1,
				    0, 0, 0, 0, 0, 0, 0, 0,
				    0, 0, 0, 0, 0, 0, 0, 0,
	
				   }; 
int size_of_snake = 4;
enum snakes { start, init, up, down, right, left, wait };
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
                                state = wait;
                        } else {
                                state = init;
                        }

                break;

                case down:
                        if((~PINA & 0x02) == 0x02){
                                state = wait;
                        } else {
                                state = init;
                        }

                break;

                case left:
                        if((~PINA & 0x04) == 0x04){
                                state = wait;
                        } else {
                                state = init;
                        }


                break;

                case right:
                        if((~PINA & 0x08) == 0x08){
                                state = wait;
                        } else {
                                state = init;
                        }

                break;

		case wait:
			if((~PINA & 0x0F) != 0x00) {
				state = wait;
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
		case wait:
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
        if((snake_start_col == bait_col) && (snake_start_row == bait_row)) {
                bait_row = 0x00;
                bait_col = 0x00;
        }  


	/*unsigned char old_bait_col = bait_col;
	unsigned char old_bait_row = bait_row;

	if(bait_row != 0x00 & bait_col != 0x00) {
		if(old_bait_col != bait_col && old_bait_row != bait_row){
			if(old_bait_row != bait_row){
				snake_rows = (snake_rows & bait_row) 
			}
		}
		snake_rows = snake_rows & bait_row; //bait
		snake_cols = snake_cols | bait_col;
	}*/

	//collision
	for(int i = 1; i < size_of_snake; i ++) {
		if(snake_start_row == snake_row[i] && snake_start_col == snake_col[i]) {
			snake_cols = 0x00;
			snake_rows = 0x00;
			//add program end
			PORTB = 0x10;
		}
	}
        return state;
}
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

unsigned char loop_array[] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF };
unsigned char loop_2array[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
unsigned char stub1, stub2;
int print(int state) {
	for(int i = 0; i < 5; i ++){
		for(int j = 0; j < 8; j ++) {
			/*if(snake_array[i][j] == 1) {
				PORTD = stub1;
				PORTC = stub2;
			}*/
			for(int a = 0; a < size_of_snake; a ++){
				if(loop_array[i] == snake_row[a] && loop_2array[j] == snake_col[a]) {
					PORTD = loop_array[i];
					PORTC = loop_2array[j];
				}
			}
			if(loop_array[i] == bait_row && loop_2array[j] == bait_col) {
				if(bait_row != 0x00) {
				PORTD = bait_row;
				PORTC = bait_col;
				}
			}
		}
        }
	return state;
}

int display(int state){
	//PORTD = stub1;
	//PORTC = stub2;
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
        DDRA = 0x00; PORTA = 0xFF;
        DDRB = 0x00; PORTB = 0xFF;
        DDRC = 0xFF; PORTC = 0x00;
        DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
        static task task1, task2, task3;// task4;
        task *tasks[] = { &task1,  &task2, &task3};// &task4 };
        const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
        const char start = 0;
        task1.state = start;
        task1.period = 100;
        task1.elapsedTime = task1.period;
        task1.TickFct = &tick;

        //task2.state = startsp;
        task2.period = 10;
        task2.elapsedTime = task2.period;
        task2.TickFct = &print;
        
        //task3.state = startL;
        task3.period = 100;
        task3.elapsedTime = task3.period;
        task3.TickFct = &display;
        
        /*task4.state = start;
        task4.period = 10;
        task4.elapsedTime = task4.period;
        task4.TickFct = &displaySMTick;*/


        TimerSet(10);
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
