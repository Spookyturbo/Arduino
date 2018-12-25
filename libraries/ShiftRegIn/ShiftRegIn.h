#ifndef ShiftRegIn_h
#define ShiftRegIn_h

class ShiftRegIn {
	
	public:
		ShiftRegIn(uint8_t loadPin, uint8_t clkPin, uint8_t dataPin);
		bool getButtonDown(uint8_t buttonNumber);
		bool getButton(uint8_t buttonNumber);
		bool getButtonUp(uint8_t buttonNumber);
		void updateButtonState(void);
	private:
		uint8_t _currentState;
		uint8_t _previousState;
		uint8_t _loadPin;
		uint8_t _clkPin;
		uint8_t _dataPin;
	
};

#endif