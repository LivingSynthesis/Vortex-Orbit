
#include <FastLED.h>
#include <FlashStorage.h>
#include "Modes.h"
#include "Buttons.h"

#define NUM_LEDS 28
#define DATA_PIN 4
#define CLOCK_PIN 3

#define totalModes 8
//---------------------------------------------------------

CRGB leds[NUM_LEDS];
Modes mode[totalModes];
Buttons button[2];

typedef struct Orbit {
  bool dataIsStored;
  int hue[totalModes][8];
  int sat[totalModes][8];
  int val[totalModes][8];
  int numColors[totalModes];
  int patternNum[totalModes];
};
FlashStorage(saveData, Orbit);

bool on;
int menu = 0;
int m = 0;
int stage = 0;
int frame = 0;
int qBand;
int gap;
int ran1, ran2, ran3, ran4;
int patNum, totalPatterns = 9 ;
int targetSlot, currentSlot, targetZone, colorZone;
int targetHue, selectedHue, targetSat, selectedSat, targetVal, selectedVal;
int buttonState, buttonState2, lastButtonState, lastButtonState2 = 0;

unsigned long mainClock, prevTime, duration, dimmer;

//--------------------------------------------------------

void setup() {

  FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  FastLED.setBrightness(30);
  button[0].createButton(0);
  button[1].createButton(2);
  setDefaults();
  loadSave();
  prevTime = 0;
  duration = 0;
}

//--------------------------------------------------------

void loop() {
  if (menu == 0) playMode();
  if (menu == 1) openColors();
  if (menu == 2) colorSet();
  if (menu == 3) openPatterns();
  if (menu == 4) patternSelect();
  checkButton();
  Serial.println(mode[m].currentColor);
  FastLED.show();

}

void playMode() {
  mainClock = millis();
  patterns(mode[m].patternNum);
}

//-----------------------------------------------------
int hue, sat, val;

