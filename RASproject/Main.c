#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/adc.h>
#include <RASLib/inc/motor.h>

tADC *adc[2];
tMotor *servomotor[2];
float frontSensor;
float rightSensor;
int i;



void initIRSensor(void) {
    adc[0] = InitializeADC(PIN_D0);
    adc[1] = InitializeADC(PIN_D1);
}
void initMotor(void) {
		servomotor[0] = InitializeServoMotor(PIN_B6,false);			//left motor
		servomotor[1] = InitializeServoMotor(PIN_B7,true);			//right motor
}
// The 'main' function is the entry point of the program
int main(void) {
    // Initialization code can go here
		initIRSensor(); 
		initMotor();
    while (1) {
      // Runtime code can go here
      frontSensor = ADCRead(adc[0])*1000;
			rightSensor = ADCRead(adc[1])*1000;
			if(frontSensor > 600){
				SetMotor(servomotor[0], -1);
				SetMotor(servomotor[1], -.5);
				i = 1000000;
				while(i){
					i -= 1;
				}
				
				
				SetMotor(servomotor[0], 0);
				SetMotor(servomotor[1], 1);
			}
			else if(rightSensor > 500){
				SetMotor(servomotor[0], -1);
				SetMotor(servomotor[1], 0);
			}
			else{
				SetMotor(servomotor[0], 1);
				SetMotor(servomotor[1], .6);
			}

    }
}
