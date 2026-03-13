/*  
    Vulintus_RGB_LED.cpp

    copyright (c) 2025, Vulintus, Inc.

    See "Vulintus_RGB_LED.h" for documentation and change log.

*/


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
        set_rgb(i, rgb[j][0],rgb[j][1],rgb[j][2]);      // Set the color in the queue.
        set_dur(i, STARTUP_STIM_DUR);                   // Set the startup stimulation duration for each.   
        j++;                                            // Increment the color index.
        j %= sizeof(rgb)/3;                             // Make sure the color index doesn't overflow.
    }

    // Set default values for the heartbeat.
    heartbeat.rgbw_high[0] = 0xFF;       // Red high value (all other colors are off by default).
    heartbeat.steps = 5;                 // Number of steps in the heartbeat effect.
    heartbeat.period = 25;               // Time between steps in the heartbeat effect (milliseconds).
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
    _set_outputs(queue[cur_stim].rgbw);
    if (queue[cur_stim].dur < 65535) {
        _light_timer = millis() + queue[cur_stim].dur;
    }
    is_on = true;
    _heartbeat_on = false;
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
    _heartbeat_on = false;
    is_on = false;
}     


// Start the heartbeat effect (individual RGBW uint8_t values).
void Vulintus_RGB_LED::heartbeat_start(uint8_t red_lo, uint8_t grn_lo, uint8_t blu_lo, 
                    uint8_t red_hi, uint8_t grn_hi, uint8_t blu_hi)
{
    heartbeat.rgbw_low[0] = red_lo;
    heartbeat.rgbw_low[1] = grn_lo;
    heartbeat.rgbw_low[2] = blu_lo;
    heartbeat.rgbw_low[3] = 0;
    heartbeat.rgbw_high[0] = red_hi;
    heartbeat.rgbw_high[1] = grn_hi;
    heartbeat.rgbw_high[2] = blu_hi;
    heartbeat.rgbw_high[3] = 0;
    heartbeat_start();
}


// Start the heartbeat effect (individual RGBW uint8_t values, assuming [0, 0, 0] for low value).
void Vulintus_RGB_LED::heartbeat_start(uint8_t red_hi, uint8_t grn_hi, uint8_t blu_hi)
{
    heartbeat.rgbw_low[0] = 0;
    heartbeat.rgbw_low[1] = 0;
    heartbeat.rgbw_low[2] = 0;
    heartbeat.rgbw_low[3] = 0;
    heartbeat.rgbw_high[0] = red_hi;
    heartbeat.rgbw_high[1] = grn_hi;
    heartbeat.rgbw_high[2] = blu_hi;
    heartbeat.rgbw_high[3] = 0;
    heartbeat_start();
}


// Start the heartbeat effect (individual RGBW uint8_t values).
void Vulintus_RGB_LED::heartbeat_start(uint8_t red_lo, uint8_t grn_lo, uint8_t blu_lo, uint8_t wht_lo, 
                    uint8_t red_hi, uint8_t grn_hi, uint8_t blu_hi, uint8_t wht_hi)
{
    heartbeat.rgbw_low[0] = red_lo;
    heartbeat.rgbw_low[1] = grn_lo;
    heartbeat.rgbw_low[2] = blu_lo;
    heartbeat.rgbw_low[3] = wht_lo;
    heartbeat.rgbw_high[0] = red_hi;
    heartbeat.rgbw_high[1] = grn_hi;
    heartbeat.rgbw_high[2] = blu_hi;
    heartbeat.rgbw_high[3] = wht_hi;
    heartbeat_start();
}


// Start the heartbeat effect (individual RGBW uint8_t values, assuming [0, 0, 0, 0] for low value).
void Vulintus_RGB_LED::heartbeat_start(uint8_t red_hi, uint8_t grn_hi, uint8_t blu_hi, uint8_t wht_hi)
{
    heartbeat.rgbw_low[0] = 0;
    heartbeat.rgbw_low[1] = 0;
    heartbeat.rgbw_low[2] = 0;
    heartbeat.rgbw_low[3] = 0;
    heartbeat.rgbw_high[0] = red_hi;
    heartbeat.rgbw_high[1] = grn_hi;
    heartbeat.rgbw_high[2] = blu_hi;
    heartbeat.rgbw_high[3] = wht_hi;
    heartbeat_start();
}


// Start the heartbeat effect (combined uint32 values).
void Vulintus_RGB_LED::heartbeat_start(uint32_t rgbw_lo, uint32_t rgbw_hi) 
{
    if (rgbw_hi == 0) {
        rgbw_hi = rgbw_lo;
        rgbw_lo = 0;
    }
    for (uint8_t i = 0; i < 3; i++) {
        heartbeat.rgbw_low[i] = (rgbw_lo >> (8 * (2-i))) & 0xFF;
        heartbeat.rgbw_high[i] = (rgbw_hi >> (8 * (2-i))) & 0xFF;
    }
    heartbeat.rgbw_low[3] = (rgbw_lo >> 24) & 0xFF;
    heartbeat.rgbw_high[3] = (rgbw_hi >> 24) & 0xFF;
    heartbeat_start();
}


// Start the heartbeat effect (use current heartbeat parameters).
void Vulintus_RGB_LED::heartbeat_start(void)
{
    int16_t max_diff = 0;
    for (uint8_t i = 0; i < 4; i++) {
        _heartbeat_values[i] = heartbeat.rgbw_low[i];
        int16_t diff = abs((int16_t)heartbeat.rgbw_high[i] - (int16_t)heartbeat.rgbw_low[i]);
        max_diff = max(max_diff, diff);
    }
    if (heartbeat.steps > max_diff) {
        heartbeat.steps = max_diff;
    }
    _heartbeat_increasing = true;
    _heartbeat_step = 0;
    _light_timer = millis() + heartbeat.period;    
    _heartbeat_on = true;
    is_on = true;    
}



