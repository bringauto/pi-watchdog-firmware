#pragma once

#include "Timer.hpp"
#include "PinoutConfig.h"

#include <vector>



namespace bringauto::watchdog {
/***
 * @brief Output pwm level
 */
enum class OutputPWMLevel {
	INVALID = -1,
	OFF,	///pwm off
	LOW,	///pwm low level
	HIGH	///pwm high level
};

/**
 * @brief pwm threshold settings
 */
struct PwmThreshold {
	float temperatureLimit_down;	///lower temperature limit
	float temperatureLimit_up;		///higher temperature limit
	OutputPWMLevel targetLevel;		///target pwm output level
};

/**
 * @brief pwm settings
 */
struct PwmSettings {
	int updateTime;		///update time limit
	float hysteresis;	///hysteresis for switching between threshold levels in degrees celsius

	std::vector <PwmThreshold> pwm1_pwmThresholds;	///thresholds for pwm1
	std::vector <PwmThreshold> pwm2_pwmThresholds;	///thresholds for pwm2
};

/**
 * @brief pwm output settings and state
 */
struct PwmOutput {
	OutputPWMLevel maximumOutput;	///maximum allowed pwm output level
	OutputPWMLevel targetOutput;	///actual targeted pwm output level
	int currentThresholdIndex { -1 }; ///current pwm threshold
	bool automaticEnabled;			///enable pwm selector cap
};

/**
 * @brief pwm duty cycle calculation parameters
 */
struct PwmDutyCycleParameters {
	float a;
	float b;
	float c;
	float d;
};

/**
 * @brief Class handling pwm control
 */
class PwmController {
public:
	/**
	 * @brief Initialize pwm
	 * @param settings settings for pwm
	 */
	void init(const PwmSettings &settings);

	/**
	 * @brief Update the pwm state
	 */
	void update();

private:
	PwmSettings settings_;	///pwm settings
	bringauto::watchdog::Timer timer_ {}; ///timer

	unsigned int updateTimer_;		///counter used for determining time for next pwm update
	unsigned int selectorState_;	///actual state of the selector on the board

	float voltage_;		///actual voltage value
	float current_;		///actual current value
	float temperature_;	///actual temperature

	PwmOutput pwm1_;	///pwm1 state
	PwmOutput pwm2_;	///pwm2 state

	/**
	 * @brief Read pwm state - set actual state for selector and pwm enable pins
	 */
	void readPwmState();

	/**
	 * @brief Update pwm target outputs based on selector state
	 */
	void setTargetOutputs();

	/**
	 * @brief Read voltage using PIN_VOLTAGE_MEASUREMENT pin
	 */
	void readVoltage();

	/**
	 * @brief Read current using PIN_CURRENT_MEASUREMENT pin
	 */
	void readCurrent();

	/**
	 * @brief Read internal temperature of the raspberry pico
	 */
	void readTemperature();

	/**
	 * @brief Update threshold of given pwm based on temperature and limits
	 * @param pwmOutput pwm to be updated
	 * @param pwmThresholds thresholds for given pwm
	 */
	void updateAutomaticPwmState(PwmOutput &pwmOutput, const std::vector <PwmThreshold> &pwmThresholds);

	/**
	 * @brief Set pwm
	 */
	void setPwmOutputs();

	/**
	 * @brief Calculate pwm level based on voltage and given target level
	 * @param targetLevel target level
	 * @return calculated duty cycle
	 */
	unsigned int calculateDutyCycle(OutputPWMLevel targetLevel);
};
}
