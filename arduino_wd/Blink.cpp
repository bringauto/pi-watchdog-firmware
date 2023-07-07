#include "Blink.hpp"

#include <Arduino.h>



namespace bringauto::watchdog {
void Blink::update(unsigned int cycleTime) {
	if(isDisabled()) {
		return;
	}

	stateTime_ += cycleTime;

	if(isOn_) {
		if(stateTime_ >= currentBlinkTime_.onTime) {
			stateTime_ = 0;
			isOn_ = false;
		}
	} else {
		if(stateTime_ >= currentBlinkTime_.offTime) {
			stateTime_ = 0;
			isOn_ = true;
		}
	}

	digitalWrite(PIN_LED, isOn_);
}

void Blink::setBlinkTime(const BlinkTime &blinkTime) {
	currentBlinkTime_ = blinkTime;
}

bool Blink::isDisabled() {
	return (currentBlinkTime_.onTime == 0 || currentBlinkTime_.offTime == 0);
}
}