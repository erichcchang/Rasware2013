#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/adc.h>
#include <RASLib/inc/motor.h>
#include <RasLib/inc/linesensor.h>

tADC *adc[2];
//tMotor *servomotor[2];
tMotor *rightmotor;
tMotor *leftmotor;
static tLineSensor *gls;
float frontSensor;
float rightSensor;
float line[8];




void initIRSensor(void) {
    adc[0] = InitializeADC(PIN_D0);
    adc[1] = InitializeADC(PIN_D1);
}
void initMotor(void) {
		leftmotor = InitializeServoMotor(PIN_B6,false);			//left motor
		rightmotor = InitializeServoMotor(PIN_B7,true);			//right motor
}

void initGPIOLineSensor(void) {
    // use 8 I/O pins to initialize a GPIO line sensor
    gls = InitializeGPIOLineSensor(PIN_B5, PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_E0, PIN_C6, PIN_C7);
}

// The 'main' function is the entry point of the program
int main(void) {
		
    // Initialization code can go here
		//initIRSensor(); 
		initMotor();
		initGPIOLineSensor();
		
		while(true){
        // put the values of the line sensor into the 'line' array 
        LineSensorReadArray(gls, line);
				
				if((line[3]>0.5)||(line[4]>0.5)){
						SetMotor(leftmotor, 1);
						SetMotor(rightmotor, 1);
				}
				else if (line[5]>0.5){
						SetMotor(leftmotor, 1);
						SetMotor(rightmotor, .6);
				}
				else if (line[2]>0.5){
						SetMotor(leftmotor, .6);
						SetMotor(rightmotor, 1);
				}
				else if (line[6]>0.5){
						SetMotor(leftmotor, 1);
						SetMotor(rightmotor, .2);
				}
				else if (line[1]>0.5){
						SetMotor(leftmotor, .2);
						SetMotor(rightmotor, 1);
				}				
				else if (line[7]>0.5){
						SetMotor(leftmotor, 1);
						SetMotor(rightmotor, 0);
				}
				else if (line[0]>0.5){
						SetMotor(leftmotor, 0);
						SetMotor(rightmotor, 1);
				}		
		}
}



//////this was our wall following code that does a 30 point turn and
//////i moved it here to use the main section for the line following
//    while (1) {
//      // Runtime code can go here
//      frontSensor = ADCRead(adc[0])*1000;
//			rightSensor = ADCRead(adc[1])*1000;
//			if(frontSensor > 600){
//				SetMotor(servomotor[0], -1);
//				SetMotor(servomotor[1], -.5);
//				i = 1000000;
//				while(i){
//					i -= 1;
//				}
//				
//				
//				SetMotor(servomotor[0], 0);
//				SetMotor(servomotor[1], 1);
//			}
//			else if(rightSensor > 500){
//				SetMotor(servomotor[0], -1);
//				SetMotor(servomotor[1], 0);
//			}
//			else{
//				SetMotor(servomotor[0], 1);
//				SetMotor(servomotor[1], .6);
//			}

//    }
