/**
 * @brief
 *      Controlling the chasing lights with interrupts
 *
 *
 * @file main.c
 * @date 9.10.2017
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

short dirtion = 0;

short idex = 0x01;

#define LED1 (0x02)
#define LED2 (0x01)
#define LED3 (1 << 4)
#define LED4 (0x01)

static void BlinkLED(uint32_t D1, uint32_t D2, uint32_t D3, uint32_t D4) {
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, D2);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, D1);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, D4);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, D3);
}


static void usrwitch_handler(void){

    if((GPIOIntStatus(GPIO_PORTJ_BASE, true) & GPIO_PIN_0)){
        dirtion ^= 0x01;
    }

    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_PIN_0);
}



static void handle_interrupts(void) {

    switch (idex) {
        case 1:
            BlinkLED(0, 0, LED3, LED4);
            break;
        case 2:

            BlinkLED(0, LED2, LED3, 0);
            break;
        case 3:
            BlinkLED(LED1, LED2, 0, 0);
            break;
        case 4:
            BlinkLED(LED1, 0, 0,  LED4 );
            break;
        default:
            break;
    }


    if (dirtion == 0) {
        ++idex;
    } else if (dirtion == 1) {
        --idex;
    }

    if(idex < 0){
        idex = 4;
    }

    idex %= 5;

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

    SysTickPeriodSet(8000000); //16m, 8m, 10m
    SysTickIntRegister(handle_interrupts);

    SysTickIntEnable();
    SysTickEnable();


    while(1);


}
