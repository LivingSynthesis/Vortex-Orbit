#ifndef Modes_h
#define Modes_h

#include "Arduino.h"


class Modes {
  private:

  public:
    Modes();
    bool endOfSet;
    int hue[8] = {0, 32, 64, 96, 128, 160, 192, 224};
    int sat[8] = {255, 255, 255, 255, 255, 255, 255, 255};
    int val[8] = {110, 170, 170, 170, 170, 170, 170, 170};
    int currentColor = 0;
    int nextColor;
    int numColors;
    int patternNum;
    void saveColor(int slot, int thisHue, int thisSat, int thisVal);
    void printMode(int num);
};

#endif
