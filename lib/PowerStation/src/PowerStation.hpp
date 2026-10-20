#ifndef PowerStation_DEFINED
#define PowerStation_DEFINED

#include <ArduinoJson.h>

#include <vector>

#include "PowerPlan.hpp"
#include "constants.hpp"

#ifndef POWER_CHANNELS_COUNT
#error "Constant POWER_CHANNELS_COUNT must be defined!"
#else

/** List of channel pin numbers. Each channel should correspond its own channel
 * no in the defined constant `RELAY_CHx`. */
static const uint8_t channels[] = {
#ifdef RELAY_CH1
    // The 1st (required) channel
    RELAY_CH1
#else
#error "Undefined RELAY_CH1 constant"
#endif
#ifdef RELAY_CH2
    ,
    RELAY_CH2
#elif POWER_CHANNELS_COUNT >= 2
#error "Undefined RELAY_CH2 constant"
#endif
};

static const size_t channelsCount = sizeof(channels) / sizeof(channels[0]);

#define TPowerPlans std::vector<PowerPlan>

class PowerStation {
 private:
  bool inited = false;

  TFTUtils* tftUtils = nullptr;  // NOTE: Kept only for `PowerPlan` constructor

  // List of plan items
  TPowerPlans plans;

  void freePlans() { this->plans.clear(); }

 public:
  // Display current status on a tft screen
  int showInfo(int yp) const {
    if (!this->inited) return 0;

    auto tft = this->tftUtils ? &tftUtils->tft : nullptr;
    if (!tft) return 0;

    int count = 0;
    for (size_t i = 0; i < POWER_CHANNELS_COUNT; i++) {
      count += this->plans[i].showInfo(yp + count);
    }

    return count;
  }

  void nextTick() {
    if (!this->inited) return;
    for (size_t i = 0; i < POWER_CHANNELS_COUNT; i++) {
      this->plans[i].nextTick();
    }
  }

  PowerPlan* findPowerPlanById(TPowerPlanId id) {
    for (size_t i = 0; i < POWER_CHANNELS_COUNT; i++) {
      if (this->plans[i].getId() == id) {
        return &this->plans[i];
      }
    }
    return nullptr;
  }

  bool initPowerStation() {
    if (tftUtils) tftUtils->writeLine("Starting Power Station...");

    // DEBUG:
    Serial.printf("[PowerStation]: Used channels (POWER_CHANNELS_COUNT): %d\n",
                  POWER_CHANNELS_COUNT);
    Serial.printf("[PowerStation]: Defined channels count: %d\n",
                  channelsCount);

    // Insufficient amount of defined channels?
    if (channelsCount < POWER_CHANNELS_COUNT) {
      const String errCode = "Insufficient amount of defined channels";
      const String errStr = errCode + " (" + String(channelsCount) +
                            "), whilst required at least " +
                            String(POWER_CHANNELS_COUNT) +
                            " (define neccessary RELAY_CHx constants).\n";
      Serial.print(RED + errStr + RESET);
      if (tftUtils) tftUtils->writeLine(errCode, true);
      return false;
    }

    // Free previously allocated memory
    this->freePlans();

    // Allocate memory
    this->plans.reserve(POWER_CHANNELS_COUNT);

    // Copy the entire memory block cleanly in one step
    for (size_t i = 0; i < POWER_CHANNELS_COUNT; i++) {
      PowerPlan plan = PowerPlan(i + 1, channels[i], this->tftUtils);
      plan.initPowerPlan();
      // NOTE: Do deep-copy, including the String member
      this->plans.push_back(plan);
    }

    this->inited = true;
    return true;
  }

  // Constructor
  PowerStation(TFTUtils* tftUtils = nullptr) {
    if (tftUtils) {
      this->tftUtils = tftUtils;
    }
  }

  // Destructor
  ~PowerStation() {
    // Free previously allocated memory
    this->freePlans();
  }
};

#endif  // POWER_CHANNELS_COUNT isn't defined
#endif  // PowerStation_DEFINED
