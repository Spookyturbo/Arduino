#include "Arduino.h"
#include "ShiftRegIn.h"

ShiftRegIn::ShiftRegIn(uint8_t loadPin, uint8_t clkPin, uint8_t dataPin) {
	_loadPin = loadPin;
	_clkPin = clkPin;
	_dataPin = dataPin;
	
	updateButtonState();
	
	pinMode(_dataPin, INPUT);
	pinMode(_loadPin, OUTPUT);
	pinMode(_clkPin, OUTPUT);
	
	digitalWrite(_clkPin, HIGH);
	digitalWrite(_loadPin, HIGH);
}

//Used so that when checking multiple buttons
//the shift reg doesn't need loaded multiple times
//causing significant delay
void ShiftRegIn::updateButtonState() {
	_previousState = _currentState;
	
	digitalWrite(_clkPin, HIGH);
	digitalWrite(_loadPin, LOW);
	delayMicroseconds(5);
	digitalWrite(_loadPin, HIGH);
	
	_currentState = shiftIn(_dataPin, _clkPin, MSBFIRST);
	
}

//Button number = 0-7
bool ShiftRegIn::getButtonDown(uint8_t buttonNumber) {
	//Not sure if the Pow function works right now
	buttonNumber = pow(2, buttonNumber);
	bool previousState = _previousState & buttonNumber;
	bool currentState = _currentState & buttonNumber;
	
	if(currentState && !previousState)
		return true;
	
	return false;
}

bool ShiftRegIn::getButton(uint8_t buttonNumber) {
	buttonNumber = pow(2, buttonNumber);
	
	return _currentState & buttonNumber;
}

bool ShiftRegIn::getButtonUp(uint8_t buttonNumber) {
	buttonNumber = pow(2, buttonNumber);
	
	bool previousState = _previousState & buttonNumber;
	bool currentState = _currentState & buttonNumber;
	
	if(previousState & !currentState)
		return true;
	
	return false;
}
