#include "Watchdog.hpp"
#include "PinoutConfig.h"

#include <Arduino.h>
#include <climits>



namespace bringauto::watchdog {
void Watchdog::init(const WatchdogSettings &settings) {
	settings_ = settings;

	///Setup GPIO
	pinMode(PIN_POWER_OFF, OUTPUT);
	digitalWrite(PIN_POWER_OFF, true);

	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_WD_ENABLED, INPUT_PULLUP);

	pinMode(PIN_SLAVE, INPUT_PULLDOWN);
	pinMode(PIN_MASTER, OUTPUT);
	digitalWrite(PIN_MASTER, true);
}

void Watchdog::update() {
	///Timer needs to be updated even when WD is disabled - if not updating, it mostly reboots RP4 after reactivation
	timer_.update();

	if(digitalRead(PIN_WD_ENABLED)) {
		blink_.update(timer_.getCycleTime());
		stateTime_ += timer_.getCycleTime();

		updateState();
	} else {
		digitalWrite(PIN_LED, false);
		stateTime_ = 0;
		return;
	}
}

void Watchdog::updateState() {
	switch(state_) {
		case WatchdogState::OS_BOOTING:
			if(stateTime_ < settings_.timeout_OsBooting) {
				if(digitalRead(PIN_SLAVE)) {
					changeState(WatchdogState::OS_RUNNING);
				}
			} else {
				reset();
			}
			break;

		case WatchdogState::OS_RUNNING:
			if(stateTime_ < settings_.timeout_OsRunning) {
				if(digitalRead(PIN_MASTER) == digitalRead(PIN_SLAVE)) {
					reply();
				}
			} else {
				reset();
			}
			break;

		case WatchdogState::POWER_OFF:
			if(stateTime_ < settings_.timeout_PowerOff) {
				digitalWrite(PIN_LED, true);
			} else {
				changeState(WatchdogState::OS_BOOTING);
				digitalWrite(PIN_POWER_OFF, true);
			}
			break;

		case WatchdogState::INVALID:
		default:
			reset();
			break;
	}
}

void Watchdog::reply() {
	digitalWrite(PIN_MASTER, !digitalRead(PIN_SLAVE));
	stateTime_ = 0;
}

void Watchdog::changeState(const WatchdogState &state) {
	state_ = state;
	stateTime_ = 0;

	switch(state_) {
		case WatchdogState::OS_BOOTING:
			blink_.setBlinkTime(settings_.blinkTime_OsBooting);
			break;

		case WatchdogState::OS_RUNNING:
			blink_.setBlinkTime(settings_.blinkTime_OsRunning);
			break;

		case WatchdogState::POWER_OFF:
			blink_.setBlinkTime(settings_.blinkTime_PowerOff);
			break;

		case WatchdogState::INVALID:
		default:
			state_ = WatchdogState::POWER_OFF;
			blink_.setBlinkTime(settings_.blinkTime_PowerOff);
			break;
	}
}

void Watchdog::reset() {
	digitalWrite(PIN_POWER_OFF, false);
	digitalWrite(PIN_MASTER, true);

	changeState(WatchdogState::POWER_OFF);
}
}