#include "Output.h"

Output led(2);

void onBlinkFinished() {
    Serial.println("Blink finished");
}

void setup() {
    Serial.begin(115200);
    Serial.begin(115200);
    
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

    //Assign callback
    led.onBlinkFinished(onBlinkFinished);

    //Blink the LED
    //Zero (0) means infinite, this is the default value
    led.blink(5);

    //Stop blinking the LED
    //led.stop();
}

void loop() {
    Timer.run();
}
