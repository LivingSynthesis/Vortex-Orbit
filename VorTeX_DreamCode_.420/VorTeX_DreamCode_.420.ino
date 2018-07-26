
#include <FastLED.h>
#include <FlashStorage.h>
#include "Modes.h"
#include "Buttons.h"

#define NUM_LEDS 28
#define DATA_PIN 4
#define CLOCK_PIN 3

#define totalModes 7
//---------------------------------------------------------

CRGB leds[NUM_LEDS];
Modes mode[totalModes];
Buttons button[2];

typedef struct Orbit {
  bool dataIsStored;
  int sHue[totalModes][8];
  int sSat[totalModes][8];
  int sVal[totalModes][8];
  int sNumColors[totalModes];
  int sPatternNum[totalModes];
};
FlashStorage(saveData, Orbit);

bool on;
int m = 0;
int menu;
int stage = 0;
int frame = 0;
int qBand;
int gap, rep;
int ran1, ran2, ran3, ran4;
int patNum, totalPatterns = 10 ;
int targetSlot, currentSlot, targetZone, colorZone;
int targetHue, selectedHue, targetSat, selectedSat, targetVal, selectedVal;
int buttonState, buttonState2, lastButtonState, lastButtonState2 = 0;

unsigned long mainClock, prevTime, duration, dimmer;

const byte numChars = 128;
char receivedChars[numChars];

boolean newData = false;

int dataNumber = 0;

//--------------------------------------------------------