// Check for queue on/off events.
bool Vulintus_RGB_LED::timing_check(void)
{
    if (!is_on) {
        return false;
    }

    if (_heartbeat_on) {
        if (millis() > _light_timer) {        
            for (uint8_t i = 0; i < 4; i++) {
                int16_t step_size = ((int16_t) heartbeat.rgbw_high[i] - heartbeat.rgbw_low[i]) / heartbeat.steps;            
                int16_t new_val;
                if (_heartbeat_increasing) {
                    new_val = (int16_t) _heartbeat_values[i] + step_size;
                }
                else {
                    new_val = (int16_t) _heartbeat_values[i] - step_size;
                }
                new_val = constrain(new_val, min(heartbeat.rgbw_low[i], heartbeat.rgbw_high[i]), max(heartbeat.rgbw_low[i], heartbeat.rgbw_high[i]));
                _heartbeat_values[i] = (uint8_t) new_val;  
            }
            _heartbeat_step++;
            if (_heartbeat_step >= heartbeat.steps) {
                _heartbeat_step = 0;
                _heartbeat_increasing = !_heartbeat_increasing;
            }
            _set_outputs(_heartbeat_values);
            _light_timer += heartbeat.period;
        }
    }
    else if ((_light_timer) && (millis() > _light_timer)) {
        light_off();
        return false;
    }

    return true;
}       


// Set the RGB values (specified stimulus index, individual uint8 values.).
void Vulintus_RGB_LED::set_rgb(uint8_t stim_i, uint8_t red, uint8_t grn, uint8_t blu)
{
    cur_stim = stim_i;
    set_rgbw(red, grn, blu, 0);
}        


// Set the RGB values (current stimulus index, individual uint8 values.).
void Vulintus_RGB_LED::set_rgb(uint8_t red, uint8_t grn, uint8_t blu)
{
    set_rgbw(red, grn, blu, 0);
}         


// Set the RGB values (specified stimulus index, combined uint32).
void Vulintus_RGB_LED::set_rgb(uint8_t stim_i, uint32_t rgb)
{
    cur_stim = stim_i;
    set_rgbw(rgb);
}   


// Set the RGB values (current stimulus index, combined uint32).
void Vulintus_RGB_LED::set_rgb(uint32_t rgb)
{
    set_rgbw(rgb);
}         


// Set the RGBW values (specified stimulus index, individual uint8).
void Vulintus_RGB_LED::set_rgbw(uint8_t stim_i, uint8_t red, uint8_t grn, uint8_t blu, uint8_t wht) 
{
    cur_stim = stim_i;
    queue[cur_stim].rgbw[0] = red;
    queue[cur_stim].rgbw[1] = grn;
    queue[cur_stim].rgbw[2] = blu;
    queue[cur_stim].rgbw[3] = wht;
    if (is_on) {
        _set_outputs(queue[cur_stim].rgbw);
    }
}


// Set the RGBW values (current stimulus index, individual uint8).
void Vulintus_RGB_LED::set_rgbw(uint8_t red, uint8_t grn, uint8_t blu, uint8_t wht) 
{
    queue[cur_stim].rgbw[0] = red;
    queue[cur_stim].rgbw[1] = grn;
    queue[cur_stim].rgbw[2] = blu;
    queue[cur_stim].rgbw[3] = wht;
    if (is_on) {
        _set_outputs(queue[cur_stim].rgbw);
    }
}


// Set the RGBW values (specified stimulus index, combined uint32).
void Vulintus_RGB_LED::set_rgbw(uint8_t stim_i, uint32_t rgbw)
{
    cur_stim = stim_i;
    set_rgbw(rgbw);
}


// Set the RGBW values (current stimulus index, combined uint32).
void Vulintus_RGB_LED::set_rgbw(uint32_t rgbw)
{
    for (uint8_t i = 0; i < 3; i++) {        
        queue[cur_stim].rgbw[i] = (rgbw >> 8*(2-i)) & 0xFF;
    }
    queue[cur_stim].rgbw[3] = (rgbw >> 24) & 0xFF;
    if (is_on) {
        _set_outputs(queue[cur_stim].rgbw);
    }
}


// Set the stimulus duration (specified stimulus index).
void Vulintus_RGB_LED::set_dur(uint8_t stim_i, uint16_t stim_dur)
{
    cur_stim = stim_i;
    queue[cur_stim].dur = stim_dur;
}


// Set the stimulus duration (current stimulus index).
void Vulintus_RGB_LED::set_dur(uint16_t stim_dur)
{
    queue[cur_stim].dur = stim_dur;
}


// Turn on the LED outputs.
void Vulintus_RGB_LED::_set_outputs(uint8_t rgbw[])
{

    // If the NeoPixel pointer is null...
    if (_neopix == NULL) {                                      
        uint8_t pwm_val;                                        
        for (uint8_t i = 0; i < _num_chan; i++) { 
            pwm_val = rgbw[i];
            if (!_polarity) {
                pwm_val = 255 - pwm_val;
            }
            analogWrite(_pin_rgbw[i], pwm_val);
        }
    }

    // If this is a NeoPixel...
    else {                                        
        for (uint8_t i = 0; i < _num_neopix; i++) {
            _neopix->setPixelColor(i, rgbw[0], rgbw[1], rgbw[2], rgbw[3]);
        }              
        _neopix->show();
    }
    
}