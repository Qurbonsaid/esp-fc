#include "Device/InputPWM.h"
#include "Utils/MemoryHelper.h"
#include <Arduino.h>

namespace Espfc {

namespace Device {

// Static instance for interrupt handling
InputPWM* InputPWM::_instance = nullptr;

void InputPWM::begin(int pin0, int pin1, int pin2, int pin3)
{
  _instance = this;
  
  _pins[0] = pin0;
  _pins[1] = pin1;
  _pins[2] = pin2;
  _pins[3] = pin3;
  
  // Initialize channel values to safe defaults
  for(size_t i = 0; i < CHANNELS; i++)
  {
    _channels[i] = (i == 2) ? 1000 : 1500; // throttle channel (2) starts at minimum
    _lastUpdate[i] = 0;
    _risingEdge[i] = 0;
  }
  
  _newData = false;
  _lastValidSignal = micros();
  
  // Setup interrupts for each channel
  attachChannelInterrupt(0, pin0);
  attachChannelInterrupt(1, pin1);
  attachChannelInterrupt(2, pin2);
  attachChannelInterrupt(3, pin3);
}

void InputPWM::attachChannelInterrupt(int channel, int pin)
{
  if(pin == -1) return;
  
  pinMode(pin, INPUT);
  
  switch(channel)
  {
    case 0:
      attachInterrupt(digitalPinToInterrupt(pin), handleInterrupt0, CHANGE);
      break;
    case 1:
      attachInterrupt(digitalPinToInterrupt(pin), handleInterrupt1, CHANGE);
      break;
    case 2:
      attachInterrupt(digitalPinToInterrupt(pin), handleInterrupt2, CHANGE);
      break;
    case 3:
      attachInterrupt(digitalPinToInterrupt(pin), handleInterrupt3, CHANGE);
      break;
  }
}

InputStatus FAST_CODE_ATTR InputPWM::update()
{
  uint32_t now = micros();
  
  // Check for signal timeout
  if(now - _lastValidSignal > TIMEOUT_US)
  {
    return INPUT_LOST;
  }
  
  if(_newData)
  {
    _newData = false;
    return INPUT_RECEIVED;
  }
  
  return INPUT_IDLE;
}

uint16_t FAST_CODE_ATTR InputPWM::get(uint8_t i) const
{
  if(i >= CHANNELS) return 1500;
  return _channels[i];
}

void FAST_CODE_ATTR InputPWM::get(uint16_t * data, size_t len) const
{
  for(size_t i = 0; i < len && i < CHANNELS; i++)
  {
    data[i] = _channels[i];
  }
  // Fill remaining channels with safe defaults if requested length > CHANNELS
  for(size_t i = CHANNELS; i < len; i++)
  {
    data[i] = (i == 2) ? 1000 : 1500;
  }
}

size_t InputPWM::getChannelCount() const 
{ 
  return CHANNELS; 
}

bool InputPWM::needAverage() const 
{ 
  return true; // PWM signals can be noisy
}

// Static interrupt handlers
void IRAM_ATTR InputPWM::handleInterrupt0()
{
  if(_instance) _instance->handleChannelInterrupt(0);
}

void IRAM_ATTR InputPWM::handleInterrupt1()
{
  if(_instance) _instance->handleChannelInterrupt(1);
}

void IRAM_ATTR InputPWM::handleInterrupt2()
{
  if(_instance) _instance->handleChannelInterrupt(2);
}

void IRAM_ATTR InputPWM::handleInterrupt3()
{
  if(_instance) _instance->handleChannelInterrupt(3);
}

void IRAM_ATTR InputPWM::handleChannelInterrupt(int channel)
{
  if(channel >= CHANNELS || _pins[channel] == -1) return;
  
  uint32_t now = micros();
  bool state = digitalRead(_pins[channel]);
  
  if(state) // Rising edge
  {
    _risingEdge[channel] = now;
  }
  else // Falling edge
  {
    if(_risingEdge[channel] > 0)
    {
      uint32_t pulseWidth = now - _risingEdge[channel];
      
      // Validate pulse width
      if(pulseWidth >= MIN_PULSE_WIDTH && pulseWidth <= MAX_PULSE_WIDTH)
      {
        _channels[channel] = pulseWidth;
        _lastUpdate[channel] = now;
        _lastValidSignal = now;
        _newData = true;
      }
      
      _risingEdge[channel] = 0;
    }
  }
}

}

}
