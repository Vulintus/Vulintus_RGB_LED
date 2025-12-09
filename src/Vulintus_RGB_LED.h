/*  
    Vulintus_RGB_LED.h

    copyright (c) 2025, Vulintus, Inc. All rights reserved.

    Control library for RGB and RGBW LEDs with support for various LED types,
    color manipulation, brightness control, and effects.

    UPDATE LOG:
        2024-02-20 - Drew Sloan - Example first created.
        2025-12-09 - Drew Sloan - Library moved from Vulintus_OmniTrak to 
                                  separate repository and renamed to 
                                  "Vulintus_RGB_LED".

*/


#ifndef _VULINTUS_RGB_LED_H_
#define _VULINTUS_RGB_LED_H_

#include <Arduino.h>                    // Standard Arduino header.


// Adafruit Neopixel library.
// When included in an umbrella library with submodule dependencies, load this library from the submodule.
#if __has_include(<dependencies/Adafruit_NeoPixel/Adafruit_NeoPixel.h>)
    #if defined(VULINTUS_PREPROCESSOR_MESSAGES)                     
        #pragma message("Adafruit_NeoPixel.h -> using 'Adafruit_NeoPixel' submodule.")
    #endif
    #include "../../Adafruit_NeoPixel/Adafruit_NeoPixel.h"
#else    
    #if defined(VULINTUS_PREPROCESSOR_MESSAGES)    
        #pragma message("Adafruit_NeoPixel.h -> using 'Adafruit_NeoPixel' from sketchbook.")
    #endif
    #include <Adafruit_NeoPixel.h>
#endif


// DEFINITIONS *******************************************************************************************************//

#ifndef LIGHT_QUEUE_SIZE
    #define LIGHT_QUEUE_SIZE 6
#endif
#ifndef STARTUP_STIM_DUR
    #define STARTUP_STIM_DUR 100
#endif

typedef struct {
    uint8_t rgbw[4];
    uint16_t dur;
} vulintus_rgbw_stim_t;     // Structure for holding RGB/RGBW stimulus parameters.

enum : uint8_t {
    COLOR_RED = 0,
    COLOR_YELLOW = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_BLUE = 4,
    COLOR_MAGENTA = 5,
};                          // Predefined color indices.


// CLASSES ***********************************************************************************************************//

class Vulintus_RGB_LED {

    public:

        // Constructors. //
        Vulintus_RGB_LED(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, uint8_t pin_w = 0xFF);
        Vulintus_RGB_LED(Adafruit_NeoPixel *neopix_ptr, uint8_t num_neopix = 1);

        // Public Variables. //
        uint8_t cur_stim = 0;                           // Currently-queued stimulus index.
        vulintus_rgbw_stim_t queue[LIGHT_QUEUE_SIZE];   // Array of stimulus parameters.
        bool is_on;                                     // Flag to indicate when the LED is on.

        // Public Functions. //
		void begin(void); 			                    // Initialization.
        void set_polarity(bool led_on);                 // Set the LED control polarity.

        void light_on(void);                            // Show the currently-selected stimulus.
        void light_on(uint8_t queue_index);             // Show the specified stimulus.
        void light_off(void);                           // Turn off any displayed stimulus.
        void timing_check(void);                        // Check for queue on/off events.

        void set_rgb(uint8_t red, uint8_t blu, uint8_t grn);                    // Set the RGB values (individual uint8).
        void set_rgb(uint32_t rgb);                                             // Set the RGB values (combined uint32).
        void set_rgbw(uint8_t red, uint8_t blu, uint8_t grn, uint8_t wht = 0);  // Set the RGBW values (individual uint8).
        void set_rgbw(uint32_t rgbw);                                           // Set the RGBW values (combined uint32).
        void set_dur(uint16_t stim_dur);                                        // Set the stimulus duration.

    private:

        // Private Variables. //
        Adafruit_NeoPixel *_neopix = NULL;  // Neopixel control object.
        uint8_t _num_neopix;                // Number of Neopixels to control.
        uint8_t _pin_rgbw[4];               // Red, Green, Blue, and White LED pins.
        uint8_t _num_chan;                  // Number of channels (3 for RGB, 4 for RGBW).
        bool _polarity = 1;                 // Control polarity (default is high turns on LED).
        uint32_t _light_timer;              // Light off timer (milliseconds).

        // Private Functions. //
        void _set_outputs(void);            // Turn on the LED outputs.

 };


 #endif          // #ifndef _VULINTUS_RGB_LED_H_