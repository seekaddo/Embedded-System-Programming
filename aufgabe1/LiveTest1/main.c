/**
 * @brief
 *      Blinking the LED based on even and odd number.
 *      That is switching the bits between the D2 and D1 LED
 *      on the board. This is done using direct register access.
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
#include <stdio.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <inc/hw_memmap.h>




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
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);



	//configuring the GPIOPIn for PORTN
	uint32_t strength, pinType;

	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_1, &strength, &pinType);
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_1, strength, GPIO_PIN_TYPE_STD);

	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_0, &strength, &pinType);
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_0, strength, GPIO_PIN_TYPE_STD);

	//For PORTF that is PF0 and PF4

	GPIOPadConfigGet(GPIO_PORTF_BASE, GPIO_PIN_0, &strength, &pinType);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, strength, GPIO_PIN_TYPE_STD);

	GPIOPadConfigGet(GPIO_PORTF_BASE, GPIO_PIN_4, &strength, &pinType);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, strength, GPIO_PIN_TYPE_STD);


	//configure and set pull up for the PJ0
	GPIOPadConfigGet(GPIO_PORTJ_BASE, GPIO_PIN_0, &strength, &pinType);
	GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, strength, GPIO_PIN_TYPE_STD_WPU);




	uint32_t idex = 1;
	uint32_t bits = 1;

	//Cannot SysCtlClockGet() be called on TM4C129 use the return value from SysCtlClockFreqSet()
	//printf("System clock now is: %u \n",g_ui32SysClock);

	while (1) {

		//LED rotate on button press on USR_SW1


		if ((GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) & GPIO_PIN_0) == 0x0) {
			SysCtlDelay(2000000);
			switch (idex) {
				case 1:
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, bits <<= 2 );
					GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, (bits >>= 1));
					break;
				case 2:
					GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, bits <<= 1);
					GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, bits >>= 2);
					break;
				case 3:
					GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, bits <<= 1);
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, bits <<= 3);
					break;
				case 4:
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, bits >>= 2);
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, bits >>= 2);
					idex %= 4;
					break;
				default:
					break;
			}
			++idex;

		}
		SysCtlDelay(4000);
	}


}
