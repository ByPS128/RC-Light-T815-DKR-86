#include "NoiseFilter.h"

NoiseFilter::NoiseFilter()
  : buffer(nullptr), bufferSize(0), currentSize(0), timeThreshold(0), countThreshold(0) {
}

NoiseFilter::~NoiseFilter() {
  clearBuffer();
}

void NoiseFilter::setProperties(int bufferSize, unsigned long timeThreshold, int countThreshold) {
  clearBuffer();
  this->bufferSize = bufferSize;
  this->timeThreshold = timeThreshold;
  this->countThreshold = countThreshold;
  buffer = new BufferItem[bufferSize]();
  currentSize = 0;
}

void NoiseFilter::addValue(unsigned long timestamp, bool value) {
  if (currentSize < bufferSize) {
    buffer[currentSize].timestamp = timestamp;
    buffer[currentSize].value = value;
    currentSize++;
  } else {
    shiftBuffer();
    buffer[bufferSize - 1].timestamp = timestamp;
    buffer[bufferSize - 1].value = value;
  }
}

void NoiseFilter::shiftBuffer() {
  for (int i = 0; i < bufferSize - 1; i++) {
    buffer[i] = buffer[i + 1];
  }
}

void NoiseFilter::clearBuffer() {
  if (buffer != nullptr) {
    delete[] buffer;
    buffer = nullptr;
  }
}

bool NoiseFilter::getFilteredValue() const {
  int count = 0;
  unsigned long currentMillis = millis();
  for (int i = 0; i < currentSize; i++) {
    if (buffer[i].timestamp >= currentMillis - timeThreshold) {
      if (buffer[i].value) {
        count++;
      }
    }
  }
  return count >= countThreshold;
}
