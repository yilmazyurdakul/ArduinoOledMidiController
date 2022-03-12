#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define encoder0PinA  9
#define encoder0PinB  2

byte selectedVal = 0;
float inc = 0;

const byte rxPin = 12; // not used for the moment
const byte txPin = 11;

byte clk = 4;
long buttonTimer = 0;
long longPressTime = 500;
boolean buttonActive = false;
boolean longPressActive = false;

byte val1;
byte val1Last;
byte val2;
byte val2Last;
byte val3;
byte val3Last;
byte val4;
byte val4Last;
byte val5;
byte val5Last;

int xVal;
byte xValLast;
int yVal;
byte yValLast;
int aVal;
byte aValLast;
int bVal;
byte bValLast;
int cVal;
byte cValLast;
int dVal;
byte dValLast;

byte b1Val; //button series
byte b1ValLast;

byte b2Val;
byte b2ValLast;

byte b3Val;
byte b3ValLast;

byte b4Val;
byte b4ValLast;

byte b5Val;
byte b5ValLast;

//values for screen
byte xVals;
byte yVals;
byte aVals;
byte bVals;
byte cVals;
byte dVals;

byte yValsOld;
byte xValsOld;

byte ch = 1;
int oldVal;

bool enabled = false;

byte noteON = 144;//144 = 10010000 in binary, note on command
byte noteOFF = 128;//128 = 10000000 in binary, note off command

byte note1 = 47;
byte note1Old;
byte note2 = 48;
byte note2Old;
byte note3 = 50;
byte note3Old;
byte note4 = 52;
byte note4Old;
byte note5 = 53;
byte note5Old;
byte velocity;
byte velocityOld;

byte octave = 0;
char octaveBank = 'A';

byte countLimit = 10;
int changeVal = 1;

bool flag1 = false;
bool flag2 = false;

byte baudSet;
byte baudNum = 1;
byte baudSetOld;

const char noteNames[12] = {'C', 'C', 'D', 'D', 'E', 'F', 'F', 'G', 'G', 'A', 'A', 'B'};
const char noteNames2[12] = {' ', '#', ' ', '#', ' ', ' ', '#', ' ', '#', ' ', '#', ' '};

void setup() {
  note1 = EEPROM.read(1);
  note2 = EEPROM.read(2);
  note3 = EEPROM.read(3);
  note4 = EEPROM.read(4);
  note5 = EEPROM.read(5);
  velocity = EEPROM.read(6);
  baudSet = EEPROM.read(7);

  note1Old = note1;
  note2Old = note2;
  note3Old = note3;
  note4Old = note4;
  note5Old = note5;
  velocityOld = velocity;
  baudSetOld = baudSet;

  pinMode(encoder0PinA, INPUT_PULLUP);
  pinMode(encoder0PinB, INPUT_PULLUP);
  pinMode(clk, INPUT_PULLUP);

  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);

  attachInterrupt(0, doEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setRotation(2);
  display.setTextColor(WHITE);

  pinMode( rxPin, INPUT_PULLUP );
  pinMode( txPin, OUTPUT);

  if (baudSet == 1) {
    Serial.begin(115200);
  }
  if (baudSet == 2) {
    Serial.begin(31250);
  }
  display.setCursor(30, 20);
  display.setTextSize(1);
  display.println("yyRobotics");
  display.setCursor(15, 40);
  display.println("MIDI Controller");
  display.display();
  delay(3000);

  display.clearDisplay();

  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 2) {
    display.drawCircle(display.width() / 2, display.height() / 2, i, WHITE);
    display.display();
    delay(1);
  }


}

