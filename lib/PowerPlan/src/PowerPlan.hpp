#ifndef PowerPlan_DEFINED
#define PowerPlan_DEFINED

#include <ArduinoJson.h>

#include <vector>

#include "DataFiles.hpp"
#include "TFTUtils.hpp"
#include "TimeUtils.hpp"

#define CHANNEL_ON HIGH
#define CHANNEL_OFF LOW
#define INITIAL_PLAN_STATE false

#define TTimeSec uint16_t

struct PowerPlanSlot {
  TTimeSec timeSec;  // 2b
  String textId;     // 16b
};

#define TPowerPlanSlots std::vector<PowerPlanSlot>

static const PowerPlanSlot defaultSlots[] = {
    {10, "Test"},
    {10 * 60 * 60 + 3 * 60 + 20, "Second slot"},
};

class PowerPlan {
 private:
  TFT_eSPI* tft = nullptr;
  uint8_t id = 0;
  uint8_t channel = 0;

  // Ticking loop
  unsigned int loops = 0;
  bool state = INITIAL_PLAN_STATE;
  size_t currentSlot = 0;
  unsigned long ticksStarted = 0;

  // List of slot items
  TPowerPlanSlots slots;

  const String getFileName() const {
    return "/power-plan-" + String(this->id) + ".txt";
  }

  void freeSlots() { this->slots.clear(); }

  bool readRawData(T2DStrings& rawData) const {
    const char* filename = this->getFileName().c_str();
    return DataFiles::loadDataFile(filename, rawData);
  }

  bool writeRawData(const T2DStrings& rawData) const {
    const char* filename = this->getFileName().c_str();
    Serial.printf("[PowerPlan:writeRawData] Writing file '%s'\n", filename);
    return DataFiles::writeDataFile(filename, rawData);
  }

  void loadDefaultSlots() {
    const size_t itemSize = sizeof(PowerPlanSlot);
    const size_t defaultSlotsSize = sizeof(defaultSlots);
    const size_t defaultSlotsItemSize = sizeof(defaultSlots[0]);
    const size_t defaultSlotsCount = defaultSlotsSize / defaultSlotsItemSize;

    /* // DEBUG
     * Serial.printf("defaultSlotsCount: %d\n", defaultSlotsCount);
     * Serial.printf("itemSize: %d\n", itemSize);                          // 20
     * Serial.printf("defaultSlotsItemSize: %d\n", defaultSlotsItemSize);  // 20
     * Serial.printf("defaultSlotsSize: %d\n", defaultSlotsSize);
     */

    // Free previously allocated memory
    this->freeSlots();

    // Allocate memory
    slots.reserve(defaultSlotsCount);

    // Copy the entire memory block cleanly in one step
    for (size_t i = 0; i < defaultSlotsCount; i++) {
      // NOTE: Do deep-copy, including the String member
      slots.push_back(defaultSlots[i]);
    }
  }

  static PowerPlanSlot strings2Slot(const TStrings& rawItem, int idx = -1) {
    const size_t rawItemSize = rawItem.size();
    PowerPlanSlot slot;
    slot.timeSec = rawItemSize >= 0 ? rawItem[0].toInt() : 10;
    slot.textId = rawItemSize >= 1 ? rawItem[1]
                                   : (idx != -1 ? "Slot " + String(idx) : "");
    // Serial.printf("XXX %d/%d: %d '%s'\n", idx, rawItemSize, slot.timeSec,
    // slot.textId);
    return slot;
  }

  static TStrings slot2strings(const PowerPlanSlot& slot) {
    TStrings rawItem = {String(slot.timeSec), slot.textId};
    return rawItem;
  }

  void loadSlots() {
    T2DStrings rawData;
    const bool success = this->readRawData(rawData);
    if (!success) {
      this->loadDefaultSlots();
      return;
    }

    const size_t count = rawData.size();

    // Free previously allocated memory
    this->freeSlots();

    // Allocate memory
    slots.reserve(count);

    // Copy the entire memory block cleanly in one step
    for (size_t i = 0; i < count; i++) {
      PowerPlanSlot slot = PowerPlan::strings2Slot(rawData[i], i + 1);
      // NOTE: Do deep-copy, including the String member
      slots.push_back(slot);
    }
  }

  bool writeSlots() const {
    const size_t slotsCount = this->slots.size();
    T2DStrings rawData;

    rawData.reserve(slotsCount);

    for (size_t i = 0; i < slotsCount; i++) {
      const PowerPlanSlot& slot = this->slots[i];
      TStrings rawItem = PowerPlan::slot2strings(slot);
      // NOTE: Do deep-copy, including the String member
      rawData.push_back(rawItem);
    }

    return this->writeRawData(rawData);
  }

  void updateChannelState() const {
    uint8_t signal = this->state ? CHANNEL_ON : CHANNEL_OFF;
    Serial.printf(
        "PowerPlan updateChannelState: slot #%d channel=%d state=%s "
        "signal=%d\n",
        this->currentSlot, this->channel, this->state ? "ON" : "OFF", signal);
    digitalWrite(this->channel, signal);
  }

  void resetTickingLoop() {
    this->ticksStarted = millis();
    this->currentSlot = 0;
    this->state = INITIAL_PLAN_STATE;
    this->updateChannelState();
  }

