#include "Output.h"

ArrayList<Output*> Output::outputs;
boolean Output::registered = false;
uint32_t Output::uid = 0;

Output::Output(const uint8_t& pin)
    : pin(pin), id(0), counter(0), callback(NULL), blinker(BlinkData()), patterns(ArrayList<uint32_t>()), activeLow() {
    patterns.add(500);
    patterns.add(500);
}

Output::~Output() {
    uid = id;
    outputs.removeIf([](Output* o) -> bool { return o->id == uid; });
}

void Output::set(const bool& state) {
    digitalWrite(pin, activeLow ? !state : state);
}

void Output::begin(const bool& activeLow) {
    this->activeLow = activeLow;
    pinMode(pin, OUTPUT);
    set(LOW);
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
        set(patterns[0] > 0);
        blinker.blinking = true;
        blinker.counter = 0;
        blinker.index = 0;
        counter = millis();
    }
}

void Output::stop() {
    blinker.blinking = false;
    set(LOW);
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
                if (patterns[blinker.index] > 0) set(LOW);
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
                    if (callback) callback();
                } else {
                    if (patterns[blinker.index] > 0) set(HIGH);
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
    outputs.forEach([](Output* o) -> void {
        o->pollEvent();
    });
}