void loop() {
  if (selectedVal != oldVal || yVals != yValsOld || xVals != yValsOld) {
    display.clearDisplay();
    oldVal = selectedVal;
    yValsOld = yVals;
    xValsOld = xVals;
    valKeep();
  }

  buttonCheck();

  if (digitalRead(10) == LOW) {
    b5Val = noteON;
  } else {
    b5Val = noteOFF;
  }
  if (digitalRead(8) == LOW) {
    b1Val = noteON;
  } else {
    b1Val = noteOFF;
  }
  if (digitalRead(7) == LOW) {
    b2Val = noteON;
  } else {
    b2Val = noteOFF;
  }
  if (digitalRead(6) == LOW) {
    b3Val = noteON;
  } else {
    b3Val = noteOFF;
  }
  if (digitalRead(5) == LOW) {
    b4Val = noteON;
  } else {
    b4Val = noteOFF;
  }

  //Joystick starts here------------------------
  xVal = analogRead(A6);
  xVal = map(xVal, 0, 1023, 0, 127);
  xVals = map(xVal, 127, 0, 121, 80);

  yVal = analogRead(A7);
  yVal = map(yVal, 0, 1023, 0, 127);
  yVals = map(yVal, 127, 0, 0, 40);
  //Joystick ends here--------------------------

  aVal = analogRead(A3);
  aVal = map(aVal, 0, 1022, 0, 127);
  aVals = map(aVal, 0, 127, 0, 70);

  bVal = analogRead(A2);
  bVal = map(bVal, 0, 1022, 0, 127);
  bVals = map(bVal, 0, 127, 0, 70);

  cVal = analogRead(A1);
  cVal = map(cVal, 0, 1022, 0, 127);
  cVals = map(cVal, 0, 127, 0, 70);

  dVal = analogRead(A0);
  dVal = map(dVal, 0, 1022, 0, 127);
  dVals = map(dVal, 0, 127, 0, 70);

  //-XY Plotter---------------------------
  display.drawRect(80, 0, 48, 48, WHITE);
  display.setCursor(xVals, yVals);
  display.println(".");
  //-XY plotter ends----------------------
  //-Val Control--------------------------
  display.setTextSize(2);
  display.setTextColor(WHITE);
  if (ch < 6) {

    //- Pot Val----------------------------
    display.drawRect(0, 16, 70, 6, WHITE);
    display.fillRect(0, 16, aVals, 6, INVERSE);
    //- Pot Val----------------------------
    display.drawRect(0, 24, 70, 6, WHITE);
    display.fillRect(0, 24, bVals, 6, INVERSE);
    //- Pot Val---------------------.
    display.drawRect(0, 32, 70, 6, WHITE);
    display.fillRect(0, 32, cVals, 6, INVERSE);
    //- Pot Val---------------------.
    display.drawRect(0, 40, 70, 6, WHITE);
    display.fillRect(0, 40, dVals, 6, INVERSE);

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println(ch);
    display.setCursor(20, 0);
    display.println(":");

    display.setCursor(40, 0);
    display.println(selectedVal);
    //-Val Control-----------------------
  } else if (ch == 6) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Velocity");

    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println(velocity);

    if (velocityOld == velocity) {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Saved");
    } else {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Unsaved");
      if (digitalRead(6) == LOW && digitalRead(5) == LOW) {
        EEPROM.write(6, velocity);
        delay(20);
        velocityOld = velocity;
      }
    }
  } else if (ch == 7) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Button 1");
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println(note1);

    display.setTextSize(1);
    display.setCursor(35, 20);
    display.println(noteNames[note1 % 12]);
    display.setCursor(45, 20);
    display.println(noteNames2[note1 % 12]);
    //-------------------------------------------------------------------------------
    if (note1Old == note1) {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Saved");
    } else {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Unsaved");
      if (digitalRead(6) == LOW && digitalRead(5) == LOW) {
        EEPROM.write(1, note1);
        delay(20);
        note1Old = note1;
      }
    }
    //------------------------------------------------------------------------------
  } else if (ch == 8) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Button 2");
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println(note2);

    display.setTextSize(1);
    display.setCursor(35, 20);
    display.println(noteNames[note2 % 12]);
    display.setCursor(45, 20);
    display.println(noteNames2[note2 % 12]);

    if (note2Old == note2) {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Saved");
    } else {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Unsaved");
      if (digitalRead(6) == LOW && digitalRead(5) == LOW) {
        EEPROM.write(2, note2);
        delay(20);
        note2Old = note2;
      }
    }
  } else if (ch == 9) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Button 3");
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println(note3);

    display.setTextSize(1);
    display.setCursor(35, 20);
    display.println(noteNames[note3 % 12]);
    display.setCursor(45, 20);
    display.println(noteNames2[note3 % 12]);

    if (note3Old == note3) {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Saved");
    } else {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Unsaved");
      if (digitalRead(6) == LOW && digitalRead(5) == LOW) {
        EEPROM.write(3, note3);
        delay(20);
        note3Old = note3;
      }
    }
  } else if (ch == 10) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Button 4");
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println(note4);

    display.setTextSize(1);
    display.setCursor(35, 20);
    display.println(noteNames[note4 % 12]);
    display.setCursor(45, 20);
    display.println(noteNames2[note4 % 12]);

    if (note4Old == note4) {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Saved");
    } else {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Unsaved");
      if (digitalRead(6) == LOW && digitalRead(5) == LOW) {
        EEPROM.write(4, note4);
        delay(20);
        note4Old = note4;
      }
    }
  } else if (ch == 11) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Button 5");
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println(note5);

    display.setTextSize(1);
    display.setCursor(35, 20);
    display.println(noteNames[note5 % 12]);
    display.setCursor(45, 20);
    display.println(noteNames2[note5 % 12]);

    if (note5Old == note5) {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Saved");
    } else {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Unsaved");
      if (digitalRead(6) == LOW && digitalRead(5) == LOW) {
        EEPROM.write(5, note5);
        delay(20);
        note5Old = note5;
      }
    }
  }  else if (ch == 12) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Output");
    display.setTextSize(2);

    display.setCursor(0, 20);

    if (baudNum > 10) {
      display.println("USB");
      baudSet = 1;
    } else {
      display.println("MIDI");
      baudSet = 2;
    }

    if (baudSet == baudSetOld) {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Saved");
    } else {
      display.setTextSize(1);
      display.setCursor(35, 30);
      display.println("Unsaved");
      if (digitalRead(6) == LOW && digitalRead(5) == LOW) {
        EEPROM.write(7, baudSet);
        delay(20);
        baudSetOld = baudSet;
      }
    }
  }
  //Velocity Value View
  display.setTextSize(1);
  display.setCursor(70, 50);
  display.println("V:");
  display.setCursor(80, 50);
  display.println(velocity);
  //Output view
  display.setCursor(102, 50);
  if (baudSet == 1) {
    display.println("USB");
  } else {
    display.println("MIDI");
  }

  //-Enable Disable Control-------
  display.setCursor(0, 50);

  if (enabled == false) {
    flag1 = false;

    if (flag2 == false)
    {
      tone(3, 440, 100);
      flag2 = true;
    }

    display.println("Edit");
    countLimit = 12;
    flag1 = false;

    //Octave Control ------------------------
    if (digitalRead(8) == LOW) {
      octave = 0;
      octaveBank = 'A';
    }
    if (digitalRead(7) == LOW) {
      octave = 12;
      octaveBank = 'B';
    }
    if (digitalRead(6) == LOW) {
      octave = 24;
      octaveBank = 'C';
    }
    if (digitalRead(5) == LOW) {
      octave = 36;
      octaveBank = 'D';
    }
    //Octave Control--------------------------

    //Enabled Section -------------
  } else if (enabled == true) {
    countLimit = 5;
    flag2 = false;

    if (flag1 == false)
    {
      ch = 1;
      tone(3, 880, 100);
      flag1 = true;
    }
    //Enable Section End-----------

    display.println("Run!");
    midiMsg();
  }

  display.setCursor(30, 50);
  display.println("Bank:");
  display.setCursor(60, 50);
  display.println(octaveBank);
  display.display();
}

