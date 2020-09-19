/* ========================================
 *
 * Copyright Jonny Reckless & Nick Twyman, 2015-2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#if !defined(ANIMATION_H)
#include <cytypes.h>
#define ANIMATION_H
    
typedef enum Animation_
{
    SetChannel
} Animation;

void
AnimationInit();

void
AnimationJiffy(uint32 jiffy_count);

void
RunAnimation(Animation animation );
#endif // !defined(ANIMATION_H)
/* [] END OF FILE */
