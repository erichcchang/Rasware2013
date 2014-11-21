#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/adc.h>
#include <RASLib/inc/motor.h>
#include <RasLib/inc/linesensor.h>
#include "SysTick.h"
#include <stdbool.h>

#define LEFT 1
#define RIGHT -1

tADC *adc[3];
//tMotor *servomotor[2];
tMotor *rightMotor;
tMotor *leftMotor;
static tLineSensor *gls;
float frontSensor;
float leftSensor;
float rightSensor;
float line[8];
int wasLeft = 0;
int wasRight = 0;

void figureEight(void);
void followWall(void);



void initIRSensor(void) {
    adc[0] = InitializeADC(PIN_E5);
    adc[1] = InitializeADC(PIN_E4);
	  adc[2] = InitializeADC(PIN_B4);
}
void initMotor(void) {
		leftMotor = InitializeServoMotor(PIN_B6,false);			//left motor
		rightMotor = InitializeServoMotor(PIN_B7,true);			//right motor
}

void initGPIOLineSensor(void) {
    // use 8 I/O pins to initialize a GPIO line sensor
    gls = InitializeGPIOLineSensor(PIN_B5, PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_E0, PIN_C6, PIN_C7);
}

// The 'main' function is the entry point of the program
int main(void) {
		
    // Initialization code can go here
		initIRSensor(); 
		initMotor();
		initGPIOLineSensor();
	
		while(true){
			followWall();
		}
}

void findObject(void) {
      // Runtime code can go here
			int item = 0;
      frontSensor = ADCRead(adc[0])*1000;
			while (item == 0) {	
				frontSensor = ADCRead(adc[0])*1000;
				if (frontSensor < 200) {
					SetMotor(rightMotor, -.1); //rotate
					SetMotor(leftMotor, .1);
				}else{
					item = 1;
				}
			}
			
			SetMotor(rightMotor, 1); //get can
			SetMotor(leftMotor, 1);
			frontSensor = ADCRead(adc[0])*1000;
			rightSensor = ADCRead(adc[1])*1000;
			leftSensor = ADCRead(adc[2])*1000;

			while (frontSensor < 850 && leftSensor < 850 && rightSensor < 850){
					frontSensor = ADCRead(adc[0])*1000;
					rightSensor = ADCRead(adc[1])*1000;
					leftSensor = ADCRead(adc[2])*1000;
			}
					SetMotor(rightMotor, 0); //STOP 
					SetMotor(leftMotor, 0);
			while(1);
}



void turn90Degrees(int dir){
	if (dir == LEFT) {
			SetMotor(leftMotor, -1);		
			SetMotor(rightMotor, 1);
			SysTick_Wait10ms(80);
	}
	if (dir == RIGHT){
			SetMotor(leftMotor, 1);		
			SetMotor(rightMotor, -1);
			SysTick_Wait10ms(80);
	}
}


void followWall(void){ // includes avoiding other robots
	////get sensor values
	frontSensor = ADCRead(adc[0])*1000;
	rightSensor = ADCRead(adc[1])*1000;
	leftSensor = ADCRead(adc[2])*1000;
	
	if (rightSensor>leftSensor) {			///follow whichever wall is closer
		//////if right wall is closer
		if(frontSensor > 900){					////back up and turn if wall ahead
			SetMotor(leftMotor, -1);
			SetMotor(rightMotor, -1);
			SysTick_Wait10ms(70);			//reverse for 3 seconds
			turn90Degrees(LEFT);
		}
		else if(rightSensor > 800){
			SetMotor(leftMotor, -.25);
			SetMotor(rightMotor, .8);
		}
		else{
			SetMotor(leftMotor, 1);
			SetMotor(rightMotor, .25);
		}
	}
	/////////if left wall is closer
	else {												
		if(frontSensor > 900){					////back up and turn if wall ahead
			SetMotor(leftMotor, -1);
			SetMotor(rightMotor, -1);
			SysTick_Wait10ms(70);			//reverse
			turn90Degrees(RIGHT);
		}
		else if(leftSensor > 650){
			SetMotor(leftMotor, .8);
			SetMotor(rightMotor, -.25);
		}
		else{
			SetMotor(leftMotor, .25);
			SetMotor(rightMotor, 1);
		}
	}
}

