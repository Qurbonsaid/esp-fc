#include <unity.h>
#include "Device/InputPWM.h"

using namespace Espfc::Device;

void setUp() {
    // Setup before each test
}

void tearDown() {
    // Cleanup after each test
}

void test_pwm_input_initialization() {
    InputPWM pwm;
    
    // Test that the device initializes correctly
    TEST_ASSERT_EQUAL(4, pwm.getChannelCount());
    TEST_ASSERT_TRUE(pwm.needAverage());
    
    // Test default channel values
    TEST_ASSERT_EQUAL(1500, pwm.get(0)); // Roll
    TEST_ASSERT_EQUAL(1500, pwm.get(1)); // Pitch
    TEST_ASSERT_EQUAL(1000, pwm.get(2)); // Throttle (should start at minimum)
    TEST_ASSERT_EQUAL(1500, pwm.get(3)); // Yaw
}

void test_pwm_input_get_channels() {
    InputPWM pwm;
    uint16_t channels[8];
    
    // Test getting channels into array
    pwm.get(channels, 8);
    
    TEST_ASSERT_EQUAL(1500, channels[0]); // Roll
    TEST_ASSERT_EQUAL(1500, channels[1]); // Pitch
    TEST_ASSERT_EQUAL(1000, channels[2]); // Throttle
    TEST_ASSERT_EQUAL(1500, channels[3]); // Yaw
    // Additional channels should have safe defaults
    TEST_ASSERT_EQUAL(1500, channels[4]);
    TEST_ASSERT_EQUAL(1500, channels[5]);
    TEST_ASSERT_EQUAL(1500, channels[6]);
    TEST_ASSERT_EQUAL(1500, channels[7]);
}

void test_pwm_input_invalid_channel() {
    InputPWM pwm;
    
    // Test invalid channel returns safe default
    TEST_ASSERT_EQUAL(1500, pwm.get(10));
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_pwm_input_initialization);
    RUN_TEST(test_pwm_input_get_channels);
    RUN_TEST(test_pwm_input_invalid_channel);
    
    return UNITY_END();
}
