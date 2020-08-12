#include "button.hpp"
#include "config.hpp"
#include "dimmer.hpp"
// #include "encoder.hpp"
#include <Arduino.h>
#include <Embedis.h>

void onRotate(int8_t);
void onClick();

Button button;
unsigned long secondElapsed = millis();
unsigned long fakeFreq = millis();
Embedis embedis(Serial);

void setup() {
    delay(2000);
    Serial.begin(115200);
    Serial.println("\nBooting");
    Serial.println(US_TO_RTC_TIMER_TICKS(65));

    dimmer::setup();
    // encoder::setup(ENCODER_PINA, ENCODER_PINB, onRotate);
    button.setup(ButtonType::pulldown, BUTTON_PIN, onClick);

    Serial.println("Ready");


    // Add pinMode command to mirror Arduino's
    Embedis::command( F("dim"), [](Embedis* e) {
        if (e->argc != 3) return e->response(Embedis::ARGS_ERROR);
        int pin = String(e->argv[1]).toInt();
        int val = String(e->argv[2]).toInt();
        onRotate(val);
        // String argv3(e->argv[2]);
        // argv3.toUpperCase();
        // int mode;
        // if (argv3 == "INPUT") mode = INPUT;
        // else if (argv3 == "OUTPUT") mode = OUTPUT;
        // else if (argv3 == "INPUT_PULLUP") mode = INPUT_PULLUP;
        // else return e->response(Embedis::ARGS_ERROR);
        // pinMode(pin, mode);
        // e->response(Embedis::OK);
    });

    Embedis::command( F("toggle"), [](Embedis* e) {
        // if (e->argc != 3) return e->response(Embedis::ARGS_ERROR);
        // int pin = String(e->argv[1]).toInt();
        // int val = String(e->argv[2]).toInt();
        onClick();
        // String argv3(e->argv[2]);
        // argv3.toUpperCase();
        // int mode;
        // if (argv3 == "INPUT") mode = INPUT;
        // else if (argv3 == "OUTPUT") mode = OUTPUT;
        // else if (argv3 == "INPUT_PULLUP") mode = INPUT_PULLUP;
        // else return e->response(Embedis::ARGS_ERROR);
        // pinMode(pin, mode);
        // e->response(Embedis::OK);
    });
}

void loop() {
    // encoder::handle();
    button.handle();
    // if (millis() - fakeFreq >= 10) {
    //     fakeFreq = millis(); // reset timer
    //     dimmer::zeroCrossed();
    // }
    if (millis() - secondElapsed >= 1000) {
        secondElapsed = millis(); // reset timer
        Serial.printf("power: %s; brightness: %d; cross count: %d; low count: %d; timer interrupt count: %d; max "
                      "interrupt count: %d\n",
            dimmer::powerOn ? "on" : "off", dimmer::brightness, dimmer::debugCrossCount, dimmer::debugLowCount,
            dimmer::debugTimerInterrupt, dimmer::maxInterruptCount);
        dimmer::debugCrossCount = 0;
        dimmer::debugLowCount = 0;
        dimmer::debugTimerInterrupt = 0;
        dimmer::maxInterruptCount = 0;
    }
    embedis.process();
}

void onRotate(int8_t position) {
    dimmer::setBrightness(position * 1);
    // Serial.println(position);
}

void onClick() { dimmer::togglePower(); }
