#include "Arduino.h"
#include "Piezo.h"

Piezo::Piezo(int pin) {
	_pin = pin;
	pinMode(_pin, OUTPUT);
	currentNote = -1;
}

void Piezo::playTone(uint16_t frequency, uint16_t duration) {
	tone(_pin, frequency, duration);
}

//This must be constantly called to play the whole song. This is to prevent this from halting the program until the song finishes
//The note multiplier is used so if the song is too fast or slow you can adjust it with a simple number > 1 = Speed up < 1 = slow down
void Piezo::playSong(uint16_t melody[], uint16_t durations[], uint16_t melodyLength, float noteMultiplier) {
	static long lastNoteTime;
	int noteDuration = 1000 / (durations[currentNote] * noteMultiplier);
	//Initial note condition                               
	if(currentNote == -1 || millis() - lastNoteTime >= noteDuration + 1) {//If you remove this +1 this will break. Think its because
		currentNote++;													  //Think its because there is a minimum break required between changing frequency
		if(currentNote >= melodyLength) {								  //And an input of note duration 16 happens to be 1 ms too fast
			currentNote = -1;
			return;
		}
		lastNoteTime = millis();
		playTone(melody[currentNote], noteDuration);
	}
	
}

void Piezo::resetSong() {
	currentNote = -1;
}