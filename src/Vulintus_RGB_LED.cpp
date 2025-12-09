
#include "./Vulintus_RGB_LED.h"


// CLASS FUNCTIONS ***********************************************************// 

// Class constructor (RGBW LED, with white element).
Vulintus_RGB_LED::Vulintus_RGB_LED(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, uint8_t pin_w)
{
    _pin_rgbw[0] = pin_r;               // Set the red pin.
    _pin_rgbw[1] = pin_g;               // Set the green pin.
    _pin_rgbw[2] = pin_b;               // Set the blue pin.
    if (pin_w == 0xFF) {                // If the white pin is not specified...
        _num_chan = 3;                  // Set the number of channels to 3.
    }
    else {                              // Otherwise...
        _pin_rgbw[3] = pin_w;           // Set the white pin.
        _num_chan = 4;                  // Set the number of channels to 4.
    }
}


// Class constructor (NeoPixel).
Vulintus_RGB_LED::Vulintus_RGB_LED(Adafruit_NeoPixel *neopix_ptr, uint8_t num_neopix)
        : _neopix(neopix_ptr), _num_neopix(num_neopix)
{

}


// Initialization.
void Vulintus_RGB_LED::begin(void)
{
    if (_neopix == NULL) {                              // If the NeoPixel pointer is null...
        for (uint8_t i = 0; i < _num_chan; i++) {       // Step through the channels.
            pinMode(_pin_rgbw[i], OUTPUT);              // Set the pin mode to output.
        }    
    }
    else {                                              // Otherwise, if this is a NeoPixel...
        _neopix->begin();                               // Initialize the NeoPixels.
    }
    light_off();                                        // Set all elements to off.

    // Pre-fill the stimulus queue with default values.
    uint8_t rgb[6][3] = {
        {0x0F, 0x00, 0x00},       // Red.
        {0x0F, 0x0F, 0x00},       // Yellow.    
        {0x00, 0x0F, 0x00},       // Green.
        {0x00, 0x0F, 0x0F},       // Cyan. 
        {0x00, 0x00, 0x0F},       // Blue.   
        {0x0F, 0x00, 0x0F},       // Magenta.          
    };                                                  // List a rainbow sequence of colors.   
    uint8_t j = 0;                                      // Current color index.
    for (uint8_t i = 0; i < LIGHT_QUEUE_SIZE; i++) {    // Step through the colors to queue.    
        cur_stim = i;                                   // Set the current light stimulus index.
        set_rgb(rgb[j][0],rgb[j][1],rgb[j][2]);         // Set the color in the queue.
        set_dur(STARTUP_STIM_DUR);                      // Set the startup stimulation duration for each.   
        j++;                                            // Increment the color index.
        j %= sizeof(rgb)/3;                             // Make sure the color index doesn't overflow.
    }
}


// Set the LED control polarity.
void Vulintus_RGB_LED::set_polarity(bool led_on)
{
    _polarity = led_on;                 // Copy the specified polarity to the private variable.
    light_off();                        // Set all elements to off.
}


// Show the currently-selected stimulus.
void Vulintus_RGB_LED::light_on(void)
{
    _set_outputs();
    if (queue[cur_stim].dur < 65535) {
        _light_timer = millis() + queue[cur_stim].dur;
    }
    is_on = true;
}       


// Show the specified stimulus.
void Vulintus_RGB_LED::light_on(uint8_t queue_index)
{
    cur_stim = queue_index;
    light_on();
}


// Turn off any displayed stimulus.
void Vulintus_RGB_LED::light_off(void)
{
    // If the NeoPixel pointer is null...
    if (_neopix == NULL) {                                       
        for (uint8_t i = 0; i < _num_chan; i++) {
            analogWrite(_pin_rgbw[i], _polarity ? 0 : 255);
        }
    }

    // If this is a NeoPixel...
    else {                                 
        for (uint8_t i = 0; i < _num_neopix; i++) {                     
            _neopix->setPixelColor(i,0,0,0,0);
        }
        _neopix->show();
    }
    _light_timer = 0;
    is_on = false;
}     


// Check for queue on/off events.
void Vulintus_RGB_LED::timing_check(void)
{
    if ((_light_timer) && (millis() > _light_timer)) {
        light_off();
    }
}       


// Set the RGB values (individual uint8).
void Vulintus_RGB_LED::set_rgb(uint8_t red, uint8_t grn, uint8_t blu)
{
    set_rgbw(red, grn, blu);
}         


// Set the RGB values (combined uint32).
void Vulintus_RGB_LED::set_rgb(uint32_t rgb)
{
    set_rgbw(rgb);
}         


// Set the RGBW values (individual uint8).
void Vulintus_RGB_LED::set_rgbw(uint8_t red, uint8_t grn, uint8_t blu, uint8_t wht) 
{
    queue[cur_stim].rgbw[0] = red;
    queue[cur_stim].rgbw[1] = grn;
    queue[cur_stim].rgbw[2] = blu;
    queue[cur_stim].rgbw[3] = wht;
    if (is_on) {
        _set_outputs();
    }
}


// Set the RGBW values (individual uint8).
void Vulintus_RGB_LED::set_rgbw(uint32_t rgbw)
{
    for (uint8_t i = 0; i < _num_chan; i++) {        
        queue[cur_stim].rgbw[i] = rgbw >> 8*i;
    }
    if (is_on) {
        _set_outputs();
    }
}


// Set the stimulus duration.
void Vulintus_RGB_LED::set_dur(uint16_t stim_dur)
{
    queue[cur_stim].dur = stim_dur;
}


// Turn on the LED outputs.
void Vulintus_RGB_LED::_set_outputs(void)
{

    // If the NeoPixel pointer is null...
    if (_neopix == NULL) {                                      
        uint8_t pwm_val;                                        
        for (uint8_t i = 0; i < _num_chan; i++) { 
            pwm_val = queue[cur_stim].rgbw[i];
            if (!_polarity) {
                pwm_val = 255 - pwm_val;
            }
            analogWrite(_pin_rgbw[i], pwm_val);
        }
    }

    // If this is a NeoPixel...
    else {                                        
        for (uint8_t i = 0; i < _num_neopix; i++) {
            _neopix->setPixelColor(i, \
                    queue[cur_stim].rgbw[0], \
                    queue[cur_stim].rgbw[1], \
                    queue[cur_stim].rgbw[2], \
                    queue[cur_stim].rgbw[3]);
        }              
        _neopix->show();
    }
    
}