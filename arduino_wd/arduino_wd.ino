#include "Config.h"

#include "Watchdog.hpp"
#include "PwmController.hpp"

#include <climits>

extern "C" {
#include <hardware/watchdog.h>
};

/**
 * Watchdog
 */
bringauto::watchdog::WatchdogSettings watchdogSettings {
		RPI_BOOT_TIME_MS,
		RESPONSE_TIME_LIMIT_MS,
		REBOOT_DISCHARGE_TIME_MS,
		bringauto::watchdog::BlinkTime(BLINK_ON_OS_BOOTING_MS, BLINK_OFF_OS_BOOTING_MS),
		bringauto::watchdog::BlinkTime(BLINK_ON_OS_RUNNING_MS, BLINK_OFF_OS_RUNNING_MS),
		bringauto::watchdog::BlinkTime(BLINK_ON_POWER_OFF_MS, BLINK_OFF_POWER_OFF_MS)
};
bringauto::watchdog::Watchdog watchdog;

/**
 * PWM Selector
 */
bringauto::watchdog::PwmSettings pwmSettings {
	PWM_UPDATE_RATE_MS,
	PWM_HYSTERESIS,
	PWM1_THRESHOLDS,
	PWM2_THRESHOLDS
};
bringauto::watchdog::PwmController pwmController;

/// CORE 0  - for watchdog logic
void setup() {
	watchdog.init(watchdogSettings);

	///Watchdog for Pico stuck after under voltage
	watchdog_enable(PICO_TIMEOUT_MS, 1);
}

void loop() {
	watchdog.update();
	watchdog_update();
}

/// CORE 1  - for PWM logic
void setup1() {
	pwmController.init(pwmSettings);

	Serial.begin(9600);
}

void loop1() {
	pwmController.update();
}
