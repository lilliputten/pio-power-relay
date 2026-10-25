#ifndef HwButton_DEFINED
#define HwButton_DEFINED

// #define HwButton_DEBUG

#include "TFTUtils.hpp"
#include "TimeUtils.hpp"

// GPIO0 on standard ESP32 (check your board for GPIO9 on C3/C6/H2)
static const uint8_t bootButtonPin = 0;

// Button presses are considered sequential if they occurred with a delay of
// less than this value (ms)
static const TTick sequentialDelay = 1500;

static const TTick lastStateActualPeriod = 1000;

// Boot button states (LOW when pressed, HIGH when released)
#define POWER_BUTTON_PRESSED LOW
#define POWER_BUTTON_RELEASED HIGH

class HwButton {
 private:
  TFTUtils* tftUtils = nullptr;

  int lastState = POWER_BUTTON_RELEASED;
  TTick lastPressed = 0;
  TTick lastStateChange = 0;
  size_t sequentialPresses = 0;

 public:
  void nextTick() {
    const TTick ticks = millis();

    // Read button state (LOW when pressed, HIGH when released)
    const int state = digitalRead(bootButtonPin);
    const bool isChanged = state != this->lastState;
    const bool isPressed = state == POWER_BUTTON_PRESSED;

    const auto lastPressDiff = ticks - this->lastPressed;

    if (isChanged) {
      if (isPressed) {
        if (lastPressDiff < sequentialDelay) {
          this->sequentialPresses++;
        } else if (this->sequentialPresses) {
          this->sequentialPresses = 0;
        }

#ifdef HwButton_DEBUG
        Serial.printf("[HwButton] Pressed (lastPressDiff=%d)\n", lastPressDiff);
        if (this->sequentialPresses) {
          Serial.printf("[HwButton] sequentialPresses=%d\n",
                        this->sequentialPresses);
        }
#endif
        this->lastPressed = ticks;
      }

      this->lastState = state;
      this->lastStateChange = ticks;
    }
  }

  const size_t getSequentialPresses() const { return this->sequentialPresses; }
  const TTick getPressed() const {
    return this->lastState == POWER_BUTTON_PRESSED ||
                   millis() - this->lastStateChange < lastStateActualPeriod
               ? this->lastPressed
               : 0;
  }

  void initHwButton() {
    if (tftUtils) tftUtils->writeLine("Starting Hardware Button...");

    pinMode(bootButtonPin, INPUT_PULLUP);  // Use internal pull-up

    this->lastState = digitalRead(bootButtonPin);
    // this->lastStateChange = millis();
  }

  // Constructor
  HwButton(TFTUtils* tftUtils = nullptr) {
    if (tftUtils) {
      this->tftUtils = tftUtils;
    }
  }
};

#endif  // HwButton_DEFINED
