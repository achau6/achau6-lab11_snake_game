/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: https://youtu.be/I6ezrocqybU
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
unsigned char snake_start_col = 0x02;
unsigned char snake_start_row = 0xFB;

unsigned char bait_row[4] = { 0xFB, 0xFE, 0xEF, 0xFD };
unsigned char bait_col[4] = { 0x80, 0x40, 0x01, 0x04 };

unsigned char snake_row[20] = { 0xFB, 0xFB};//, 0xFB, 0xFB }; //chose max size
unsigned char snake_col[20] = { 0x02, 0x01};//, 0x02, 0x01 };

int size_of_snake = 2;
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
			/*if((~PINA & 0x01) == 0x01){
                                state = up;
                        } else if((~PINA & 0x02) == 0x02){
                                state = down;
                        } else if((~PINA & 0x08) == 0x08){
                                state = right;
                        } else {
				state = wait;
			}*/


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
			//state = left;
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
        return state;
}

enum fruit { ate };
int eat(int state) {
	switch(state) {
		case ate:
			state = ate;
		break;
		default:
		break;
	}  	 	

	switch(state){
		case ate:
			for(int i = 0; i < 4; i ++) {
			if((snake_start_col == bait_col[i]) && (snake_start_row == bait_row[i])) {
                		bait_row[i] = 0x00;
                		bait_col[i] = 0x00;
                		size_of_snake += 1;
                		snake_row[size_of_snake] = snake_row[size_of_snake-1];
                		snake_col[size_of_snake] = snake_col[size_of_snake-1] >> 1;
        		}
			}
		break;
		default:
		break;
	}
	//add point system
	return state;
}

unsigned char loop_array[] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF };
unsigned char loop_2array[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
enum display2 { alway };
int printSnake(int state) {
	switch(state) {
		case alway:
		break;
		default:
		break;
	}
	switch(state){
	case alway:
	if(snake_start_row != 0x00 && snake_start_col != 0x00) {
	for(int i = 0; i < 5; i ++){
		for(int j = 0; j < 8; j ++) {
			for(int a = 0; a < size_of_snake; a ++){
				if(loop_array[i] == snake_row[a] && loop_2array[j] == snake_col[a]) {
					PORTD = loop_array[i];
					PORTC = loop_2array[j];
				}
			}
		}
	}
	} else { 
		PORTC = 0x00;
		PORTD = 0x00;
	}
	break;
	default:
	break;
	}
	return state;
}

enum display { always };
int printFruit(int state) {
	switch(state) {
		case always:
			state = always;
		break;
		default:
		break;
	}
	switch(state) {
	case always:	
        for(int i = 0; i < 5; i ++){
                for(int j = 0; j < 8; j ++) {
			for(int a = 0; a < 4; a ++) {
                        if(loop_array[i] == bait_row[a] && loop_2array[j] == bait_col[a]) {           
                                        PORTD = loop_array[i]; 
                                        PORTC = loop_2array[j];                       
                        }                          
			}			
                }                                                                                     
        }
        break;	
	default:
	break;
	}

        return state;
}

enum crash { collide };
int collision(int state) {
	switch(state) {
		case collide:
			state = collide;
		break;
		default:
		break;
	}
	switch(state) {
		case collide:
		//collision
        	for(int i = 1; i < size_of_snake; i ++) {
                	if(snake_start_row == snake_row[i] && snake_start_col == snake_col[i]) {
                        	snake_start_row = 0x00;
                        	snake_start_col = 0x00;
                        	//add program end
                        	PORTB = 0x10;
                	} else if(snake_start_row > 0xFE || snake_start_row < 0xEF) {
				snake_start_row = 0x00;
                                snake_start_col = 0x00;
                                //add program end
                                PORTB = 0x10;
			} else if(snake_start_col > 0x80 || snake_start_col < 0x01) {
				snake_start_row = 0x00;
                                snake_start_col = 0x00;
                                //add program end
                                PORTB = 0x10;
			}
       	 	}
		break;

		default:
		break;

	}

	return state;
}

enum game { on , startOver };
int restart(int state){
	switch(state) {
		case on:
			if((~PINA & 0x10) != 0x10) {
				state = on;
			} else {
				state = startOver;
			}
		break;

		case startOver:
			state = on;
		break;

		default:
		break;
	}

	switch(state){
		case on:
		break;

		case startOver:
			//for(int i = 2; i < 20; i ++) {
				//free(snake_row);
				//free(snake_col);
			//}
			size_of_snake = 2;
			snake_start_col = 0x02;
			snake_start_row = 0x0FB;

			snake_row[0] = 0xFB;
		        snake_row[1] = 0xFB;

			snake_col[0] = 0x02;
		        snake_col[1] = 0x01;

			bait_row[0] = 0xFB;
			bait_row[1] = 0xFE;
			bait_row[2] = 0xEF;
			bait_row[3] = 0xFD;

			bait_col[0] = 0x80;
			bait_col[1] = 0x40;
			bait_col[2] = 0x01;
			bait_col[3] = 0x04;
		break;

		default:
		break;
	}
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
        DDRA = 0x00; PORTA = 0xFF;
        DDRB = 0x00; PORTB = 0xFF;
        DDRC = 0xFF; PORTC = 0x00;
        DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
        static task task1, task2, task3, task4, task5, task6;
        task *tasks[] = { &task1,  &task2, &task3, &task4, &task5, &task6 };
        const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
        const char start = 0;
        task1.state = start;
        task1.period = 3000;
        task1.elapsedTime = task1.period;
        task1.TickFct = &tick;

        task2.state = always;
        task2.period = 1;
        task2.elapsedTime = task2.period;
        task2.TickFct = &printFruit;
        
        task3.state = on;
        task3.period = 100;
        task3.elapsedTime = task3.period;
        task3.TickFct = &restart;
        
        task4.state = ate;
        task4.period = 100;
        task4.elapsedTime = task4.period;
        task4.TickFct = &eat;

	task5.state = collide;
        task5.period = 100;
        task5.elapsedTime = task5.period;
        task5.TickFct = &collision;

	task6.state = alway;
        task6.period = 1;
        task6.elapsedTime = task6.period;
        task6.TickFct = &printSnake;



        TimerSet(1);
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
