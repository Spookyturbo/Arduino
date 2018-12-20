#include "Arduino.h"
#include "Piezo.h"

Piezo::Piezo(int pin) {
	_pin = pin;
	pinMode(_pin, OUTPUT);
}

Piezo::tone(int frequency, uint16_t duration) {
	tone(_pin, frequency, duration);
	currentNote = -1;
}

//This must be constantly called to play the whole song. This is to prevent this from halting the program until the song finishes
Piezo::playSong(int[] melody, int[] durations, int melodyLength) {
	static long lastNoteTime;
	
	//Initial note condition
	if(currentNote == -1 || millis() - lastNoteTime >= durations[currentNote]) {
		currentNote++;
		if(currentNote >= melodyLength) {
			currentNote = -1;
			return;
		}
		lastNoteTime = millis();
		tone(_pin, melody[currentNote], durations[currentNote]);
	}
	
}

Piezo::resetSong() {
	currentNote = -1;
}