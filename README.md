## Output Library for Arduino
A wrapper to control IO pin as `OUTPUT`. This library rely on [Timer](https://github.com/wachidsusilo/Timer) and [ArrayList](https://github.com/wachidsusilo/ArrayList) libraries. You can download or clone it from my repository.

## Example
The blinking pattern can be set through `setPattern` method. The default value is 500 ms ON and 500 ms OFF.
```c++
#include "Output.h"

Output led(2);

void setup() {
    //true means active low
    //false means active high
    led.begin(true);
    
    //Turn on the LED
    led.set(true);

    delay(2000);

    //Turn off the LED
    led.set(false);

    //Get the state of the pin
    //If the mode is active high, this method will return true if the output is HIGH
    //If the mode is active low, this method will return true if the output is LOW
    bool state = led.get();

    //Set the pattern of blink
    //100ms ON, 100ms OFF, 100ms ON, 700ms OFF
    led.setPattern(100, 100, 100, 700);

    //Set the blink count
    //Zero (0) means infinite, this is the default value
    led.setBlinkCount(5);

    //Blink the LED
    led.blink();

    //Stop blinking the LED
    led.stop();
}

void loop() {
    Timer.run();
}
```