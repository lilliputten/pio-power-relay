#include <ArduinoFake.h>
#include <unity.h>

#include "TimeUtils.hpp"

void setUp(void) {
  // Set up any state needed before each test
  ArduinoFakeReset();
}

void tearDown(void) {
  // Clean up after each test
}

void test_getSecPeriodSteps_s_and_m(void) {
  TTimePeriods result = TimeUtils::getSecPeriodSteps(80);
  // Check result to be {{1,"m"},{20,"s"}}
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_getSecPeriodSteps_s_and_m);
  return UNITY_END();
}
