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
    Output(const uint8_t& pin = -1);
    ~Output();

    void begin(const bool& activeLow = false);
    void blink(const int& count = 0);
    void stop();
    void onBlinkFinished(Callback callback);
    void pollEvent();

    void set(const bool& state);
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

    uint8_t pin;
    uint32_t id;
    uint32_t counter;
    Callback callback;
    BlinkData blinker;
    ArrayList<uint32_t> patterns;

    boolean activeLow;

    static ArrayList<Output*> outputs;
    static boolean registered;
    static uint32_t uid;
    static void run();
};

#endif
