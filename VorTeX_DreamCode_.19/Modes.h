#ifndef Modes_h
#define Modes_h

#include "Arduino.h"


class Modes {
  private:

  public:
    bool endOfSet;
    int hue[8] = {0, 32, 64, 96, 128, 160, 192, 224};
    int sat[8] = {255, 255, 255, 255, 255, 255, 255, 255};
    int val[8] = {110, 170, 170, 170, 170, 170, 170, 170};
    int currentColor = 0;
    int numColors = 8;
    int patternNum = 3;
    Modes();
    int menuNum;
    int getPattern();
    int getSize();
    void savePattern(int num);
    void saveColor(int slot, int sHue, int sSat, int sVal);
    void addColor();
    void deleteColor();
    void printMode(int num);
};

#endif
