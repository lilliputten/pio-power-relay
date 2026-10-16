#ifndef TimeUtils_DEFINED
#define TimeUtils_DEFINED

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
  String name;  // Eg: sec, min, hr, day, week, month
};

struct TTimePeriod {
  TTimeSec time;
  String name;  // Eg: sec, min, hr, day, week, month
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
      TTimeSec remainder = period % range;
      period /= range;
      TTimePeriod periodItem = {remainder, name};
      if (!period) {
        break;
      }
      name = timeOfPeriodSteps[step].name;
      periods.push_back(periodItem);
    }
    std::reverse(periods.begin(), periods.end());
    return periods;
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