void setup() {
  Serial.begin(9600);
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
  menu = mode[m].menuNum;
  if (menu == 0) playMode();
  if (menu == 1) openColors();
  if (menu == 2) colorSet();
  if (menu == 3) openPatterns();
  if (menu == 4) patternSelect();
  if (menu == 5) confirm();
  checkButton();
  checkSerial();
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
  int next = mode[m].nextColor;

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
      if (totalColors == 1) val = 0;
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
      if (totalColors == 1) val = 0;
      setLed(ran1);
      setLed(ran2);
      setLed(ran3);
      setLed(ran4);
    }
    ran1 = random(0, 7);
    ran2 = random(7, 14);
    ran3 = random(14, 21);
    ran4 = random(21, 28);
    if (!on)nextColor (1);
    on = !on;
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
      nextColor (0);
      frame++;
      if (frame > 6) frame = 0;
      prevTime = mainClock;
    }
  }
  if (pat == 4) { //Dot Zip
    getColor(currentColor);
    if (mainClock - prevTime > 50) {
      clearAll();
      qBand++;
      if (qBand > 6) qBand = 0;
      setLed(qBand);
      setLed(qBand + 7);
      setLed(qBand + 14);
      setLed(qBand + 21);
      nextColor (0);
      prevTime = mainClock;
    }
  }
  if (pat == 5) {
    getColor(currentColor);
    if (mainClock - prevTime > 100) {
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
        nextColor (0);
      }

      on = !on;
      prevTime = mainClock;
    }
  }
  if (pat == 6) { //Impact
    if (mainClock - prevTime > 75) on = !on, prevTime = mainClock;
    getColor(0);
    if (on) {
      clearAll();
      setLeds(0, 2);
      setLeds(11, 17);
      setLeds(25, 27);
    }
    getColor(1);
    if (totalColors == 1) val = 0;
    if (!on) {
      clearAll();
      setLeds(4, 9);
      setLeds(18, 23);
    }
    getColor(currentColor);
    if (totalColors <= 2) val = 0;
    setLed(3), setLed(10), setLed(17), setLed(24);
    nextColor(2);
  }
  if (pat == 7) {
    getColor(currentColor);
    int color1 = mode[m].hue[currentColor];
    int color2 = mode[m].hue[next];
    if (color1 > color2 && color1 - color2 < (255 - color1) + color2)gap--;
    if (color1 > color2 && color1 - color2 > (255 - color1) + color2)gap++;
    if (color1 < color2 && color2 - color1 < (255 - color2) + color1)gap++;
    if (color1 < color2 && color2 - color1 > (255 - color2) + color1)gap--;
    if (color1 + gap >= 255) gap -= 255;
    if (color1 + gap < 0) gap += 255;
    int finalHue = color1 + gap;
    if (finalHue == color2) gap = 0, nextColor(0);
    for (int a = 0; a < 28; a++) leds[a].setHSV(finalHue, sat, val);
  }
  if (pat == 8) {
    getColor(currentColor);
    setLeds(0, 27);
    nextColor(0);
  }

  if (pat == 9) {
    if (!on) clearAll(); if (mainClock - prevTime > 10) on = !on, prevTime = mainClock;
    if (on) {
      getColor(currentColor);
      setLeds(0, 27);
      if (currentColor == totalColors - 1) on = !on;
      nextColor(0);
    }
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
  if (mode[m].currentColor >= mode[m].numColors) mode[m].currentColor = start;
  mode[m].nextColor = mode[m].currentColor + 1;
  if (mode[m].nextColor >= mode[m].numColors) mode[m].nextColor = start;
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

void confirm() {
  mainClock = millis();
  if (mainClock - prevTime > 50) {
    if (frame == 0) clearAll();
    if (frame == 1) sat = 0, setLeds(0, 27);
    if (frame == 2) clearAll();
    if (frame == 3) frame = 0, mode[m].menuNum = 0;
    frame++;
    prevTime = mainClock;
  }
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
          if (button[b].holdTime > 2000 && button[b].holdTime < 3000 && menu == 0) mode[m].menuNum = 1;
          if (button[b].holdTime > 3000 && menu == 1) mode[m].menuNum = 2;
        }
        if (b == 1) {
          if (button[b].holdTime > 2000 && button[b].holdTime < 3000 && menu == 0) mode[m].menuNum = 3;
          if (button[b].holdTime > 3000 && menu == 3) mode[m].menuNum = 4, mode[m].currentColor = 0;
        }
      }
      if (button[b].buttonState == HIGH && button[b].lastButtonState == LOW && millis() - button[b].prevPressTime > 200) {
        if (button[b].holdTime < 300) {
          if (b == 0) {
            if (menu == 0)m++, frame = 0, gap = 0, mode[m].currentColor = 0;
            if (menu == 2) {
              if (stage == 0) targetSlot++; //next option
              if (stage == 1) targetZone++;
              if (stage == 2) targetHue++;
              if (stage == 3) targetSat++;
              if (stage == 4) targetVal++;
            }
            if (menu == 4)patNum++, frame = 0, mode[m].currentColor = 0;
          }
          if (b == 1) {
            if (menu == 0)m--, frame = 0; gap = 0, mode[m].currentColor = 0;
            if (menu == 2) {
              if (stage == 0) targetSlot--; //previous option
              if (stage == 1) targetZone--;
              if (stage == 2) targetHue--;
              if (stage == 3) targetSat--;
              if (stage == 4) targetVal--;
            }
            if (menu == 4)patNum--, frame = 0, mode[m].currentColor = 0; //previous option
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
            if (menu == 4) {
              mode[m].patternNum = patNum, saveAll(), mode[m].menuNum = 5;//confirm selection
            }
          }
          if (b == 1) {
            if (menu == 2) {
              if (stage == 0)mode[m].currentColor = 0, mode[m].nextColor = 1, saveAll(), mode[m].menuNum = 0;//cancle exit
              if (stage == 1)stage = 0;
              if (stage == 2)stage = 1;
              if (stage == 3)stage = 2;
              if (stage == 4)stage = 3;
            }
            if (menu == 4)mode[m].menuNum = 5;//cancle exit
          }
        }
        if (button[b].holdTime > 600 && Serial) exportSettings();
        if (button[b].holdTime < 3000 && menu == 1)mode[m].menuNum = 0;
        if (button[b].holdTime < 3000 && menu == 3)mode[m].menuNum = 0;
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
      mode[modes].patternNum = myOrbit.sPatternNum[modes];
      mode[modes].numColors = myOrbit.sNumColors[modes];
      for (int c = 0; c < mode[modes].numColors; c++) {
        mode[modes].hue[c] = myOrbit.sHue[modes][c];
        mode[modes].sat[c] = myOrbit.sSat[modes][c];
        mode[modes].val[c] = myOrbit.sVal[modes][c];
      }
    }
  }
}
void saveAll() {
  Orbit myOrbit;
  for (int modes = 0; modes < totalModes; modes ++) {
    myOrbit.sPatternNum[modes] = mode[modes].patternNum;
    myOrbit.sNumColors[modes] = mode[modes].numColors;
    for (int c = 0; c < mode[modes].numColors; c++) {
      myOrbit.sHue[modes][c] = mode[modes].hue[c];
      myOrbit.sSat[modes][c] = mode[modes].sat[c];
      myOrbit.sVal[modes][c] = mode[modes].val[c];
    }
  }
  myOrbit.dataIsStored = true;
  saveData.write(myOrbit);
}
void setDefaults() {
  mode[0].patternNum = 8;
  mode[0].numColors = 8;
  for (int c = 0; c < mode[0].numColors; c++) {
    mode[0].hue[c] = c * 32;
    mode[0].sat[c] = 255;
    mode[0].val[c] = 170;
  }

  mode[1].patternNum = 6;
  mode[1].numColors = 3;
  mode[1].hue[0] = 0;
  mode[1].sat[0] = 255;
  mode[1].val[0] = 170;
  mode[1].hue[1] = 160;
  mode[1].sat[1] = 255;
  mode[1].val[1] = 170;
  mode[1].hue[2] = 224;
  mode[1].sat[2] = 255;
  mode[1].val[2] = 120;


  mode[2].patternNum = 2;
  mode[2].numColors = 5;
  mode[2].hue[0] = 0;
  mode[2].sat[0] = 0;
  mode[2].val[0] = 0;
  mode[2].hue[1] = 0;
  mode[2].sat[1] = 0;
  mode[2].val[1] = 120;
  mode[2].hue[2] = 64;
  mode[2].sat[2] = 170;
  mode[2].val[2] = 120;
  mode[2].hue[2] = 64;
  mode[2].sat[2] = 255;
  mode[2].val[2] = 120;
  mode[2].hue[3] = 160;
  mode[2].sat[3] = 255;
  mode[2].val[3] = 120;



  mode[3].patternNum = 3;
  mode[3].numColors = 2;
  mode[3].hue[0] = 224;
  mode[3].sat[0] = 255;
  mode[3].val[0] = 170;
  mode[3].hue[1] = 192;
  mode[3].sat[1] = 255;
  mode[3].val[1] = 170;


  mode[4].patternNum = 9;
  mode[4].numColors = 3;
  mode[4].hue[0] = 0;
  mode[4].sat[0] = 255;
  mode[4].val[0] = 170;
  mode[4].hue[1] = 96;
  mode[4].sat[1] = 255;
  mode[4].val[1] = 170;
  mode[4].hue[2] = 160;
  mode[4].sat[2] = 255;
  mode[4].val[2] = 170;


  mode[5].patternNum = 5;
  mode[5].numColors = 4;
  mode[5].hue[0] = 0;
  mode[5].sat[0] = 255;
  mode[5].val[0] = 120;
  mode[5].hue[1] = 160;
  mode[5].sat[1] = 255;
  mode[5].val[1] = 170;
  mode[5].hue[2] = 64;
  mode[5].sat[2] = 255;
  mode[5].val[2] = 170;
  mode[5].hue[3] = 96;
  mode[5].sat[3] = 255;
  mode[5].val[3] = 170;

  mode[6].patternNum = 7;
  mode[6].numColors = 3;
  mode[6].hue[0] = 0;
  mode[6].sat[0] = 255;
  mode[6].val[0] = 120;
  mode[6].hue[1] = 192;
  mode[6].sat[1] = 255;
  mode[6].val[1] = 170;
  mode[6].hue[2] = 128;
  mode[6].sat[2] = 255;
  mode[6].val[2] = 170;
}

