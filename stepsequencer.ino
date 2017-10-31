#include <EEPROM.h>
int buttonPins[4] = {29, 30, 31, 32};
boolean lastButtonStates[4] = { LOW, LOW, LOW, LOW };
int ledPins[4] = {35, 36, 37, 38};
boolean buttonStates[4] = {LOW, LOW, LOW, LOW};
boolean stepState[3][4] = {{false, false, false, false},
  {false, false, false, false},
  {false, false, false, false}
};
int tempo = 100;
int currentStep = 0;
unsigned long lastStepTime = 0;
int toggleUpPin = 18;
int toggleDownPin = 17;
int stepper = 0;
int lastStepperState = 0;
int upState = LOW;
int downState = LOW;


void setup() {
  memoryReader();
  
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(A5, INPUT);
  pinMode(toggleUpPin, INPUT);
  pinMode(toggleDownPin, INPUT);
}

void loop() {
  for (int i = 0; i < 4; i++) {
    checkButtons(i);
    setLeds();
    sequence();
    int lastUpState = upState;
    int lastDownState = downState;
    upState = digitalRead(toggleUpPin);
    downState = digitalRead(toggleDownPin);
    if (upState == HIGH && lastUpState == LOW) {
      if (stepper < 2) {
        stepper++;

      }
      Serial.println(stepper);
      lastUpState = upState;
      lastStepperState = stepper;
    }
    if (downState == HIGH && lastDownState == LOW) {
      if (stepper != 0) {
        stepper--;
      }
      Serial.println(stepper);
      lastDownState = downState;
      lastStepperState = stepper;
    }


  }
}

void sequence() {
  tempo = analogRead(A5); //sets tempo based on pot
  if (millis() > lastStepTime + tempo) {
    //if its time to go to the next step...
    // analogWrite(ledPins[currentStep], 0);  //turn off the current led
    currentStep = currentStep + 1;         //increment to the next step
    if (currentStep > 3) {
      currentStep = 0; //keeps max of 4 steps
    }
    if (stepState[0][currentStep] == true) {
      usbMIDI.sendNoteOn(37, 100, 1); //if the LED for the respective step is on, trigger a MIDI note
      usbMIDI.sendNoteOff(37, 0, 1);
    }
    if (stepState[1][currentStep] == true) {
      usbMIDI.sendNoteOn(38, 100, 1); //if the LED for the respective step is on, trigger a MIDI note
      usbMIDI.sendNoteOff(38, 0, 1);
    }
    if (stepState[2][currentStep] == true) {
      usbMIDI.sendNoteOn(39, 100, 1); //if the LED for the respective step is on, trigger a MIDI note
      usbMIDI.sendNoteOff(39, 0, 1);
    }
    //analogWrite(ledPins[currentStep], 10); //turn on the new led

    lastStepTime = millis();               //set lastStepTime to the current time

  }
}
void checkButtons(int x) {
  lastButtonStates[x] = buttonStates[x];
  buttonStates[x] = digitalRead(buttonPins[x]);
  if (buttonStates[x] == HIGH && lastButtonStates[x] == LOW) {
    if (stepState[stepper][x] == false) {
      stepState[stepper][x] = true;
      
    } else if (stepState[stepper][x] == true) {
      stepState[stepper][x] = false;
    }
    memoryWriter();
  }t
}

void setLeds() {
  for (int x = 0; x < 4; x++) {
    if (currentStep == x) {
      analogWrite(ledPins[currentStep], 10); //if the current LED has not had its button pressed, set the brightness to 10 while it's on that step

    }
    else if (stepState[stepper][x] == HIGH) {
      analogWrite(ledPins[x], 100);   //if the current LED has had its button pressed, always set it to 100

    }
    else {
      analogWrite(ledPins[x], 0); //if the LED hasn't been pressed and it's not the current step, turn it off

    }
  }
}
void memoryWriter() {
  int memCount = 0;
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 4; i++) {
      EEPROM.write(memCount, stepState[j][i]);
      Serial.print(EEPROM.read(memCount));
      memCount++;
    }
    
    Serial.println("");
    
  }
  Serial.println("");
}

void memoryReader() {
  int memCount = 0;
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 4; i++) {
      stepState[j][i] = EEPROM.read(memCount);
      Serial.print(stepState[j][i]);
      memCount++;
    }
    
    Serial.println("");
  }
  Serial.println("");
}