void patterns(int pat) {

  int totalColors = mode[m].numColors;
  int currentColor = mode[m].currentColor;

  if (pat == 0) { // All Ribbon
    if (mainClock - prevTime > 10) {
      getColor(currentColor);
      setLeds(0, 27);
      nextColor(0);
      prevTime = mainClock;
    }
  }
  if (pat == 1) { //all tracer
    getColor(0);
    setLeds(0, 27);
    if (on) {
      getColor(currentColor);
      setLeds(0, 27);
      duration = 2;
    }
    if (!on) duration = 15;
    if (mainClock - prevTime > duration) {
      if (!on)nextColor(1);
      on = !on;
      prevTime = mainClock;
    }
  }
  if (pat == 2) { //SparkleTrace
    getColor(0);
    setLeds(0, 27);
    if (on) {
      getColor(currentColor);
      setLed(ran1);
      setLed(ran2);
      setLed(ran3);
      setLed(ran4);
    }
    if (mainClock - prevTime > 1) {
      ran1 = random(0, 7);
      ran2 = random(7, 14);
      ran3 = random(14, 21);
      ran4 = random(21, 28);
      if (!on)nextColor (1);
      on = !on;
      prevTime = mainClock;
    }
  }
  if (pat == 3) { // Vortex
    getColor(currentColor);
    if (mainClock - prevTime > 60) {
      clearAll();
      for (int side = 0; side < 4; side++) {
        if (frame <= 3) {
          setLed(3 + (7 * side) + frame);
          setLed(3 + (7 * side) - frame);
        }
        if (frame >= 4) {
          setLed(3 + (7 * side) + (6 - frame));
          setLed(3 + (7 * side) - (6 - frame));
        }
      }
      nextColor(0);
      frame++;
      if (frame > 6) frame = 0;
      prevTime = mainClock;
    }
  }
  if (pat == 4) { // 1/2 Vortex
    getColor(currentColor);
    if (mainClock - prevTime > 75) {
      clearAll();
      for (int side = 0; side < 4; side++) {
        setLed(3 + (7 * side) + frame);
        setLed(3 + (7 * side) - frame);
      }
      frame++;
      if (frame > 3) frame = 0;
      nextColor (0);
      prevTime = mainClock;
    }
  }
  if (pat == 5) { //Dot Zip
    getColor(currentColor);
    if (mainClock - prevTime > 50) {
      clearAll();
      qBand++;
      if (qBand > 6) qBand = 0;
      setLed(qBand);
      setLed(qBand + 7);
      setLed(qBand + 14);
      setLed(qBand + 21);
      nextColor(0);
      prevTime = mainClock;
    }
  }
  if (pat == 6) {
    getColor(currentColor);
    if (mainClock - prevTime > 10) {
      clearAll();
      for (int s = 0; s < 7; s++) {
        if (on) {
          setLed(s);
          setLed(s + 14);
        }
        if (!on) {
          setLed(s + 7);
          setLed(s + 21);
        }
      }
      nextColor(0);
      on = !on;
      prevTime = mainClock;
    }
  }
  if (pat == 7) {
    getColor(0);
    setLeds(0, 2);
    setLeds(11, 17);
    setLeds(25, 27);
    getColor(1);
    setLeds(4, 9);
    setLeds(18, 23);
    getColor(currentColor);
    setLed(3), setLed(10), setLed(17), setLed(24);
    if (mainClock - prevTime > 10) {
      nextColor(2);
      prevTime = mainClock;
    }
  }
  if (pat == 8) {
    getColor(currentColor);
    int color1 = mode[m].hue[currentColor];
    int color2 = mode[m].hue[mode[m].nextColor];
    int finalHue = color1 + gap;
    if (finalHue > 255) finalHue -= 255;
    if (finalHue < 0) finalHue += 255;
    for (int a = 0; a < 28; a++) leds[a].setHSV(finalHue, sat, val);
    if (mainClock - prevTime > 10) {
      if (color1 > color2 && color2 > 127)gap--;
      if (color1 > color2 && color2 <= 127)gap++;
      if (color2 > color1 && color2 > 127)gap--;
      if (color2 > color1 && color2 <= 127)gap++;
      if (finalHue == color2)nextColor(0), gap = 0;
      prevTime = mainClock;
    }
    //    int shift;
    //    if (mainClock - dimmer > 250) {
    //      for (int a = 0; a < 28; a++) leds[a].setHSV(hue + (shift * gap) , sat, val);
    //      gap++;
    //      if (gap > 9) gap = 0;
    //      dimmer = mainClock;
    //    }
    //    if (mainClock - prevTime > 1000) {
    //      getColor(currentColor);
    //      setLeds(0, 28);
    //      nextColor(0, totalColors);
    //      int color1 = mode[m].hue[currentColor];
    //      int color2 = mode[m].hue[mode[m].nextColor];
    //      shift = color2 - color1 / 4;
    //
    //      prevTime = mainClock;
    //    }
  }
  // color fade
  // stretch
  // centerpoint
}

//-----------------------------------------------------

void getColor(int target) {
  hue = mode[m].hue[target];
  sat = mode[m].sat[target];
  val = mode[m].val[target];
}

void setLed(int target) {
  leds[target].setHSV(hue, sat, val);
}

void setLeds(int first, int last) {
  for (int a = first; a <= last; a++) setLed(a);
}

void lightSwitch(unsigned long flipTime) {
  if (mainClock - prevTime > flipTime) {
    on = !on;
    prevTime = mainClock;
  }
}

void nextColor(int start) {
  mode[m].currentColor++;
  if (mode[m].currentColor == mode[m].numColors) mode[m].currentColor = start;
}


void blinkTarget(unsigned long blinkTime) {
  mainClock = millis();
  if (mainClock - prevTime > blinkTime) {
    on = !on;
    prevTime = mainClock;
  }
}

