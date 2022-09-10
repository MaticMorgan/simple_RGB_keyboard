#include "pitches.h"

 // declaration of constants //
const byte pwr_pin = 2;
const byte mode_pin = 3;
const byte brightness_pin = A0;
const byte light_spd_pin = A1;
const byte led = 9;
const byte spkr_pin = A4;
const int note_duration = 500;
const int note_pause = 1000;

 // declaration of variables //
byte brightness = 255;
byte light_spd = 0;
byte stp = 0;
byte random_count = 0;
bool dir = true;
volatile bool mode_change = false;
volatile bool pwr_change = false;
volatile byte mode = 0;
volatile bool pwr = true;

 // arrays of notes for different melodies //
int melody_mode[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int melody_power_on[] = {
  NOTE_G5, NOTE_C6, NOTE_E6,
  NOTE_G6, 0, NOTE_E6, NOTE_G6
  
};

int melody_power_off[] = {
  NOTE_GS7, NOTE_DS7, NOTE_GS6, NOTE_AS6
};

 // arrays of note durations for different melodies //
int note_durations_mode[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

int note_durations_power_on[] = {
  3, 3, 3, 4, 24, 6, 1
};

int note_durations_power_off[] = {
  3, 3, 3, 2
};


 // three functions that play different melodies //
void PowerOnMelody() {
  for (int thisNote = 0; thisNote < 7; thisNote++) {
    int noteDuration = 1000 / note_durations_power_on[thisNote];
    tone(spkr_pin, melody_power_on[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(spkr_pin);
  }
}

void PowerOffMelody() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 500 / note_durations_power_off[thisNote];
    tone(spkr_pin, melody_power_off[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(spkr_pin);
  }
}

void ModeMelody() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / note_durations_mode[thisNote];
    tone(spkr_pin, melody_mode[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(spkr_pin);
  }
}

 // interrupt function to change the lights mode //
void mode_interrupt() {
  mode++;
  mode_change = true;
}

 // interrupt function to change power status //
void pwr_interrupt() {
  pwr = !pwr;
  pwr_change = true;
}

 // this function adjusts the LEDs for the correct mode, correct brightness and for the n-th step in the cycle //
void NthStep(byte mode, byte brightness, byte stp){

    // if the step has incremented to the highest value, change the dirrection for cases 0 and 1 //
   if (stp == 255) {
      dir = !dir;
      }

   // all possible lightning modes //
  switch (mode) {

     // growing and falling lights //
    case 0:
      if (dir == true) {
        analogWrite(led, stp);
      }
      else {
        analogWrite(led, 255 - stp);
      }
      break;

     // random blinking with a 1/3 change of being turned off //
    case 1:
        if (random(3)) {
          analogWrite(led, 0);
        }
        else {
          analogWrite(led, brightness);
        }
      break;

     // static lightning //
    default:
      analogWrite(led, brightness);
      break;
  }
}


void setup() {

   // declaring the pin modes //
  pinMode(brightness_pin, INPUT);
  pinMode(light_spd_pin, INPUT);
  pinMode(pwr_pin, INPUT_PULLUP);
  pinMode(mode_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pwr_pin), pwr_interrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(mode_pin), mode_interrupt, FALLING);
  pinMode(led, OUTPUT);
  pinMode(spkr_pin, OUTPUT);

   // play the "power on" melody //
  PowerOnMelody();
    
}


void loop() {

  delay(light_spd);
  light_spd = analogRead(light_spd_pin);
  brightness = analogRead(brightness_pin);
  
     // check if the power button was pressed and act accordingly //
  if (pwr_change == true) {
    if (pwr == true) {
      PowerOnMelody();
    }
    else {
      PowerOffMelody();
    }
    pwr_change = false;
  }

  if (pwr == false) {
    analogWrite(led, 0);
  }
  
  else {
    
     // check if the mode button was pressed and act accordingly //    
    if (mode_change == true) {
      ModeMelody();
      mode_change = false;
    }
    
    mode = mode % 3;
    stp++ % 256;
    NthStep(mode, brightness, stp);
  }
}