void followLine(void){
	// put the values of the line sensor into the 'line' array 
        LineSensorReadArray(gls, line);
				
				if((line[3]>0.5)||(line[4]>0.5)){
						SetMotor(leftMotor, 1);
						SetMotor(rightMotor, 1);
				}
				else if (line[5]>0.5){
						SetMotor(leftMotor, .9);
						SetMotor(rightMotor, .25);
				}
				else if (line[2]>0.5){
						SetMotor(leftMotor, .25);
						SetMotor(rightMotor, .9);
				}
				else if (line[6]>0.5){
						SetMotor(leftMotor, .8);
						SetMotor(rightMotor, .1);
				}
				else if (line[1]>0.5){
						SetMotor(leftMotor, .1);
						SetMotor(rightMotor, .8);
				}				
				else if (line[7]>0.5){
						SetMotor(leftMotor, .7);
						SetMotor(rightMotor, 0);
				}
				else if (line[0]>0.5){
						SetMotor(leftMotor, 0);
						SetMotor(rightMotor, .7);
				}				
}


void squareDance(void){
	int turns = 0;
	bool postTurn = false;
	bool clockWise;
	float sensor;
	
	rightSensor = ADCRead(adc[1])*1000;
	leftSensor = ADCRead(adc[2])*1000;
	if (rightSensor>leftSensor){
		clockWise = true;
	}else{
		clockWise = false;
	}
	
	while(turns < 4) {
		if (clockWise) {sensor = ADCRead(adc[1])*1000;}else{sensor = ADCRead(adc[2])*1000;}
		if (postTurn){
			//////after we make a turn 
			SetMotor(leftMotor, 1);
			SetMotor(rightMotor, 1);
			SysTick_Wait10ms(50);   // go straight for a short time to get back by object
			do {
				if (clockWise) {sensor = ADCRead(adc[1])*1000;}else{sensor = ADCRead(adc[2])*1000;}
			}while(sensor > 600);
			postTurn = false;
		}
		else {
			SetMotor(leftMotor, 1);
			SetMotor(rightMotor, 1);
			/////keep going straight while no wall is close
			do {
				if (clockWise) {sensor = ADCRead(adc[1])*1000;}else{sensor = ADCRead(adc[2])*1000;}
			}while(sensor < 600);
			/////now that we have encountered a wall
			/////keep going straight until we have passed the wall so we can make turn
			do {
				if (clockWise) {sensor = ADCRead(adc[1])*1000;}else{sensor = ADCRead(adc[2])*1000;}
			}while(sensor > 600);
			/////stop briefly
			SetMotor(leftMotor, 0);
			SetMotor(rightMotor, 0);
			/////turn corner
			if (clockWise) {turn90Degrees(RIGHT);} else { turn90Degrees(LEFT);}
			turns += 1;
			postTurn = true;
		}
	}
	//////once we have made 4 turns we will stop
	SetMotor(leftMotor, 0);
	SetMotor(rightMotor, 0);
	while(true);
}

void figureEight(void) {
      // Runtime code can go here
      frontSensor = ADCRead(adc[0])*1000;
			rightSensor = ADCRead(adc[1])*1000;
			leftSensor = ADCRead(adc[2])*1000;
			
			if(rightSensor > 300 && leftSensor < 300){	
				if (rightSensor > 700){
					SetMotor(rightMotor, .2); //TURN RIGHT
					SetMotor(leftMotor, 1);
				}else{
					SetMotor(rightMotor, .15); //TURN RIGHT
					SetMotor(leftMotor, 1);
				}
				wasRight = 1;
				wasLeft = 0;
			}
			else if(leftSensor > 300 && rightSensor < 300){
				if (leftSensor>700){
					SetMotor(rightMotor, 1);
					SetMotor(leftMotor,.2);
				}else{
					SetMotor(rightMotor, 1); //TURN LEFT
					SetMotor(leftMotor, .15);
				}
				wasLeft = 1;
				wasRight = 0;
			}
			else if (leftSensor>300 && rightSensor>300) {
				if (wasRight){
					SetMotor(leftMotor, .2); 
					SetMotor(rightMotor, 1);
				}else if (wasLeft){
					SetMotor(leftMotor, 1); 
					SetMotor(rightMotor, .2);
				}
			}
			else{
				SetMotor(leftMotor, 1);
				SetMotor(rightMotor, 1);
				wasLeft = 0;
				wasRight = 0;
			}
			
}


