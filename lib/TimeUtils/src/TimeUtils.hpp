#ifndef TimeUtils_DEFINED
#define TimeUtils_DEFINED

#include <Arduino.h>

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

static const TTimeOfPeriodStep timeOfPeriodSteps[] = {
    {0, "sec"},   // seconds
    {60, "min"},  // minutes
    {60, "hr"},   // hours
    {24, "day"},  // days
    {7, "week"},  // weeks
};
static const size_t timeOfPeriodStepsCount =
    sizeof(timeOfPeriodSteps) / sizeof(TTimeOfPeriodStep);

class TimeUtils {
 public:
  // Get a string like "1/10min" for timeSec, periodSec
  static TTimeOfPeriodResult timeOfPeriodFromSecs(TTimeSec timeSec,
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
