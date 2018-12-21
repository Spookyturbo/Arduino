#include "Arduino.h"
#include "Piezo.h"

Piezo::Piezo(int pin) {
	_pin = pin;
	pinMode(_pin, OUTPUT);
	currentNote = -1;
}

void Piezo::playTone(int frequency, uint16_t duration) {
	tone(_pin, frequency, duration);
}

//This must be constantly called to play the whole song. This is to prevent this from halting the program until the song finishes
void Piezo::playSong(int melody[], int durations[], int melodyLength) {
	static long lastNoteTime;
	int noteDuration = 2000 / durations[currentNote];
	//Initial note condition
	if(currentNote == -1 || millis() - lastNoteTime >= noteDuration) {
		currentNote++;
		if(currentNote >= melodyLength) {
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