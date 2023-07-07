#pragma once

namespace bringauto::watchdog {
/**
 * @brief Structure containing settings for blink functionality
 */
struct BlinkTime {
	unsigned int onTime { 0 };		///on time limit
	unsigned int offTime { 0 };		///off time limit

	/**
	 * @brief Constructor
	 * @param on set the on time limit
	 * @param off set the off time limit
	 */
	BlinkTime(unsigned int on, unsigned int off): onTime(on), offTime(off) {};
};

/**
 * @brief Handler for dealing
 */
class Blink {
public:
	/**
	 * @brief Constructor
	 * @param blinkTime led time limit
	 */
	Blink(const BlinkTime &blinkTime): currentBlinkTime_(blinkTime) {};

	/**
	 * @brief Update the internal timer
	 * @param cycleTime time since last update call
	 */
	void update(unsigned int cycleTime);

	/**
	 * @brief Change the led time limits
	 * @param blinkTime
	 */
	void setBlinkTime(const BlinkTime &blinkTime);

private:
	BlinkTime currentBlinkTime_; 	///current time limit for led
	unsigned int stateTime_ { 0 };	///time spend in current state
	bool isOn_ { false };			///current state of the led

	/**
	 * @brief Check if led is disabled
	 * @return true if led blink functionality is disabled
	 * @return false if led blink functionality is enabled
	 */
	bool isDisabled();
};
}
