#include "Modes.h"

Modes::Modes() {
  bool endOfSet;
  int hue[8] = {0, 32, 64, 96, 128, 160, 192, 224};
  int sat[8] = {255, 255, 255, 255, 255, 255, 255, 255};
  int val[8] = {120, 170, 170, 170, 170, 170, 170, 170};
  int currentColor;
  int nextColor;
  int numColors;
  int patternNum;
}

void Modes::saveColor(int slot, int thisHue, int thisSat, int thisVal) {
  if (slot == numColors)numColors++;
  hue[slot] = thisHue;
  sat[slot] = thisSat;
  val[slot] = thisVal;
}

void Modes::printMode(int num) {
  Serial.print("Mode "), Serial.println(num);
  for (int slot = 0; slot < numColors; slot ++) {
    Serial.print(hue[slot]), Serial.print(" ");
    Serial.print(sat[slot]), Serial.print(" ");
    Serial.println(val[slot]);
  }
  Serial.println();
}