void exportSettings() {
  Serial.println("Each line below contains 1 mode, copy and paste them to the line above to upload it!");
  for (int mo = 0; mo < totalModes; mo++) {
    Serial.print("<");
    Serial.print(mo);
    Serial.print(", ");
    Serial.print(mode[mo].patternNum);
    Serial.print(", ");
    Serial.print(mode[mo].numColors);
    Serial.print(", ");
    for (int co = 0; co < 8; co++) {
      Serial.print(mode[mo].hue[co]);
      Serial.print(", ");
      Serial.print(mode[mo].sat[co]);
      Serial.print(", ");
      Serial.print(mode[mo].val[co]);
      if (co != 7) Serial.print(", ");
    }
    Serial.println(">");
  }
}

void checkSerial() {
  recvWithStartEndMarkers();
  importData();
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  if (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void importData() {
  if (newData == true) {
    char * strtokIndx; // this is used by strtok() as an index
    strtokIndx = strtok(receivedChars, ",");
    int mNum = atoi(strtokIndx);
    strtokIndx = strtok(NULL, ",");
    mode[mNum].patternNum = atoi(strtokIndx);
    strtokIndx = strtok(NULL, ",");
    mode[mNum].numColors = atoi(strtokIndx);
    for (int col = 0; col < 8; col++) {
      strtokIndx = strtok(NULL, ",");
      mode[mNum].hue[col] = atoi(strtokIndx);
      strtokIndx = strtok(NULL, ",");
      mode[mNum].sat[col] = atoi(strtokIndx);
      strtokIndx = strtok(NULL, ",");
      mode[mNum].val[col] = atoi(strtokIndx);      
    }
    Serial.println("Data recieved");
    saveAll();
    newData = false;
  }
}
