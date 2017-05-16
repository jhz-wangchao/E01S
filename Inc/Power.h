#ifndef __POWER_H__
#define __POWER_H__
#ifdef __cplusplus

#define POWER_KEY_DETECT		PinCode(PORTB, GPIO_PIN_13)
#define POWER_KEY_SOS			PinCode(PORTB, GPIO_PIN_14)
#define POWER_ON_CONTROL		PinCode(PORTB, GPIO_PIN_15)
#define POWER_OFF_CONTROL		PinCode(PORTD, GPIO_PIN_10)


void Power_Initilize(void);
unsigned char Power_KeyDetect(void);
unsigned char Power_SosDetect(void);
void Power_Off(void);
#endif
#endif
