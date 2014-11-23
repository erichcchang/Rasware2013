#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/adc.h>
#include <RASLib/inc/motor.h>
#include <RasLib/inc/linesensor.h>
#include "SysTick.h"
#include "PLL.h"
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
int stage = 0;


void DisableInterrupts(void);
void EnableInterrupts(void);
void figureEight(void);
void followWall(void);
void squareDance (void);
void followLine (void);
void findObject (void);
bool linePresent (void);



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
    gls = InitializeGPIOLineSensor(PIN_C7, PIN_C6, PIN_E0, PIN_D3, PIN_D2, PIN_D1, PIN_D0, PIN_B5);    
}

int main(void) {
	PLL_Init();       	// bus clock at 80 MHz
	SysTick_Init(80000);        // initialize SysTick timer with interrupts every 1ms
	initIRSensor(); 
	initMotor();
	initGPIOLineSensor();
	
	while(true){
		//LineSensorReadArray(gls, line);
		switch (stage) {
			case 0:				//start state
				if (linePresent()) {followLine();}
				else {followWall();}
				break;
			case 1:				//once 90degree turn passed
				break;
			case 2:				//once line found again after walled section
				break;
			case 3:				//end of course look for flag
				break;
		}
	}
}


void SysTick_Handler(void){

}

bool linePresent(void){
	int i = 0;
	int count = 0;
	LineSensorReadArray(gls, line);
	for (i = 0; i<8; i+=1){
		if(line[i]>0.5){count += 1;}
	}
	if(count>1&&count<4) {return true;}
	else {return false;}
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
	stage = 1;
}


void followWall(void){ // includes avoiding other robots
	////get sensor values
	bool wasLeftWall = false;
	bool wasRightWall = false;
	frontSensor = ADCRead(adc[0])*1000;
	rightSensor = ADCRead(adc[1])*1000;
	leftSensor = ADCRead(adc[2])*1000;
	
	if (rightSensor>300 && rightSensor>leftSensor) {			///follow whichever wall is closer
		//////if right wall is closer
		if(frontSensor > 900){					////back up and turn if wall ahead
			SetMotor(leftMotor, -1);
			SetMotor(rightMotor, -1);
			SysTick_Wait10ms(70);			//reverse for 3 seconds
			turn90Degrees(LEFT);
			wasRightWall = true;
			wasLeftWall = false;
		}
		else if(rightSensor > 700){
			SetMotor(leftMotor, -.25);
			SetMotor(rightMotor, .8);
		}
		else{
			SetMotor(leftMotor, 1);
			SetMotor(rightMotor, .25);
		}
	}
	/////////if left wall is closer
	else if(leftSensor>300 && leftSensor>rightSensor){												
		if(frontSensor > 900){					////back up and turn if wall ahead
			SetMotor(leftMotor, -1);
			SetMotor(rightMotor, -1);
			SysTick_Wait10ms(70);			//reverse
			turn90Degrees(RIGHT);
			wasLeftWall = true;
			wasRightWall = false;
		}
		else if(leftSensor > 700){
			SetMotor(leftMotor, .8);
			SetMotor(rightMotor, -.25);
		}
		else{
			SetMotor(leftMotor, .25);
			SetMotor(rightMotor, 1);
		}
	}
	else {
		if (wasLeftWall) {
			SetMotor(leftMotor, 1);
			SetMotor(rightMotor, .7);
		}
		else {
			SetMotor(leftMotor, .7);
			SetMotor(rightMotor, 1);
		}
	}
}

void followLine(void){
	// put the values of the line sensor into the 'line' array 
			SetPin(PIN_F2, 1);
        LineSensorReadArray(gls, line);
				if((line[3]>0.5)&&(line[4]>0.5)){
						SetMotor(leftMotor, 1);
						SetMotor(rightMotor, 1);
				}
				else if (line[5]>0.5&&line[4]>0.5){
						SetMotor(leftMotor, 1);
						SetMotor(rightMotor, .8);
				}
				else if (line[2]>0.5&&line[3]>0.5){
						SetMotor(leftMotor, .8);
						SetMotor(rightMotor, 1);
				}
				else if (line[6]>0.5&&line[5]>0.5){
						SetMotor(leftMotor, .6);
						SetMotor(rightMotor, .3);
				}
				else if (line[1]>0.5&&line[2]>0.5){
						SetMotor(leftMotor, .6);
						SetMotor(rightMotor, .3);
				}	
				else if (line[7]>0.5&&line[6]>0.5){
						SetMotor(leftMotor, .3);
						SetMotor(rightMotor, -.25);
				}
				else if (line[0]>0.5&&line[1]>0.5){
						SetMotor(leftMotor, -.25);
						SetMotor(rightMotor, .3);
				}
				SetPin(PIN_F2, 0);
}


void squareDance(void){
	int turns = 0;
	bool postTurn = true;
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
			SysTick_Wait10ms(250);   // go straight for a short time to get back by object
			do {
				if (clockWise) {sensor = ADCRead(adc[1])*1000;}else{sensor = ADCRead(adc[2])*1000;}
			}while(sensor > 400);
			postTurn = false;
		}
		else {
			SetMotor(leftMotor, 1);
			SetMotor(rightMotor, 1);
			/////keep going straight while no wall is close
			do {
				if (clockWise) {sensor = ADCRead(adc[1])*1000;}else{sensor = ADCRead(adc[2])*1000;}
			}while(sensor < 400);
			/////now that we have encountered a wall
			/////keep going straight until we have passed the wall so we can make turn
			do {
				if (clockWise) {sensor = ADCRead(adc[1])*1000;}else{sensor = ADCRead(adc[2])*1000;}
			}while(sensor > 400);
			SysTick_Wait10ms(70);
			/////stop briefly
			SetMotor(leftMotor, 0);
			SetMotor(rightMotor, 0);
			SysTick_Wait10ms(40);
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


