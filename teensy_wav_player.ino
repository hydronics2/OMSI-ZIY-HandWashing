// For healthy hand washing
// 6/8/2015
// when pin 2 is pulled LOW, the sound is played "water.wav"
// the audio file needs to be 16bit, 44.1khz, 






//
// Requires the audio shield:
//   http://www.pjrc.com/store/teensy3_audio.html
//
// Data files to put on your SD card can be downloaded here:
//   http://www.pjrc.com/teensy/td_libs_AudioDataFiles.html
//
// This example code is in the public domain.

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioPlaySdWav           playWav1;       //xy=154,78
AudioOutputI2S           i2s1;           //xy=334,89
AudioConnection          patchCord1(playWav1, 0, i2s1, 0);
AudioConnection          patchCord2(playWav1, 1, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=240,153
// GUItool: end automatically generated code

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  digitalWrite(2, HIGH);
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(5);

  sgtl5000_1.enable();
  sgtl5000_1.volume(.5);

  SPI.setMOSI(7);
  SPI.setSCK(14);
  if (!(SD.begin(10))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);

  // Simply wait for the file to finish playing.
//  while (playWav1.isPlaying()) {
  
    //while (digitalRead(2) == LOW) {
    //delay(20);
  //}
}


void loop() {
  if (digitalRead(2) == LOW) {
  playWav1.play("water.wav");

  // A brief delay for the library read WAV info
  delay(5);

    while (digitalRead(2) == LOW && playWav1.isPlaying()) {
      delay(20);
    }
}

  if (digitalRead(2) == HIGH) {
  playWav1.stop();
}}