void MIDImessage(byte command, byte data1, byte data2)
{
  Serial.write(command);
  Serial.write(data1);
  Serial.write(data2);

}

void midiMsg() {
  if (val1 != val1Last) // If the value does not = the last value the following command is made. This is because the pot has been turned. Otherwise the pot remains the same and no midi message is output.
  {
    MIDImessage(176, 1, val1);
  }         // 176 = CC command (channel 1 control change), 1 = Which Control, val = value read from Potentionmeter 1 NOTE THIS SAYS VAL not VA1 (lowercase of course)
  val1Last = val1;
  //---
  if (val2 != val2Last)
  {
    MIDImessage(176, 2, val2);
  }
  val2Last = val2;
  //---
  if (val3 != val3Last)
  {
    MIDImessage(176, 3, val3);
  }
  val3Last = val3;
  //---
  if (val4 != val4Last)
  {
    MIDImessage(176, 4, val4);
  }
  val4Last = val4;
  //---
  if (val5 != val5Last)
  {
    MIDImessage(176, 5, val5);
  }
  val5Last = val5;
  //---

  //--X Y Value to MIDI------------------------------
  if (xVal != xValLast)
  {
    MIDImessage(176, 11, xVal);
  }
  xValLast = xVal;
  //------------------
  if (yVal != yValLast)
  {
    MIDImessage(176, 12, yVal);
  }
  yValLast = yVal;

  //------------------
  if (aVal != aValLast)
  {
    MIDImessage(176, 13, aVal);
  }
  aValLast = aVal;
  //------------------
  if (bVal != bValLast)
  {
    MIDImessage(176, 14, bVal);
  }
  bValLast = bVal;
  //------------------
  if (cVal != cValLast)
  {
    MIDImessage(176, 15, cVal);
  }
  cValLast = cVal;
  //------------------
  if (dVal != dValLast)
  {
    MIDImessage(176, 16, dVal);
  }
  dValLast = dVal;
  //Button Section -------------------------------------
  if (b1Val != b1ValLast)
  {
    MIDImessage(b1Val, note1 + octave, velocity);
  }
  b1ValLast = b1Val;

  if (b2Val != b2ValLast)
  {
    MIDImessage(b2Val, note2 + octave, velocity);
  }
  b2ValLast = b2Val;

  if (b3Val != b3ValLast)
  {
    MIDImessage(b3Val, note3 + octave, velocity);
  }
  b3ValLast = b3Val;

  if (b4Val != b4ValLast)
  {
    MIDImessage(b4Val, note4 + octave, velocity);
  }
  b4ValLast = b4Val;

  if (b5Val != b5ValLast)
  {
    MIDImessage(b5Val, note5 + octave, velocity);
  }
  b5ValLast = b5Val;
}

