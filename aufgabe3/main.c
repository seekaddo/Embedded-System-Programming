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


//todo: volatile global variables here

volatile uint32_t bPressedSw1 = 0x02;
volatile uint32_t bPressedSw2 = 0x0;


static void BlinkLED(uint32_t D1, uint32_t D2, uint32_t D3, uint32_t D4) {
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, D2);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, D1);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, D4);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, D3);
}


static void handle_interrupts(void) {

    if((GPIOIntStatus(GPIO_PORTJ_BASE, true) & GPIO_PIN_1)){
        bPressedSw1 = 0x02;
        bPressedSw2 = 0x0;
    }

    if((GPIOIntStatus(GPIO_PORTJ_BASE, true) & GPIO_PIN_0)){
        bPressedSw2 = 0x01;
        bPressedSw1 = 0x0;
    }

    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_PIN_1);
    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_PIN_0);

}


int main(void) {


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); //for PORTN lED PN0 and PN1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // for PORTF  that is 3rd and 4th LED PF0 PF4
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
           && !(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)));

    //enable the peripheral for PJ0 userswicth0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)));


    /*Other LED PIN are the PF4 and PF0 for the Ethernet connectivity
     * PN0 and PN1 are for user application software use only
     * But you can configure anyone of them for any use software application us
     * */

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);


    //configure it as input
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);



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


    //todo: configure and set pull up for the PJ0 PJ1
    GPIOPadConfigGet(GPIO_PORTJ_BASE, GPIO_PIN_0, &strength, &pinType);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, strength, GPIO_PIN_TYPE_STD_WPU);

    GPIOPadConfigGet(GPIO_PORTJ_BASE, GPIO_PIN_1, &strength, &pinType);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_1, strength, GPIO_PIN_TYPE_STD_WPU);


    //todo: <<<<--------- Setting up the interrupts from here ----->>>>>>>>>>>

    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);
    //GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_1, GPIO_FALLING_EDGE);


    //todo: add interrupt handler method to the register

    GPIOIntRegister(GPIO_PORTJ_BASE, handle_interrupts);


    //todo: enable the interrupt
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);


    int idex = 0x01;



    while (1) {

        SysCtlDelay(2000000);
        switch (idex) {
            case 1:
                BlinkLED(0, 0, 1 << 4, 0x01);
                break;
            case 2:

                BlinkLED(0, 0x01, 1 << 4, 0x0);
                break;
            case 3:
                BlinkLED(0x02, 0x01, 0, 0);
                break;
            case 4:
                BlinkLED(0x02, 0, 0, 0x01);
                break;
            default:
                break;
        }



        if (bPressedSw1 == 0x02) {
            ++idex;
        } else if (bPressedSw2 == 0x01) {
            --idex;
        }

        if (idex < 0) {
            idex = 4;
        }
        idex %= 5;



        //SysCtlDelay(4000);
    }


}
