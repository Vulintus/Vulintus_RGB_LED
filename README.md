# Vulintus RGB LED Library

An Arduino-compatible library to simplify control of RGB and RGBW LEDs with support for both discrete LED pins and NeoPixel (WS2812) addressable LEDs.

_Disclaimer: This README file was AI-generated, but checked for accuracy by a human._

## Features

- **Dual LED Support**: Control both discrete RGB/RGBW LEDs (via PWM pins) and NeoPixel addressable LEDs
- **Stimulus Queue System**: Pre-program different color/duration combinations
- **Automatic Timing**: Built-in timer for automatic LED shutoff after specified durations
- **Flexible Color Control**: Set colors using individual RGB/RGBW values or packed 32-bit integers
- **Polarity Control**: Configure active-high or active-low LED control
- **Predefined Colors**: Built-in color constants for common colors (red, yellow, green, cyan, blue, magenta)

## Installation

### Arduino IDE
1. Download this repository as a ZIP file
2. In Arduino IDE, go to **Sketch** → **Include Library** → **Add .ZIP Library**
3. Select the downloaded ZIP file
4. The library will be installed and ready to use

### PlatformIO
Add to your `platformio.ini`:
```ini
lib_deps =
    https://github.com/Vulintus/Vulintus_RGB_LED.git
```

## Dependencies

- [Adafruit_NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) (for NeoPixel support)

## Hardware Compatibility

### Discrete RGB/RGBW LEDs
Connect LED pins to PWM-capable pins on your microcontroller:
- Red, Green, Blue channels (required)
- White channel (optional, for RGBW LEDs)

### NeoPixel LEDs
Connect NeoPixel data line to any digital output pin.

## Usage

### Basic Example - Discrete RGB LED

```cpp
#include <Vulintus_RGB_LED.h>

// Define your LED pins
#define PIN_LED_R  9
#define PIN_LED_G  10
#define PIN_LED_B  11

// Create LED object
Vulintus_RGB_LED led(PIN_LED_R, PIN_LED_G, PIN_LED_B);

void setup() {
  led.begin();                      // Initialize the LED
  led.set_rgb(255, 0, 0);           // Set color to red
  led.set_dur(1000);                // Set duration to 1 second
  led.light_on();                   // Turn on the LED
}

void loop() {
  led.timing_check();               // Check if LED should turn off
}
```

### Example - RGBW LED with White Channel

```cpp
#include <Vulintus_RGB_LED.h>

#define PIN_LED_R  9
#define PIN_LED_G  10
#define PIN_LED_B  11
#define PIN_LED_W  12

// Create RGBW LED object
Vulintus_RGB_LED led(PIN_LED_R, PIN_LED_G, PIN_LED_B, PIN_LED_W);

void setup() {
  led.begin();
  led.set_rgbw(0, 255, 0, 100);     // Green with some white
  led.set_dur(2000);                // 2 second duration
  led.light_on();
}

void loop() {
  led.timing_check();
}
```

### Example - NeoPixel LED

```cpp
#include <Vulintus_RGB_LED.h>
#include <Adafruit_NeoPixel.h>

#define PIN_NEOPIX  13
#define NUM_NEOPIX  1

// Create NeoPixel object
Adafruit_NeoPixel neopix = Adafruit_NeoPixel(NUM_NEOPIX, PIN_NEOPIX);

// Create LED controller for NeoPixel
Vulintus_RGB_LED led(&neopix, NUM_NEOPIX);

void setup() {
  led.begin();
  led.set_rgb(0, 0, 255);           // Set to blue
  led.light_on();
}

void loop() {
  led.timing_check();
}
```

### Example - Stimulus Queue

```cpp
#include <Vulintus_RGB_LED.h>

Vulintus_RGB_LED led(PIN_LED_R, PIN_LED_G, PIN_LED_B);

void setup() {
  led.begin();
  
  // Program multiple color/duration combinations
  led.cur_stim = 0;                 // Select queue slot 0
  led.set_rgb(255, 0, 0);           // Red
  led.set_dur(500);                 // 500ms
  
  led.cur_stim = 1;                 // Select queue slot 1
  led.set_rgb(0, 255, 0);           // Green
  led.set_dur(500);
  
  led.cur_stim = 2;                 // Select queue slot 2
  led.set_rgb(0, 0, 255);           // Blue
  led.set_dur(500);
}

void loop() {
  // Cycle through the queue
  for (uint8_t i = 0; i < 3; i++) {
    led.light_on(i);                // Turn on queue slot i
    while (led.is_on) {
      led.timing_check();           // Wait for auto-shutoff
    }
    delay(200);                     // Pause between flashes
  }
}
```