void buttonCheck() {
  if (digitalRead(clk) == LOW) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      enabled = !enabled;
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {

        if (ch < countLimit) {
          ch++;
          display.clearDisplay();
        } else {
          ch = 1;
          display.clearDisplay();
        }
        valGet();
        inc = selectedVal;
      }
      buttonActive = false;
    }
  }
}

void valGet() {
  switch (ch) {
    case 1:
      selectedVal = val1;
      break;
    case 2:
      selectedVal = val2;
      break;
    case 3:
      selectedVal = val3;
      break;
    case 4:
      selectedVal = val4;
      break;
    case 5:
      selectedVal = val5;
      break;
    case 6:
      selectedVal = velocity;
      break;
    case 7:
      selectedVal = note1;
      break;
    case 8:
      selectedVal = note2;
      break;
    case 9:
      selectedVal = note3;
      break;
    case 10:
      selectedVal = note4;
      break;
    case 11:
      selectedVal = note5;
      break;
    case 12:
      selectedVal = baudNum;
      break;
  }
}

void valKeep() {
  switch (ch) {
    case 1:
      val1 = selectedVal;
      break;
    case 2:
      val2 = selectedVal;
      break;
    case 3:
      val3 = selectedVal;
      break;
    case 4:
      val4 = selectedVal;
      break;
    case 5:
      val5 = selectedVal;
      break;
    case 6:
      velocity = selectedVal;
      break;
    case 7:
      note1 = selectedVal;
      break;
    case 8:
      note2 = selectedVal;
      break;
    case 9:
      note3 = selectedVal;
      break;
    case 10:
      note4 = selectedVal;
      break;
    case 11:
      note5 = selectedVal;
      break;
    case 12:
      baudNum = selectedVal;
      break;
  }
}
void doEncoder()
{
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB))
  {
    if (selectedVal < 127) {
      inc = inc + 0.5;
    }
  }
  else
  {
    if (selectedVal > 0) {
      inc = inc - 0.5;
    }
  }
  selectedVal = inc;
}
