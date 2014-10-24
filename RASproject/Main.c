#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/adc.h>
#include <RASLib/inc/motor.h>

tADC *adc[2];
tMotor *servomotor[2];
void initIRSensor(void) {
    adc[0] = InitializeADC(PIN_D0);
    adc[1] = InitializeADC(PIN_D1);
}
void initMotor(void) {
		servomotor[0] = InitializeServoMotor(PIN_B6,true);
		servomotor[1] = InitializeServoMotor(PIN_B7,true);
}
// The 'main' function is the entry point of the program
int main(void) {
    // Initialization code can go here
		initIRSensor();  	
    while (1) {
        // Runtime code can go here
       float ADCValue0 = ADCRead(adc[0]);
			 float ADCValue1 = ADCRead(adc[1]);
			
    }
}
