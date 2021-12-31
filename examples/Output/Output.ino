#include "Output.h"

Output led(6);

void setup() {
    Serial.begin(115200);
    led.begin(true);
    led.blink();
}

void loop() {
    Timer.run();
    if (Serial.available()) {
        String str = Serial.readString();
        if (str.startsWith("c")) {
            led.blink(str.substring(1).toInt());
        } else if (str.equals("b")) {
            led.blink();
        } else if (str.equals("s")) {
            led.stop();
        } else {
            led.setPattern(str.substring(0, str.indexOf(",")).toInt(), str.substring(str.indexOf(",") + 1).toInt());
        }
    }
}
