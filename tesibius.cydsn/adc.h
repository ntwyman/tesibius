/* ========================================
 *
 * Copyright Jonny Reckless & Nick Twyman, 2015
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
  THIS CODE IS FOR AUTO DE FET
 * ========================================
*/
#if !defined(ADC_H)
#include <cytypes.h>
#define ADC_H
    
void 
AdcInit();

typedef struct ChannelTarget_ ChannelTarget;
struct ChannelTarget_
{
    int16 channel_num;
    int16 value;
};
#endif // defined(ADC_H)
/* [] END OF FILE */
