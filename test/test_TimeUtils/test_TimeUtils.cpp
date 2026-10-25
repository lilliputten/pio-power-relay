#include <ArduinoFake.h>
#include <unity.h>

// #define test_TimeUtils_DEBUG

#include "TimeUtils.hpp"

// #include "TimeUtils_getSecPeriodSteps.cpp"
// #include "TimeUtils_timeOfPeriod.cpp"

void setUp(void) {
  // Set up any state needed before each test
  ArduinoFakeReset();
}

void tearDown(void) {
  // Clean up after each test
}

/// getSecPeriodSteps

void test_getSecPeriodSteps_s_and_m(void) {
  TTimePeriods result = TimeUtils::getSecPeriodSteps(80);
  TTimePeriods expect = {{1, "m"}, {20, "s"}};
#ifdef test_TimeUtils_DEBUG
  // DEBUG: Show results:
  printf("Got size: %d (expected: %d)\n", result.size(), expect.size());
  for (size_t i = 0; i < result.size(); i++) {
    printf("%d: {%d, %s}\n", i, result[i].time, result[i].name.c_str());
  }
#endif
  // Check elements count
  TEST_ASSERT_EQUAL(result.size(), expect.size());
  // 1st row
  TEST_ASSERT_EQUAL(result[0].time, expect[0].time);
  TEST_ASSERT_EQUAL_STRING(result[0].name.c_str(), expect[0].name.c_str());
  // 2nd row
  TEST_ASSERT_EQUAL(result[1].time, expect[1].time);
  TEST_ASSERT_EQUAL_STRING(result[1].name.c_str(), expect[1].name.c_str());
}

/// timeOfPeriod

void test_timeOfPeriod_seconds(void) {
  TTimeOfPeriodResult result = TimeUtils::timeOfPeriod(45, 50);
  TEST_ASSERT_EQUAL(45, result.time);
  TEST_ASSERT_EQUAL(50, result.period);
  TEST_ASSERT_EQUAL_STRING("s", result.name.c_str());
}

void test_timeOfPeriod_minutes(void) {
  TTimeOfPeriodResult result = TimeUtils::timeOfPeriod(90, 150);
  TEST_ASSERT_EQUAL(1, result.time);    // 90s / 60 = 1
  TEST_ASSERT_EQUAL(2, result.period);  // 150s / 60 = 2
  TEST_ASSERT_EQUAL_STRING("m", result.name.c_str());
}

void test_timeOfPeriod_hours(void) {
  TTimeOfPeriodResult result = TimeUtils::timeOfPeriod(3600, 7200);
  TEST_ASSERT_EQUAL(1, result.time);    // 3600s / 3600 = 1
  TEST_ASSERT_EQUAL(2, result.period);  // 7200s / 3600 = 2
  TEST_ASSERT_EQUAL_STRING("h", result.name.c_str());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();

  // getSecPeriodSteps
  RUN_TEST(test_getSecPeriodSteps_s_and_m);

  // timeOfPeriod
  RUN_TEST(test_timeOfPeriod_seconds);
  RUN_TEST(test_timeOfPeriod_minutes);
  RUN_TEST(test_timeOfPeriod_hours);

  return UNITY_END();
}
