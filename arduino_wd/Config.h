#pragma once

#include "PwmController.hpp"

#include <map>
#include <string>
/**
 WATCHDOG SETTINGS
 **/

/// PICO CONSTANTS
const unsigned int PICO_TIMEOUT_MS = 10000;                /// Time it takes to reboot Pico if get stuck

/// TIMEOUTS CONSTANTS
const unsigned int RPI_BOOT_TIME_MS = 90000;            /// Time it takes to boot RPi
const unsigned int RESPONSE_TIME_LIMIT_MS = 5000;    /// Expected response time from rpi - 5 msg 1s apart
const unsigned int REBOOT_DISCHARGE_TIME_MS = 3000;     /// Time to wait between MOSFET off and MOSFET on

/// BLINK CONSTANTS
const unsigned int BLINK_ON_OS_BOOTING_MS = 250;        /// Onboard LED ON time - state RPi4 booting
const unsigned int BLINK_OFF_OS_BOOTING_MS = 250;        /// Onboard LED OFF time - state RPi4 booting

const unsigned int BLINK_ON_OS_RUNNING_MS = 250;        /// Onboard LED ON time - state RPi4 OS running
const unsigned int BLINK_OFF_OS_RUNNING_MS = 750;        /// Onboard LED OFF time - state RPi4 OS Running

const unsigned int BLINK_ON_POWER_OFF_MS = 0;            /// Onboard LED ON time - state power to RPi4 is cut off
const unsigned int BLINK_OFF_POWER_OFF_MS = 0;          /// Onboard LED OFF time - state power to RPi4 is cut off

/**
 PWM CONTROLLER SETTINGS
 **/

const unsigned int PWM_UPDATE_RATE_MS = 2500;            /// Update rate of PWM
const float PWM_HYSTERESIS = 5.0f;                        /// PWM hysteresis (in °C)
/// PWM thresholds for PWM 1 <lower limit °c, upper limit °c, selected output level>
const std::vector <bringauto::watchdog::PwmThreshold> PWM1_THRESHOLDS = {
		{ -100.0f, 20.0f,  bringauto::watchdog::OutputPWMLevel::OFF },
		{ 20.0f,   40.0f,  bringauto::watchdog::OutputPWMLevel::LOW },
		{ 40.0f,   100.0f, bringauto::watchdog::OutputPWMLevel::HIGH }
};
/// PWM thresholds for PWM 2 <lower limit °c, upper limit °c, selected output level>
const std::vector <bringauto::watchdog::PwmThreshold> PWM2_THRESHOLDS = {
		{ -100.0f, 20.0f,  bringauto::watchdog::OutputPWMLevel::OFF },
		{ 20.0f,   30.0f,  bringauto::watchdog::OutputPWMLevel::LOW },
		{ 30.0f,   100.0f, bringauto::watchdog::OutputPWMLevel::HIGH }
};

/// PWM duty cycle calculation parameters for 5V
const bringauto::watchdog::PwmDutyCycleParameters PWM_5V_OUT_PARAMS { .a = -0.00007f, .b = -0.38f, .c = -0.25f, .d = 1.0f };
/// PWM duty cycle calculation parameters for 8V
const bringauto::watchdog::PwmDutyCycleParameters PWM_8V_OUT_PARAMS { .a = 0.000644f, .b = -0.434f, .c = -0.141f, .d = 1.0f };
/// PWM duty cycle calculation parameters for 10V
const bringauto::watchdog::PwmDutyCycleParameters PWM_10V_OUT_PARAMS { .a = -0.00168f, .b = -0.501f, .c = -0.145f, .d = 1.0f };
/// PWM duty cycle calculation parameters for 12V
const bringauto::watchdog::PwmDutyCycleParameters PWM_12V_OUT_PARAMS { .a = 0.0043f, .b = -0.644f, .c = -0.133f, .d = 1.0f };

///mapping of OutputPWMLevel to string representation
const std::map<bringauto::watchdog::OutputPWMLevel, const char *> outputPwmLvlToString {
		{ bringauto::watchdog::OutputPWMLevel::OFF,  "OFF" },
		{ bringauto::watchdog::OutputPWMLevel::LOW,  "LOW" },
		{ bringauto::watchdog::OutputPWMLevel::HIGH, "HIGH" },
};
/**
  ADC params
 */
const float ADC_V_REF = 3.34f;      /// External reference voltage of ADC
const int ADC_RES = 12;             /// ADC resolution bits