void clearAll() {
  for (int a = 0; a < 28; a++) leds[a].setHSV(0, 0, 0);
}

void openColors() {
  mainClock = millis();
  if (mainClock - prevTime > 75) {
    clearAll();
    for (int side = 0; side < 4; side++) {
      if (frame >= 0 && frame <= 3) {
        leds[3 + (7 * side) + frame].setHSV(0, 0, 170);
        leds[3 + (7 * side) - frame].setHSV(0, 0, 170);
      }
      if (frame >= 4 && frame <= 6) {
        leds[3 + (7 * side) + (6 - frame)].setHSV(0, 0, 170);
        leds[3 + (7 * side) - (6 - frame)].setHSV(0, 0, 170);
      }
    }
    frame++;
    if (frame > 6) frame = 0;
    prevTime = mainClock;
  }
}

void colorSet() {
  if (stage == 0) {
    int setSize = mode[m].numColors;
    clearAll();
    for (int colorSlot = 0; colorSlot < 8; colorSlot++) {
      int side = colorSlot / 2;
      leds[colorSlot + (5 * side)].setHSV(0, 0, 1);
      leds[((2 + (3 * side)) * 3) - colorSlot].setHSV(0, 0, 1);
    }
    for (int colorNum = 0; colorNum < setSize; colorNum++) {
      getColor(colorNum);
      int side = colorNum / 2;
      leds[(5 * side) + colorNum].setHSV(hue, sat, val);
      leds[((2 + (3 * side)) * 3) - colorNum].setHSV(hue, sat, val);
    }
    if (targetSlot < setSize) {
      if (on) {
        int side = targetSlot / 2;
        int blinkVal = 0;
        if (mode[m].val[targetSlot] == 0)blinkVal = 5;
        leds[targetSlot + (5 * side)].setHSV(0, 0, blinkVal);
        leds[((2 + (3 * side)) * 3) - targetSlot].setHSV(0, 0, blinkVal);
      }
      blinkTarget(300);
    }
    if (targetSlot == setSize) {
      if (on) {
        int side = (setSize - 1) / 2;
        leds[(5 * side) + (setSize - 1)].setHSV(0, 0, 1);
        leds[((2 + (3 * side)) * 3) - (setSize - 1)].setHSV(0, 0, 1);
        for (int side = 0; side < 4; side++) {
          leds[2 + (7 * side)].setHSV(0, 0, 1);
          leds[4 + (7 * side)].setHSV(0, 0, 1);
        }
      }
      blinkTarget(60);
    }
    if (targetSlot > setSize) {
      if (on) {
        int targLed = targetSlot - 1;
        int side = targLed / 2;
        leds[targLed + (5 * side)].setHSV(0, 0, 0);
        leds[((2 + (3 * side)) * 3) - targLed].setHSV(0, 0, 0);
      }
      blinkTarget(300);
    }
  }
  if (stage == 1) colorWheel(0);
  if (stage == 2) colorWheel(1);
  if (stage == 3) colorWheel(2);
  if (stage == 4) colorWheel(3);
}
void colorWheel(int layer) {
  int hue = 0, sat = 255, val = 170;
  if (layer == 0) {
    for (int color = 0; color < 16; color++) {
      int side = color / 4;
      hue = color * 16;
      if (color >= 0 + 4 * side && color <= 3 + side * 4) {
        leds[(side * 3) + color].setHSV(hue, sat, val);
        leds[((side * 11) + 6) - color].setHSV(hue, sat, val);
      }
    }
  }
  if (layer == 1) {
    for (int shade = 0; shade < 4; shade++) {
      hue = (shade * 16) + (64 * colorZone);
      for (int band = 0; band < 7; band++)leds[band + (7 * shade)].setHSV(hue, sat, val);
    }
  }
  if (layer == 2) {
    for (int fade = 0; fade < 4; fade++) {
      sat = 255 - (85 * fade);
      for (int band = 0; band < 7; band++)leds[band + (7 * fade)].setHSV(selectedHue, sat, val);
    }
  }
  if (layer == 3) {
    for (int bright = 0; bright < 4; bright ++) {
      val = 255 - (85 * bright);
      if (bright == 2) val = 120;
      for (int band = 0; band < 7; band++)leds[band + (7 * bright)].setHSV(selectedHue, selectedSat, val);
    }
  }
  if (on) {
    if (layer == 0)for (int band = 0; band < 7; band++)leds[band + (7 * targetZone)].setHSV(0, 0, 0);
    if (layer == 1)for (int band = 0; band < 7; band++)leds[band + (7 * targetHue)].setHSV(0, 0, 0);
    if (layer == 2)for (int band = 0; band < 7; band++)leds[band + (7 * targetSat)].setHSV(0, 0, 0);
    if (layer == 3)for (int band = 0; band < 7; band++)leds[band + (7 * targetVal)].setHSV(0, 0, 0);
    if (layer == 3 && targetVal == 3) for (int band = 0; band < 7; band++)leds[band + (7 * targetVal)].setHSV(0, 0, 1);
  }
  blinkTarget(300);
}

