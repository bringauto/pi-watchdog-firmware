#include "Timer.hpp"

#include <Arduino.h>
#include <climits>



namespace bringauto::watchdog {

void Timer::update() {
	timestampLast_ = timestampCurrent_;
	timestampCurrent_ = millis();

	///mills() overflow fix
	if(timestampCurrent_ < timestampLast_) {
		cycleTime_ = (ULONG_MAX - timestampLast_) + timestampCurrent_;
	} else {
		cycleTime_ = timestampCurrent_ - timestampLast_;
	}
}

unsigned int Timer::getCycleTime() {
	return cycleTime_;
}
}
