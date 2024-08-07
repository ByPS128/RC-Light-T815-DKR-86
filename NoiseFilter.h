#pragma once

#include <Arduino.h>

class NoiseFilter {
public:
  NoiseFilter();
  ~NoiseFilter();

  void setProperties(int bufferSize, unsigned long timeThreshold, int countThreshold);
  void addValue(unsigned long timestamp, bool value);
  bool getFilteredValue() const;

private:
  struct BufferItem {
    unsigned long timestamp;
    bool value;
  };

  BufferItem* buffer;
  int bufferSize;
  int currentSize;
  unsigned long timeThreshold;
  int countThreshold;

  void shiftBuffer();
  void clearBuffer();
};