void openPatterns() {
  mainClock = millis();
  if (mainClock - prevTime > 100) {
    for (int a = 0; a < 28; a++) {
      leds[a].setHSV(0, 0, 255);
    }
    if (on) {
      clearAll();
    }
    on = !on;
    prevTime = mainClock;
  }
}

void patternSelect() {
  mainClock = millis();
  patterns(patNum);
}

void checkButton() {
  for (int b = 0; b < 2; b++) {
    button[b].buttonState = digitalRead(button[b].pinNum);
    if (button[b].buttonState == LOW && button[b].lastButtonState == HIGH && (millis() - button[b].pressTime > 200)) {
      button[b].pressTime = millis();
    }
    button[b].holdTime = (millis() - button[b].pressTime);
    if (button[b].holdTime > 50) {
      if (button[b].buttonState == LOW && button[b].holdTime > button[b].prevHoldTime) {
        if (b == 0) {
          if (button[b].holdTime > 2000 && button[b].holdTime < 3000) menu = 1;
          if (button[b].holdTime > 3000 && menu == 1) menu = 2;
        }
        if (b == 1) {
          if (button[b].holdTime > 2000 && button[b].holdTime < 3000) menu = 3;
          if (button[b].holdTime > 3000 && menu == 3) menu = 4, mode[m].currentColor = 0;
        }
      }
      if (button[b].buttonState == HIGH && button[b].lastButtonState == LOW && millis() - button[b].prevPressTime > 200) {
        if (button[b].holdTime < 300) {
          if (b == 0) {                                       //next option
            if (menu == 0)m++, mode[m].currentColor = 0;
            if (menu == 2) {
              if (stage == 0) targetSlot++;
              if (stage == 1) targetZone++;
              if (stage == 2) targetHue++;
              if (stage == 3) targetSat++;
              if (stage == 4) targetVal++;
            }
            if (menu == 4)patNum++, frame = 0, mode[m].currentColor = 0;
          }
          if (b == 1) {                                       //previous option
            if (menu == 0)m--, mode[m].currentColor = 0;
            if (menu == 2) {
              if (stage == 0) targetSlot--; //previous option
              if (stage == 1) targetZone--;
              if (stage == 2) targetHue--;
              if (stage == 3) targetSat--;
              if (stage == 4) targetVal--;
            }
            if (menu == 4)patNum--, frame = 0, mode[m].currentColor = 0;
          }
        }
        if (button[b].holdTime > 400 && button[b].holdTime < 3000) {
          //medium press
          if (b == 0) {
            if (menu == 2) {
              if (stage == 0) {
                int setSize = mode[m].numColors;
                if (targetSlot < setSize)stage = 1, currentSlot = targetSlot; //confirm selection
                if (targetSlot == setSize && mode[m].numColors > 1)targetSlot--, mode[m].numColors--;
                if (targetSlot > setSize)stage = 1, currentSlot = setSize;
              }
              else if (stage == 1) stage = 2, colorZone = targetZone;
              else if (stage == 2) stage = 3, selectedHue = (targetHue * 16) + (colorZone * 64);
              else if (stage == 3) stage = 4, selectedSat = 255 - (85 * targetSat);
              else if (stage == 4) {
                selectedVal = 255 - (85 * targetVal);
                if (targetVal == 2) selectedVal = 120;
                mode[m].saveColor(currentSlot, selectedHue, selectedSat, selectedVal);
                stage = 0;
              }
            }
            if (menu == 4)mode[m].patternNum = patNum, menu = 0, saveAll();//confirm selection
          }
          if (b == 1) {
            if (menu == 2) {
              if (stage == 0)menu = 0, clearAll(), saveAll();//cancle exit
              if (stage == 1)stage = 0;
              if (stage == 2)stage = 1;
              if (stage == 3)stage = 2;
              if (stage == 4)stage = 3;
            }
            if (menu == 4)menu = 0;//cancle exit
          }
        }
        if (button[b].holdTime < 3000 && menu == 1)menu = 0;
        if (button[b].holdTime < 3000 && menu == 3)menu = 0;
        button[b].prevPressTime = millis();
      }
    }
    //these are the max and minimum values for each variable.
    if (patNum > totalPatterns - 1) patNum = 0;
    if (patNum < 0) patNum = totalPatterns - 1;
    int lastSlot = mode[m].numColors + 1;
    if (mode[m].numColors == 8) lastSlot = mode[m].numColors;
    if (targetSlot > lastSlot) targetSlot = 0;
    if (targetSlot < 0) targetSlot = lastSlot;
    if (targetZone > 3) targetZone = 0;
    if (targetZone < 0) targetZone = 3;
    if (targetHue > 3) targetHue = 0;
    if (targetHue < 0) targetHue = 3;
    if (targetSat > 3) targetSat = 0;
    if (targetSat < 0) targetSat = 3;
    if (targetVal > 3) targetVal = 0;
    if (targetVal < 0) targetVal = 3;
    if (m < 0)m = totalModes - 1;
    if (m > totalModes - 1)m = 0;
    button[b].lastButtonState = button[b].buttonState;
    button[b].prevHoldTime = button[b].holdTime;
  }
}

