// See `.pio/libdeps/native/ArduinoFake/src/Arduino.h`
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

void test_time_of_period_seconds(void) {
    TTimeOfPeriodResult result = TimeUtils::timeOfPeriodFromSecs(45, 50);
    TEST_ASSERT_EQUAL(45, result.time);
    TEST_ASSERT_EQUAL(50, result.period);
    TEST_ASSERT_EQUAL_STRING("sec", result.name.c_str());
}

void test_time_of_period_minutes(void) {
    TTimeOfPeriodResult result = TimeUtils::timeOfPeriodFromSecs(90, 150);
    TEST_ASSERT_EQUAL(1, result.time); // 90s / 60 = 1
    TEST_ASSERT_EQUAL(2, result.period);   // 150s / 60 = 2
    TEST_ASSERT_EQUAL_STRING("min", result.name.c_str());
}

void test_time_of_period_hours(void) {
    TTimeOfPeriodResult result = TimeUtils::timeOfPeriodFromSecs(3600, 7200);
    TEST_ASSERT_EQUAL(1, result.time); // 3600s / 3600 = 1
    TEST_ASSERT_EQUAL(2, result.period);   // 7200s / 3600 = 2
    TEST_ASSERT_EQUAL_STRING("hr", result.name.c_str());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_time_of_period_seconds);
    RUN_TEST(test_time_of_period_minutes);
    RUN_TEST(test_time_of_period_hours);
    return UNITY_END();
}
