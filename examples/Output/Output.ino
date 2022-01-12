#include "Output.h"

Output led(13);

//true means PWM
Output pwm(9, true);

void onBlinkFinished() {
    Serial.println("Blink finished");
}

void setup() {
    Serial.begin(115200);
    
    //true means active low
    //false means active high
    led.begin(true);
    pwm.begin(true);
    
    //Turn on the LED
    led.set(true);
    pwm.set(255);

    delay(2000);

    //Turn off the LED
    led.set(false);
    pwm.set(0);

    //Get the state of the pin
    //If the mode is active high, this method will return true if the output is HIGH
    //If the mode is active low, this method will return true if the output is LOW
    bool state = led.get();

    //Set the pattern of blink
    //100ms ON, 100ms OFF, 100ms ON, 700ms OFF
    led.setPattern(100, 100, 100, 700);
    pwm.setPattern(100, 100, 100, 700);

    //Set the duty cycle of the PWM when the output is blinking
    pwm.setDutyCycle(50);

    //Assign callback
    led.onBlinkFinished(onBlinkFinished);

    //Blink the LED
    //Zero (0) means infinite, this is the default value
    led.blink(5);
    pwm.blink(5);

    //Stop blinking the LED
    //led.stop();
}

void loop() {
    Timer.run();
}
