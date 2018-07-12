#include "Modes.h"

Modes::Modes() {
  bool endOfSet;
  int hue[8] = {0, 32, 64, 96, 128, 160, 192, 224};
  int sat[8] = {255, 255, 255, 255, 255, 255, 255, 255};
  int val[8] = {110, 170, 170, 170, 170, 170, 170, 170};
  int currentColor = 0;
  int numColors = 8;
  int patternNum = 1;
}

int Modes::getPattern() {
  return patternNum;
}

int Modes::getSize() {
  return numColors;
}

int Modes::getHue(int num) {
  return hue[num];
}
int Modes::getSat(int num) {
  return sat[num];
}
int Modes::getVal(int num) {
  return val[num];
}

void Modes::savePattern(int num){
  patternNum = num;
}

void Modes::saveColor(int slot, int sHue, int sSat, int sVal) {
  if (slot == numColors)addColor();
  hue[slot] = sHue;
  sat[slot] = sSat;
  val[slot] = sVal;
}

void Modes::addColor(){
  numColors++;
}

void Modes::deleteColor() {
  if (numColors > 1)numColors--;
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



