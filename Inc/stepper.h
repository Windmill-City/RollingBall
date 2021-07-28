#ifndef __STEPPER_H__
#define __STEPPER_H__
#include <stdint.h>
#include "Base.h"

void stepperinit(void);
void stepper(int i);

#endif

Pin_t pins [4];


void stepper(int i)
{
    switch (i)
    {
    case 0:
        HAL_GPIO_WritePin(pins[0].port, pins[0].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(pins[1].port, pins[1].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[2].port, pins[2].pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[3].port, pins[3].pin,GPIO_PIN_RESET);
        break;
 
    case 1:
        HAL_GPIO_WritePin(pins[0].port, pins[0].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(pins[1].port, pins[1].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(pins[2].port, pins[2].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[3].port, pins[3].pin,GPIO_PIN_RESET);
        break;
      
    case 2:
        HAL_GPIO_WritePin(pins[0].port, pins[0].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[1].port, pins[1].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(pins[2].port, pins[2].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[3].port, pins[3].pin, GPIO_PIN_RESET);
        break;
       
    case 3:
        HAL_GPIO_WritePin(pins[0].port, pins[0].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[1].port, pins[1].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(pins[2].port, pins[2].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(pins[3].port, pins[3].pin,GPIO_PIN_RESET);
        break;
   
    case 4:
        HAL_GPIO_WritePin(pins[0].port, pins[0].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[1].port, pins[1].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[2].port, pins[2].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(pins[3].port, pins[3].pin, GPIO_PIN_RESET);
        break;
     
    case 5:
        HAL_GPIO_WritePin(pins[0].port, pins[0].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[1].port, pins[1].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[2].port, pins[2].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(pins[3].port, pins[3].pin, GPIO_PIN_SET);
        break;
        
    case 6:
        HAL_GPIO_WritePin(pins[0].port, pins[0].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[1].port, pins[1].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[2].port, pins[2].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[3].port, pins[3].pin,GPIO_PIN_SET);
        break;

    case 7:
        HAL_GPIO_WritePin(pins[0].port, pins[0].pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(pins[1].port, pins[1].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[2].port, pins[2].pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pins[3].port, pins[3].pin, GPIO_PIN_SET);
        break;
    default:;
    }
}
