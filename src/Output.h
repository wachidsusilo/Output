#ifndef OUTPUT_H
#define OUTPUT_H

#if defined(ESP32) || defined(ESP8266)
#include <functional>
#endif

#include "Arduino.h"
#include "Timer.h"

class Output {
   public:
#if defined(ESP32) || defined(ESP8266)
    using Callback = std::function<void()>;
#else
    using Callback = void (*)();
#endif

#ifdef ESP32
    Output(const uint8_t& pin = -1, const bool& isPwm = false, const uint8_t& channel = 0);
#else
    Output(const uint8_t& pin = -1, const bool& isPwm = false);
#endif
    ~Output();

    void begin(const bool& activeLow = false, const uint16_t& freq = 490, const uint8_t& resolution = 8);
    void blink(const int& count = 0);
    void stop();
    void onBlinkFinished(Callback callback);
    void pollEvent();

    void set(const uint16_t& dutyCycle);
    void setFrequency(const uint16_t& freq);
    void setResolution(const uint8_t& resolution);
    void setDutyCycle(const uint16_t& dutyCycle);
    bool get();
    bool isBlinking();

    template <typename... T>
    void setPattern(T&&... args) {
        uint32_t intervals[] = {args...};
        patterns = ArrayList<uint32_t>(intervals, sizeof...(args));
        if (patterns.size() % 2 != 0) {
            patterns.add(0);
        }
        if (blinker.blinking) {
            blinker.counter = 0;
            blinker.index = 0;
            counter = millis();
        }
    }

   private:
    struct BlinkData {
        uint8_t index;
        int16_t count;
        uint32_t counter;
        boolean blinking;

        BlinkData() : counter(0), count(0), index(0), blinking(false) {}
    };

    bool isPwm;
    uint8_t pin;
    uint16_t duty;
    uint16_t maxDuty;
    uint32_t id;
    uint32_t counter;
    Callback callback;
    BlinkData blinker;
    ArrayList<uint32_t> patterns;

    boolean activeLow;
    void write(uint16_t state);

#ifdef ESP32
    uint8_t channel;
    uint8_t resolution;
    uint16_t freq;
#endif

    static ArrayList<Output*> outputs;
    static boolean registered;
    static uint32_t uid;
    static void run();
};

#endif
