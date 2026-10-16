#ifndef TimeUtils_DEFINED
#define TimeUtils_DEFINED

// #define TimeUtils_DEBUG

#include <Arduino.h>

#include <vector>

#define TTimeSec uint16_t

struct TTimeOfPeriodStep {
  TTimeSec range;  // Eg: 60, 60, 24, 7, 30
  String name;     // Eg: sec, min, hr, day, week, month
};

struct TTimeOfPeriodResult {
  TTimeSec time;
  TTimeSec period;
  String name;  // Eg: s, m, h, d, etc
};

struct TTimePeriod {
  TTimeSec time;
  String name;  // Eg: s, m, h, d, etc
};
#define TTimePeriods std::vector<TTimePeriod>

static const TTimeOfPeriodStep timeOfPeriodSteps[] = {
    {0, "s"},   // seconds
    {60, "m"},  // minutes
    {60, "h"},  // hours
    {24, "d"},  // days
                // {7, "w"},  // weeks
};
static const size_t timeOfPeriodStepsCount =
    sizeof(timeOfPeriodSteps) / sizeof(TTimeOfPeriodStep);

class TimeUtils {
 public:
  static TTimePeriods getSecPeriodSteps(TTimeSec periodSec) {
    size_t step = 0;
    TTimePeriods periods;
    TTimeSec period = periodSec;
    String name = timeOfPeriodSteps[step].name;
    while (step < timeOfPeriodStepsCount) {
      step++;
      const auto range = timeOfPeriodSteps[step].range;
      if (!range) {
        continue;
      }
      const TTimeSec value = period % range;
      const TTimeSec nextPeriod = period / range;
#ifdef TimeUtils_DEBUG
      // DEBUG:
      printf(
          "[TimeUtils] Step %d (%s): period=%d range=%d value=%d "
          "nextPeriod=%d\n",
          step, name.c_str(), period, range, value, nextPeriod);
#endif
      const TTimePeriod periodItem = {value, name};
      periods.push_back(periodItem);
      if (!nextPeriod) {
        break;
      }
      name = timeOfPeriodSteps[step].name;
      period = nextPeriod;
    }
    std::reverse(periods.begin(), periods.end());
    return periods;
  }

  static String getSecPeriodString(TTimeSec periodSec,
                                   const size_t maxSteps = 2) {
    const TTimePeriods periods = getSecPeriodSteps(periodSec);
    String result = "";
    for (size_t i = 0; i < periods.size() && i < maxSteps; i++) {
      if (i) result += " ";
      const TTimePeriod& period = periods[i];
      result += String(period.time) + period.name;
    }
    return result;
  }

  static TTimeOfPeriodResult timeOfPeriod(TTimeSec timeSec,
                                          TTimeSec periodSec) {
    size_t step = 0;
    TTimeSec time = timeSec;
    TTimeSec period = periodSec;
    String name = timeOfPeriodSteps[step].name;
    while (step < timeOfPeriodStepsCount) {
      step++;
      const auto range = timeOfPeriodSteps[step].range;
      if (time < range && period < range) {
        break;
      }
      if (range) {
        time /= range;
        period /= range;
      }
      name = timeOfPeriodSteps[step].name;
    }
    const TTimeOfPeriodResult result = {time, period, name};
    return result;
  }
};

#endif
