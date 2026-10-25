#ifndef PowerPlan_DEFINED
#define PowerPlan_DEFINED

// #define PowerPlan_DEBUG

#include <ArduinoJson.h>

#include <vector>

#include "DataFiles.hpp"
#include "TFTUtils.hpp"
#include "TimeUtils.hpp"
#include "constants.hpp"

#define CHANNEL_ON HIGH
#define CHANNEL_OFF LOW
#define INITIAL_PLAN_STATE false

#define TTimeSec uint16_t

#define TPowerPlanId uint8_t
#define TChannelNo uint8_t

struct PowerPlanSlot {
  TTimeSec timeSec;
  String textId;
};

struct PowerPlanStatus {
  TPowerPlanId id;  // this->id

  TChannelNo channel;       // this->channel
  unsigned int loopsCount;  // this->loops
  bool initalState;         // this->initalState
  bool state;               // this->state
  TTick ticksDiff;          // this->ticks - this->ticksStarted
  TTick ticks;              // this->ticks
  TTick ticksStarted;       // this->ticksStarted

  // unsigned int totalSlotsTimeSec;  // this->getTotalSlotsTimeSec()

  size_t slotsCount;     // this->slots.size()
  size_t slot;           // this->currentSlot
  TTimeSec slotTimeSec;  // slot.timeSec
  String slotTextId;     // slot.textId
};

#define TPowerPlanSlots std::vector<PowerPlanSlot>

static const PowerPlanSlot defaultSlots[] = {
    {10, "Test"}, {5, "5s"},
    // {2 * 60 * 60 + 3 * 60 + 20, "2h 3m 20s"},
};

class PowerPlan {
 private:
  TFTUtils* tftUtils = nullptr;
  TPowerPlanId id = 0;
  TChannelNo channel = 0;

  // Ticking loop
  unsigned int loops = 0;
  bool initalState = INITIAL_PLAN_STATE;
  bool state = INITIAL_PLAN_STATE;
  size_t currentSlot = 0;
  TTick ticks = 0;
  TTick ticksStarted = 0;

  // List of slot items
  TPowerPlanSlots slots;

  const String getFileName() const {
    return "/power-plan-" + String(this->id) + ".txt";
  }

  void freeSlots() { this->slots.clear(); }

  bool readRawData(T2DStrings& rawData) const {
    const String filename = this->getFileName();
    return DataFiles::loadDataFile(filename.c_str(), rawData);
  }

