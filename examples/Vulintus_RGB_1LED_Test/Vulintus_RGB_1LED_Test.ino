/*  Vulintus_RGB_3LED_Test.ino

    copyright Vulintus, Inc., 2023

    Example demonstrating functions in the OmniTrak RGB LED class for devices
    with 3 LEDs.

    UPDATE LOG:
      2024-02-20 - Drew Sloan - Example first created.
      2024-06-30 - Drew Sloan - Renamed from "OmniTrak_RGB_LED_Test.ino" to 
                                "OmniTrak_RGB_3LED_Test.ino".
      2024-11-19 - Drew Sloan - Added an explicitly indexed color set.

*/


// Included libraries. // 
#include <Vulintus_OmniTrak.h>                  // Vulintus OmniTrak catch-all library.

// Light queue. //
#ifndef LIGHT_QUEUE_SIZE
  #define LIGHT_QUEUE_SIZE  6
#endif

// LED control.// 
#if defined(PIN_NEOPIX)                                                   // If there's a neopixel pin...
  Adafruit_NeoPixel neopix = Adafruit_NeoPixel(NUM_NEOPIX, PIN_NEOPIX);   // Adafruit Neopixel class.
  Vulintus_RGB_LED led(&neopix, NUM_NEOPIX);                              // Create the Vulintus RGB LED class with a neopixel.
#elif defined(PIN_LED_W)                                                  // If there's a white LED pin...
  Vulintus_RGB_LED led(PIN_LED_R, PIN_LED_G, PIN_LED_B, PIN_LED_W);       // Assume there's also RGB pins.
#else                                                                     // Otherwise, for basic RGB LEDs...
  Vulintus_RGB_LED led(PIN_LED_R, PIN_LED_G, PIN_LED_B);                  // Use only the RGB pins.
#endif
enum : uint8_t {
  COLOR_RED = 0,
  COLOR_YELLOW = 1,
  COLOR_GREEN = 2,
  COLOR_CYAN = 3,
  COLOR_BLUE = 4,
  COLOR_MAGENTA = 5,
};                                                                        // Create indices for colors.

const uint16_t STIM_DUR = 250;      // Set the stimulus duration.
const uint16_t STIM_PAUSE = 250;    // Set the stimulus duration.
const uint8_t PWM_VAL = 50;         // PWM value for all RGBW channels.


// INITIALIZATION ************************************************************// 
void setup() {

  // Initialize the LEDs.
  led.begin();                  // Initialize the LED class.
  uint8_t rgb[6][3] = {
    {PWM_VAL, 0x00, 0x00},      // Red.
    {PWM_VAL, PWM_VAL, 0x00},   // Yellow. 
    {0x00, PWM_VAL, 0x00},      // Green.
    {0x00, PWM_VAL, PWM_VAL},   // Cyan. 
    {0x00, 0x00, PWM_VAL},      // Blue.   
    {PWM_VAL, 0x00, PWM_VAL},   // Magenta.    
  };                            // List a rainbow sequence of colors.   
  uint8_t j = 0;                                      // Current color index.
  for (uint8_t i = 0; i < LIGHT_QUEUE_SIZE; i++) {    // Step through the colors to queue.    
    led.cur_stim = i;                                 // Set the current light stimulus index.
    led.set_rgb(rgb[j][0],rgb[j][1],rgb[j][2]);       // Set the color in the queue.
    led.set_dur(STIM_DUR);                            // Set the startup stimulation duration for each.   
    j++;                                              // Increment the color index.
    j %= sizeof(rgb)/3;                               // Make sure the color index doesn't overflow.
  }
}


// MAIN LOOP *****************************************************************// 
void loop() {

  for (uint8_t i = 0; i < LIGHT_QUEUE_SIZE; i++) {    // Step through the queue size.
    led.light_on(i);                                  // Turn on each light.
    while (led.is_on) {                               // Loop until the LED turns off.
      led.timing_check();                             // Run an LED timing check.
    }
    delay(STIM_PAUSE);                                // Pause in between flashes.
  }
  delay(1000);                                        // Pause for 1 second.
}