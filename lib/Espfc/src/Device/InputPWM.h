#pragma once

#include "Device/InputDevice.h"

namespace Espfc {

namespace Device {

class InputPWM: public InputDevice
{
  public:
    void begin(int pin0, int pin1, int pin2, int pin3);
    InputStatus update() override;
    uint16_t get(uint8_t i) const override;
    void get(uint16_t * data, size_t len) const override;
    size_t getChannelCount() const override;
    bool needAverage() const override;

  private:
    static constexpr size_t CHANNELS = 4;
    static constexpr uint32_t MIN_PULSE_WIDTH = 800;  // minimum valid pulse width in microseconds
    static constexpr uint32_t MAX_PULSE_WIDTH = 2200; // maximum valid pulse width in microseconds
    static constexpr uint32_t TIMEOUT_US = 50000;     // timeout for signal loss detection (50ms)

    void attachChannelInterrupt(int channel, int pin);
    static void handleInterrupt0();
    static void handleInterrupt1();
    static void handleInterrupt2();
    static void handleInterrupt3();
    void handleChannelInterrupt(int channel);

    volatile uint16_t _channels[CHANNELS];
    volatile uint32_t _lastUpdate[CHANNELS];
    volatile uint32_t _risingEdge[CHANNELS];
    volatile bool _newData;
    int _pins[CHANNELS];
    uint32_t _lastValidSignal;
    
    static InputPWM* _instance; // singleton for interrupt handling
};

}

}