void loadSave() {
  Orbit myOrbit;
  myOrbit = saveData.read();
  if (myOrbit.dataIsStored == true) {
    for (int modes = 0; modes < totalModes; modes ++) {
      for (int c = 0; c < mode[modes].numColors; c++) {
        mode[modes].hue[c] = myOrbit.hue[modes][c];
        mode[modes].sat[c] = myOrbit.sat[modes][c];
        mode[modes].val[c] = myOrbit.val[modes][c];
      }
      mode[modes].numColors = myOrbit.numColors[modes];
      mode[modes].patternNum = myOrbit.patternNum[modes];
    }
  }
}
void saveAll() {
  Orbit myOrbit;
  for (int modes = 0; modes < totalModes; modes ++) {
    for (int c = 0; c < mode[modes].numColors; c++) {
      myOrbit.hue[modes][c] = mode[modes].hue[c];
      myOrbit.sat[modes][c] = mode[modes].sat[c];
      myOrbit.val[modes][c] = mode[modes].val[c];
    }
    myOrbit.numColors[modes] = mode[modes].numColors;
    myOrbit.patternNum[modes] = mode[modes].patternNum;
  }
  myOrbit.dataIsStored = true;
  saveData.write(myOrbit);
}

void setDefaults() {
  for (int modes = 0; modes < totalModes; modes ++) {
    mode[modes].patternNum = modes;
    mode[modes].numColors = 8 - modes;
  }
}

