#include "gpio.h"

typedef struct Pin
{
    GPIO_TypeDef *port;
    uint16_t pin;
} Pin_t, *pPin;