 public:
  // Display current status on a tft screen
  int showInfo(int yp) {
    auto tft = this->tft;
    if (!tft) return 0;

    int xp = 10;
    int font1 = FONT_SM;
    int fh = tft->fontHeight(font1);

    tft->setTextColor(TFT_CYAN, TFT_BLACK);
    tft->setTextSize(1);
    tft->setCursor(xp, yp, font1);

    char charBuf[50];

    const PowerPlanSlot& slot = this->slots[this->currentSlot];

    const unsigned long ticks = millis();
    const uint16_t diffSec = (ticks - this->ticksStarted) / 1000;

    snprintf(charBuf, sizeof(charBuf), "Pwr %d %s %d/%d %d/%ds", this->id,
             this->state ? "+" : "-", this->currentSlot + 1, this->slots.size(),
             diffSec, slot.timeSec);

    Serial.println(charBuf);

    tft->print(charBuf);
    // Place extra spaces to cleanup the previous output text
    tft->println("    ");

    return fh;
  }

  void nextTick() {
    const PowerPlanSlot& slot = this->slots[this->currentSlot];
    const unsigned long ticks = millis();
    if (!this->ticksStarted) {
      this->ticksStarted = ticks;
    }
    const uint16_t diffSec = (ticks - this->ticksStarted) / 1000;
    const bool goToNext = diffSec >= slot.timeSec;

    // Serial.printf("PowerPlan tick: slot #%d (%s) secs=%d/%d state=%s\n",
    // this->currentSlot, slot.textId, diffSec, slot.timeSec, this->state ?
    // "ON": "OFF");

    if (goToNext) {
      if (++this->currentSlot >= this->slots.size()) {
        // Case 1: The plan program has finished, start again
        this->resetTickingLoop();
        this->loops++;
      } else {
        // Case 2: Ok, just go to the next slot
        this->ticksStarted = ticks;
        this->state = !this->state;
        this->updateChannelState();
      }
    }
  }

  String getSlotsJson() {
    JsonDocument doc;
    doc["ok"] = true;
    doc["id"] = this->id;
    JsonArray arr = doc["slots"].to<JsonArray>();
    for (const PowerPlanSlot& slot : slots) {
      JsonObject obj = arr.add<JsonObject>();
      obj["timeSec"] = slot.timeSec;
      obj["textId"] = slot.textId;
    }
    String out;
    serializeJson(doc, out);
    return out;
  }

  String updateSlotsFromJsonArray(const JsonArray& arr) {
    if (arr.isNull()) {
      return "ERR_MISSING_SLOTS";
    }

    // Clear existing slots before rewriting them
    this->slots.clear();

    // TODO: Reserve expected slots amount
    this->slots.reserve(arr.size());

    Serial.printf("Reading %d slots from json\n", arr.size());

    // Parse and populate the vector
    size_t rowNo = 0;
    for (JsonObject obj : arr) {
      // Check if the required keys exist in the current object
      if (!obj["timeSec"].is<TTimeSec>() || !obj["textId"].is<const char*>()) {
        return "ERR_INVALID_SLOT_FORMAT";
      }

      PowerPlanSlot slot;
      slot.timeSec = obj["timeSec"];
      slot.textId = obj["textId"].as<String>();
      Serial.printf("%d: %d (%s)\n", ++rowNo, slot.timeSec, slot.textId);

      this->slots.push_back(slot);
    }

    // Store updated data
    if (!this->writeSlots()) {
      return "ERR_FAILED_WRITING_SLOTS_FILE";
    };

    // TODO: Update the cuurently looping data (somehow)

    return "";
  }

  String updateSlotsFromJson(const String& jsonStr) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);

    // Return specific parsing error code from ArduinoJson
    if (error) {
      Serial.printf("Error json parsing: %s\n", error.c_str());
      return "ERR_JSON_PARSE";
    }

    // Validate the presence of the slots array
    JsonArray arr = doc["slots"].as<JsonArray>();

    return this->updateSlotsFromJsonArray(arr);
  }

  void initPowerPlan() {
    auto tft = this->tft;

    Serial.printf("-- Power Plan %d Initialization started --", this->id);
    if (tft) tft->printf("Power plan %d\n", this->id);

    this->loadSlots();

    // DEBUG
    int rowNo = 0;
    Serial.printf("Loaded %d slots:\n", this->slots.size());
    if (tft) tft->printf("Loaded %d slots:\n", this->slots.size());
    for (size_t i = 0; i < this->slots.size(); i++) {
      const PowerPlanSlot& slot = this->slots[i];
      char charBuf[50];
      String timeStr = TimeUtils::getSecPeriodString(slot.timeSec, 5);
      if (slot.timeSec >= 60) {
        timeStr += " (" + String(slot.timeSec) + "s)";
      }
      snprintf(charBuf, sizeof(charBuf), "%d: %s %s", ++rowNo, timeStr.c_str(),
               slot.textId ? "(" + slot.textId + ")" : "--");
      Serial.print(charBuf);
      Serial.println();
      if (tft) {
        tft->print(charBuf);
        tft->println();
      }
    }

    this->resetTickingLoop();

    Serial.println("-- Power Plan Initialization done --");
  }

  // Constructor
  PowerPlan(uint8_t id, uint8_t channel, TFTUtils* tftUtils = nullptr) {
    if (tftUtils) {
      this->tft = &(tftUtils->tft);
    }
    this->id = id;
    this->channel = channel;
  }

  // Destructor
  ~PowerPlan() {
    // Free previously allocated memory
    this->freeSlots();
  }

  size_t slotsCount() const { return this->slots.size(); }
  size_t slotsTimeSec() const {
    const size_t count = this->slots.size();
    TTimeSec summ = 0;
    for (size_t i = 0; i < count; i++) {
      summ += this->slots[i].timeSec;
    }
    return summ;
  }
};

#endif
