//////////////////////////////////////////////////////////////////////////////
// Module: RGBRingLed
// Description: Basic routines to control brightness and color of LED's
//////////////////////////////////////////////////////////////////////////////

#include "RGBRingTimer.h"

void InitLED(void);
void set_led_red(uint8_t led, uint8_t red);
void set_led_green(uint8_t led, uint8_t green);
void set_led_blue(uint8_t led, uint8_t blue);
void set_all_rgb(uint8_t red, uint8_t green, uint8_t blue);
void set_led_rgb(uint8_t led, uint8_t red, uint8_t green, uint8_t blue);
void set_led_unicolor(uint8_t led, uint8_t rgb, uint8_t var);
void set_all_unicolor(uint8_t rgb, uint8_t var);

/* macro definitions for LED control */ 
#define ALLLEDBLACK()  set_all_rgb( 0,0,0)
#define ALLLEDRED()  set_all_rgb( __max_brightness,0,0)
#define ALLLEDYELLO() set_all_rgb( __max_brightness, __max_brightness,0)
#define ALLLEDGREEN()  set_all_rgb( 0,__max_brightness,0)
#define ALLLEDTURQUOISE()  set_all_rgb( 0,__max_brightness, __max_brightness)
#define ALLLEDBLUE()  set_all_rgb( 0,0,__max_brightness)
#define ALLLEDFUCHSIA()  set_all_rgb( __max_brightness,0,__max_brightness)
#define ALLLEDWHITE()  set_all_rgb( __max_brightness,__max_brightness,__max_brightness)
