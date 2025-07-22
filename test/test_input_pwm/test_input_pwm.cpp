#include <unity.h>
#include <ArduinoFake.h>
#include "Device/InputPWM.h"
#include "Device/InputDevice.h"

using namespace Espfc;
using namespace Espfc::Device;
using namespace fakeit;

void setUp() {
    // Setup before each test
}

void tearDown() {
    // Cleanup after each test
}

void test_pwm_input_initialization() {
    InputPWM pwm;
    
    // Mock Arduino functions
    When(Method(ArduinoFake(), micros)).Return(0);
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalPinToInterrupt)).Return(0);
    When(Method(ArduinoFake(), attachInterrupt)).Return();
    
    // Initialize with dummy pins (they won't actually be used in test)
    pwm.begin(2, 3, 4, 5);
    
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
    
    // Mock Arduino functions
    When(Method(ArduinoFake(), micros)).Return(0);
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalPinToInterrupt)).Return(0);
    When(Method(ArduinoFake(), attachInterrupt)).Return();
    
    // Initialize with dummy pins
    pwm.begin(2, 3, 4, 5);
    
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
    
    // Mock Arduino functions
    When(Method(ArduinoFake(), micros)).Return(0);
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalPinToInterrupt)).Return(0);
    When(Method(ArduinoFake(), attachInterrupt)).Return();
    
    // Initialize with dummy pins
    pwm.begin(2, 3, 4, 5);
    
    // Test invalid channel returns safe default
    TEST_ASSERT_EQUAL(1500, pwm.get(10));
}

void test_pwm_input_update_status() {
    InputPWM pwm;
    
    // Mock Arduino functions
    When(Method(ArduinoFake(), micros)).Return(1000);
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalPinToInterrupt)).Return(0);
    When(Method(ArduinoFake(), attachInterrupt)).Return();
    
    // Initialize with dummy pins
    pwm.begin(2, 3, 4, 5);
    
    // Test initial status
    InputStatus status = pwm.update();
    TEST_ASSERT_EQUAL(INPUT_IDLE, status);
    
    // Test timeout condition - simulate time passing without signal
    When(Method(ArduinoFake(), micros)).Return(60000); // 60ms later
    status = pwm.update();
    TEST_ASSERT_EQUAL(INPUT_LOST, status);
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_pwm_input_initialization);
    RUN_TEST(test_pwm_input_get_channels);
    RUN_TEST(test_pwm_input_invalid_channel);
    RUN_TEST(test_pwm_input_update_status);
    
    return UNITY_END();
}