## API Reference

### Constructors

#### `Vulintus_RGB_LED(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, uint8_t pin_w = 0xFF)`
Create an LED controller for discrete RGB or RGBW LEDs.
- **pin_r**: Red LED pin
- **pin_g**: Green LED pin  
- **pin_b**: Blue LED pin
- **pin_w**: White LED pin (optional, omit or use 0xFF for RGB-only)

#### `Vulintus_RGB_LED(Adafruit_NeoPixel *neopix_ptr, uint8_t num_neopix = 1)`
Create an LED controller for NeoPixel LEDs.
- **neopix_ptr**: Pointer to initialized Adafruit_NeoPixel object
- **num_neopix**: Number of NeoPixels to control (default: 1)

### Public Methods

#### `void begin(void)`
Initialize the LED controller. Must be called in `setup()` before using other functions.

#### `void light_on(void)`
Turn on the LED using the currently selected stimulus from the queue.

#### `void light_on(uint8_t queue_index)`
Turn on the LED using a specific stimulus from the queue.
- **queue_index**: Index (0-5) of the stimulus to display

#### `void light_off(void)`
Immediately turn off the LED.

#### `void timing_check(void)`
Check if the LED should automatically turn off based on the programmed duration. Call this regularly in `loop()`.

#### `void set_rgb(uint8_t red, uint8_t grn, uint8_t blu)`
Set the RGB color values (0-255) for the current stimulus.

#### `void set_rgb(uint32_t rgb)`
Set the RGB color using a packed 32-bit integer.

#### `void set_rgbw(uint8_t red, uint8_t grn, uint8_t blu, uint8_t wht = 0)`
Set the RGBW color values (0-255) for the current stimulus.

#### `void set_rgbw(uint32_t rgbw)`
Set the RGBW color using a packed 32-bit integer.

#### `void set_dur(uint16_t stim_dur)`
Set the duration (in milliseconds) for the current stimulus. Use 65535 for infinite duration.

#### `void set_polarity(bool led_on)`
Set the LED control polarity.
- **true**: High signal turns LED on (default)
- **false**: Low signal turns LED on (inverted)

### Public Variables

#### `uint8_t cur_stim`
Current stimulus queue index (0-5). Set this to select which queue slot to program or display.

#### `bool is_on`
Status flag indicating whether the LED is currently on.

#### `vulintus_rgbw_stim_t queue[LIGHT_QUEUE_SIZE]`
Array of stimulus parameters. Each element contains RGB/RGBW values and duration.

### Predefined Color Constants

```cpp
COLOR_RED       // Index 0
COLOR_YELLOW    // Index 1
COLOR_GREEN     // Index 2
COLOR_CYAN      // Index 3
COLOR_BLUE      // Index 4
COLOR_MAGENTA   // Index 5
```

## Configuration

You can customize these defines before including the library:

```cpp
#define LIGHT_QUEUE_SIZE 6      // Number of stimulus slots (default: 6)
#define STARTUP_STIM_DUR 100    // Default startup duration in ms (default: 100)

#include <Vulintus_RGB_LED.h>
```

## Examples

The library includes two example sketches:
- **Vulintus_RGB_1LED_Test**: Demonstrates single LED control with color cycling
- **Vulintus_RGB_3LED_Test**: Demonstrates control of multiple NeoPixels

Find these in **File** → **Examples** → **Vulintus RGB LED Library** in the Arduino IDE.

## License

Copyright © 2025 Vulintus, Inc. All rights reserved.

See [LICENSE](LICENSE) file for details.

## Support

For questions, issues, or contributions:
- **GitHub**: [https://github.com/Vulintus/Vulintus_RGB_LED](https://github.com/Vulintus/Vulintus_RGB_LED)
- **Email**: info@vulintus.com
- **Website**: [https://www.vulintus.com](https://www.vulintus.com)

## Changelog

### Version 0.1.0 (2025-12-09)
- Initial release
- Separated from Vulintus_OmniTrak library
- Renamed from OmniTrak_RGB_LED to Vulintus_RGB_LED
- Support for discrete RGB/RGBW LEDs and NeoPixels
- Stimulus queue system with automatic timing
