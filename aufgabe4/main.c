/**
 * @brief
 *      Controlling the chasing lights with interrupts
 *
 *
 * @file main.c
 * @date 18.09.2017
 * @version 0.01
 *
 * @author Dennis Addo   ic16b026@technikum-wien.at
 *
 **/


#include <stdint.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <inc/hw_memmap.h>
#include <driverlib/systick.h>

volatile uint32_t counter = 0;
volatile uint32_t pwmState = 10;
volatile uint32_t percent = 1;

#define LED4 (0x01)



static void BlinkLED(uint32_t D1, uint32_t D2, uint32_t D3, uint32_t D4) {
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, D2);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, D1);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, D4);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, D3);
}



static void usrwitch_handler(void){

    if((GPIOIntStatus(GPIO_PORTJ_BASE, true) & GPIO_PIN_0)){


        if (percent == 4){
            percent = 1;
        }
        switch(percent)
        {
            case 1: pwmState = 25; break;
            case 2: pwmState = 33;break;
            case 3: pwmState = 99;break;
        }
        percent++;
    }

    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_PIN_0);
}



static void handle_interrupts(void) {
    counter++;

    if(pwmState >= 100){
        pwmState = 0;
    }

    if(counter  < pwmState) {
        BlinkLED(0,0,0,LED4); //dimm
    }else {
        BlinkLED(0,0,0,0);  //brighter

    }

    if(counter == 100){
        counter = 0;
    }
   // counter %= 100;

}

int main(void) {

    (void)SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480,120000000);



    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); //for PORTN lED PN0 and PN1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // for PORTF  that is 3rd and 4th LED PF0 PF4
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
           && !(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)));



    //todo:: setting up the userswitch ports
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)));



    /*Other LED PIN are the PF4 and PF0 for the Ethernet connectivity
     * PN0 and PN1 are for user application software use only
     * But you can configure anyone of them for any use software application us
     * */

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);




    //todo: configuring the GPIOPIn for PORTN
    uint32_t strength, pinType;

    GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_1, &strength, &pinType);
    GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_1, strength, GPIO_PIN_TYPE_STD);

    GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_0, &strength, &pinType);
    GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_0, strength, GPIO_PIN_TYPE_STD);

    //todo: For PORTF that is PF0 and PF4

    GPIOPadConfigGet(GPIO_PORTF_BASE, GPIO_PIN_0, &strength, &pinType);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, strength, GPIO_PIN_TYPE_STD);

    GPIOPadConfigGet(GPIO_PORTF_BASE, GPIO_PIN_4, &strength, &pinType);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, strength, GPIO_PIN_TYPE_STD);





    //todo:: userswitch1
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);


    //todo: configure and set pull up for the PJ0

    GPIOPadConfigGet(GPIO_PORTJ_BASE, GPIO_PIN_0, &strength, &pinType);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, strength, GPIO_PIN_TYPE_STD_WPU);



    //todo:: interrupt for the UserSwitch

    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);


    //todo:: register the interrupt handler for the switch
    //todo: add interrupt handler method to the register

    GPIOIntRegister(GPIO_PORTJ_BASE, usrwitch_handler);


    //todo: enable the interrupt
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0);




    //todo: <<<<--------- Setting up the SysTick for PWM from here ----->>>>>>>>>>>

    SysTickPeriodSet(120000000/10000);
    SysTickIntRegister(handle_interrupts);

    SysTickIntEnable();
    SysTickEnable();


    while(1){
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, (1<<4));
    }


}
