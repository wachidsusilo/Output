#include "Output.h"

ArrayList<Output*> Output::outputs;
boolean Output::registered = false;
uint32_t Output::uid = 0;

#ifdef ESP32
Output::Output(const uint8_t& pin, const bool& isPwm, const uint8_t& channel)
    : channel(channel),
#else
Output::Output(const uint8_t& pin, const bool& isPwm)
    :
#endif
      pin(pin),
      isPwm(isPwm),
      duty(255),
      maxDuty(255),
      id(0),
      counter(0),
      callback(NULL),
      blinker(BlinkData()),
      patterns(ArrayList<uint32_t>()),
      activeLow() {
    patterns.add(500);
    patterns.add(500);
}

Output::~Output() {
    uid = id;
    outputs.removeIf([](Output* o) -> bool { return o->id == uid; });
}

void Output::write(uint16_t state) {
    if(state > maxDuty) state = maxDuty;
    if (isPwm) {
#ifdef ESP32
        ledcWrite(channel, activeLow ? maxDuty - state : state);
#else
        analogWrite(pin,  activeLow ? maxDuty - state : state);
#endif
    } else {
        digitalWrite(pin, activeLow ? !state : state);
    }
}

void Output::set(const uint16_t& state) {
    if (isBlinking()) return;
    write(state);
}

void Output::setFrequency(const uint16_t& freq) {
    if (isPwm) {
#ifdef ESP32
        this->freq = freq;
        ledcSetup(channel, freq, resolution);
#elif defined(ESP8266)
        analogWriteFreq(pin, freq);
#endif
    }
}

void Output::setResolution(const uint8_t& resolution) {
    if (isPwm) {
        maxDuty = round(pow(2.0, resolution) - 1);
#ifdef ESP32
        this->resolution = resolution;
        ledcSetup(channel, freq, resolution);
#elif defined(ESP8266)
        analogWriteRange(maxDuty);
#endif
    }
}

void Output::setDutyCycle(const uint16_t& dutyCycle) {
    duty = dutyCycle;
}

void Output::begin(const bool& activeLow, const uint16_t& freq, const uint8_t& resolution) {
    this->activeLow = activeLow;
    pinMode(pin, OUTPUT);
    if (isPwm) {
        maxDuty = round(pow(2.0, resolution) - 1);
        duty = maxDuty;
#ifdef ESP32
        ledcAttachPin(pin, channel);
        ledcSetup(channel, freq, resolution);
#elif defined(ESP8266)
        analogWriteFreq(pin, freq);
        analogWriteRange(maxDuty);
#endif
    }
    write(LOW);
    uid = millis();
    while (outputs.contains([](Output* o) -> bool { return o->id == uid; })) uid++;
    if (!outputs.contains([](Output* o) -> bool { return o->id == uid; })) {
        outputs.add(this);
    }
    if (!registered) {
        registered = true;
        Timer.registerEvent(run);
    }
}

void Output::blink(const int& count) {
    if (patterns.size() > 0) {
        blinker.count = count;
        write(patterns[0] > 0 ? duty : LOW);
        blinker.blinking = true;
        blinker.counter = 0;
        blinker.index = 0;
        counter = millis();
    }
}

void Output::stop() {
    if (blinker.blinking) {
        blinker.blinking = false;
        write(LOW);
        if (callback) callback();
    }
}

void Output::onBlinkFinished(Callback callback) {
    this->callback = callback;
}

void Output::pollEvent() {
    if (blinker.blinking && (blinker.counter < blinker.count || blinker.count <= 0)) {
        if (blinker.index % 2 == 0) {
            if (millis() - counter >= patterns[blinker.index]) {
                blinker.index++;
                counter = millis();
                if (patterns[blinker.index] > 0) write(LOW);
            }
        } else {
            if (millis() - counter >= patterns[blinker.index]) {
                blinker.index++;
                counter = millis();
                if (blinker.index >= patterns.size()) {
                    blinker.index = 0;
                    if (blinker.count > 0) blinker.counter++;
                }
                if (blinker.count > 0 && blinker.counter >= blinker.count) {
                    stop();
                } else {
                    if (patterns[blinker.index] > 0) write(duty);
                }
            }
        }
    }
}

bool Output::get() {
    return activeLow ? !digitalRead(pin) : digitalRead(pin);
}

bool Output::isBlinking() {
    return blinker.blinking;
}

void Output::run() {
    outputs.forEach([](Output* o, size_t index) -> bool {
        o->pollEvent();
        return true;
    });
}