  bool writeRawData(const T2DStrings& rawData) const {
    const String filename = this->getFileName();
    Serial.printf("[PowerPlan:writeRawData] Writing file '%s'\n",
                  filename.c_str());
    return DataFiles::writeDataFile(filename.c_str(), rawData);
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
    this->slots.reserve(defaultSlotsCount);

    // Copy the entire memory block cleanly in one step
    for (size_t i = 0; i < defaultSlotsCount; i++) {
      // NOTE: Do deep-copy, including the String member
      this->slots.push_back(defaultSlots[i]);
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

  bool loadSlots() {
    T2DStrings rawData;
    const bool success = this->readRawData(rawData);
    if (!success) {
      this->loadDefaultSlots();
      return false;
    } else {
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

    return success;
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
    TChannelNo signal = this->state ? CHANNEL_ON : CHANNEL_OFF;
#ifdef PowerPlan_DEBUG
    Serial.printf(
        "PowerPlan updateChannelState: plan %d slot #%d channel=%d state=%s "
        "signal=%d\n",
        this->id, this->currentSlot, this->channel, this->state ? "ON" : "OFF",
        signal);
#endif
    digitalWrite(this->channel, signal);
  }

  void resetTickingLoop() {
    this->ticksStarted = millis();
    this->ticks = this->ticksStarted;
    this->currentSlot = 0;
    this->state = this->initalState;
    this->updateChannelState();
  }

 public:
  TPowerPlanId getId() const { return this->id; }

  // Display current status on a tft screen
  int showInfo(int yp) const {
    auto tft = this->tftUtils ? &tftUtils->tft : nullptr;
    if (!tft) return 0;

    int xp = 10;
    int font1 = FONT_SM;
    int fh = tft->fontHeight(font1);

    tft->setTextColor(TFT_CYAN, TFT_BLACK);
    tft->setTextSize(1);
    tft->setCursor(xp, yp, font1);

    const PowerPlanSlot& slot = this->slots[this->currentSlot];

    // this->ticks = millis();
    const uint16_t diffSec = (this->ticks - this->ticksStarted) / 1000;

    char charBuf[50];
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
    this->ticks = millis();
    if (!this->ticksStarted) {
      this->ticksStarted = this->ticks;
    }
    const uint16_t diffSec = (this->ticks - this->ticksStarted) / 1000;
    const bool goToNext = diffSec >= slot.timeSec;

    const size_t slotsCount = this->slots.size();

#ifdef PowerPlan_DEBUG
    Serial.printf(
        "PowerPlan %d tick: slot #%d/%d (%s) secs=%d/%d state=%s next=%s\n",
        this->id, this->currentSlot, slotsCount, slot.textId, diffSec,
        slot.timeSec, this->state ? "ON" : "OFF", goToNext ? "yes" : "no");
#endif

    if (goToNext) {
      if (++this->currentSlot >= slotsCount) {
        // Case 1: The plan program has finished, start again
        this->resetTickingLoop();
        this->loops++;
      } else {
        // Case 2: Ok, just go to the next slot
        this->ticksStarted = this->ticks;
        this->state = !this->state;
        this->updateChannelState();
      }
    }
  }

  JsonDocument getStatusJson() const {
    JsonDocument doc;
    doc["id"] = this->id;

    doc["channel"] = this->channel;          // 0
    doc["loopsCount"] = this->loops;         // 0
    doc["initalState"] = this->initalState;  // INITIAL_PLAN_STATE
    doc["state"] = this->state;              // INITIAL_PLAN_STATE
    doc["ticksDiff"] = this->ticks - this->ticksStarted;
    doc["ticks"] = this->ticks;                // 0
    doc["ticksStarted"] = this->ticksStarted;  // 0

    // doc["totalSlotsTimeSec"] = this->getTotalSlotsTimeSec();

    doc["slotsCount"] = this->slots.size();
    doc["slot"] = this->currentSlot;  // 0
    const PowerPlanSlot& slot = this->slots[this->currentSlot];
    doc["slotTimeSec"] = slot.timeSec;
    doc["slotTextId"] = slot.textId;

    return doc;
  }

  PowerPlanStatus getStatus() const {
    PowerPlanStatus status;
    status.id = this->id;

    status.channel = this->channel;          // 0
    status.loopsCount = this->loops;         // 0
    status.initalState = this->initalState;  // INITIAL_PLAN_STATE
    status.state = this->state;              // INITIAL_PLAN_STATE
    status.ticksDiff = this->ticks - this->ticksStarted;
    status.ticks = this->ticks;                // 0
    status.ticksStarted = this->ticksStarted;  // 0

    // status.totalSlotsTimeSec = this->getTotalSlotsTimeSec();

    status.slotsCount = this->slots.size();
    status.slot = this->currentSlot;  // 0
    const PowerPlanSlot& slot = this->slots[this->currentSlot];
    status.slotTimeSec = slot.timeSec;
    status.slotTextId = slot.textId;

    return status;
  }

  unsigned int getTotalSlotsTimeSec() const {
    unsigned int totalTimeSec = 0;
    for (const PowerPlanSlot& slot : this->slots) {
      totalTimeSec += slot.timeSec;
    }
    return totalTimeSec;
  }

  JsonDocument getSlotsJson() const {
    JsonDocument doc;
    doc["id"] = this->id;
    unsigned int totalTimeSec;
    JsonArray arr = doc["slots"].to<JsonArray>();
    for (const PowerPlanSlot& slot : this->slots) {
      totalTimeSec += slot.timeSec;
      JsonObject obj = arr.add<JsonObject>();
      obj["timeSec"] = slot.timeSec;
      obj["textId"] = slot.textId;
    }
    // doc["totalSlotsTimeSec"] = totalTimeSec;
    // doc["totalSlotsTimeSec"] = this->getTotalSlotsTimeSec();
    return doc;
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

    return "OK";
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
    if (tftUtils)
      tftUtils->writeLine("Starting Power Plan " + String(this->id) + "...");

    this->loadSlots();

    // DEBUG
    int rowNo = 0;
    // Serial.printf("Loaded %d slots:\n", this->slots.size());
    if (tftUtils)
      tftUtils->writeLine("Loaded " + String(this->slots.size()) + " slots:");
    for (size_t i = 0; i < this->slots.size(); i++) {
      const PowerPlanSlot& slot = this->slots[i];
      char charBuf[50];
      String timeStr = TimeUtils::getSecPeriodString(slot.timeSec, 5);
      if (slot.timeSec >= 60) {
        timeStr += " (" + String(slot.timeSec) + "s)";
      }
      snprintf(charBuf, sizeof(charBuf), "%d: %s %s", ++rowNo, timeStr.c_str(),
               slot.textId ? "(" + slot.textId + ")" : "--");
      if (tftUtils) tftUtils->writeLine(String(charBuf));
    }

    if (this->slots.size() <= 1) {
      Serial.println(RED
                     "Only slots amount more than 1 could be effective." RESET);
    }

    this->resetTickingLoop();
  }

  // Constructor
  PowerPlan(TPowerPlanId id, TChannelNo channel, TFTUtils* tftUtils = nullptr) {
    if (tftUtils) {
      this->tftUtils = tftUtils;
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

#endif  // PowerPlan_DEFINED
