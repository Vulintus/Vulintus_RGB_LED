/*  Vulintus_RGB_3LED_Test.ino

    copyright Vulintus, Inc., 2023

    Example demonstrating functions in the OmniTrak RGB LED class for devices
    with 3 LEDs.

    UPDATE LOG:
      2024-02-20 - Drew Sloan - Example first created.
      2024-06-30 - Drew Sloan - Renamed from "OmniTrak_RGB_LED_Test.ino" to 
                                "OmniTrak_RGB_3LED_Test.ino".

*/


// Included libraries. // 
#include <Vulintus_OmniTrak.h>                  // Vulintus OmniTrak catch-all library.

// Pin assignments. //
#ifndef PIN_NEOPIX
  #define PIN_NEOPIX  13                        // NeoPixel output.
#endif

// LED control.//
#ifndef NUM_NEOPIX                              // If the neopixel count isn't set in the board definition...
  #define NUM_NEOPIX  3                         // Number of neopixels.
#endif
Adafruit_NeoPixel neopix = 
    Adafruit_NeoPixel(NUM_NEOPIX, PIN_NEOPIX);  // Adafruit Neopixel class.
Vulintus_RGB_LED cue_led[] = {
  Vulintus_RGB_LED(&neopix, 0),
  Vulintus_RGB_LED(&neopix, 1),
  Vulintus_RGB_LED(&neopix, 2),
};                                              // Array of RGB LED class objects.
uint16_t stim_dur = 100;                        // Set the stimulus duration.
uint16_t stim_pause = 100;                      // Set the stimulus duration.


// INITIALIZATION ************************************************************// 
void setup() {

  // Initialize the LEDs.
  uint8_t rgb[3];                                     // Create a list of RGB values.
  for (uint8_t i = 0; i < NUM_NEOPIX; i++) {          // Step through each NeoPixel.
    cue_led[i].begin();                               // Initialize the neopixel.
    for (uint8_t j = 0; j < LIGHT_QUEUE_SIZE; j++) {  // Step through the queue size.
      cue_led[i].cur_stim = j;                        // Set the stimulus index.
      memset(rgb,0,3);                                // Clear the RGB values.
      rgb[j % 3] = 255;                               // Set one value to 255.
      cue_led[i].set_rgb(rgb[0],rgb[1],rgb[2]);       // Set the RGB values.
      cue_led[i].set_dur(stim_dur);                   // Set the stimulus duration.
    }
  }
}


// MAIN LOOP *****************************************************************// 
void loop() {

  for (uint8_t j = 0; j < LIGHT_QUEUE_SIZE; j++) {    // Step through the queue size.
    for (uint8_t i = 0; i < NUM_NEOPIX; i++) {        // Step through each NeoPixel.
      cue_led[i].light_on(j);                         // Turn on each light.
      while (cue_led[i].is_on) {                      // Loop until the LED turns off.
        cue_led[i].timing_check();                    // Run an LED timing check.
      }
      delay(stim_pause);                              // Pause in between flashes.
    }
  }

}