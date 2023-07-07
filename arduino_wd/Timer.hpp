#pragma once

namespace bringauto::watchdog {
/**
 * @brief Class for time management, miliseconds used
 */
class Timer {
public:
	/**
	 * @brief Update the time
	 */
	void update();

	/**
	 * @brief Get cycle time - time between last update and actual update
	 * @return
	 */
	unsigned int getCycleTime();

private:
	unsigned int timestampLast_ { 0 };		///timestamp of last update call in milliseconds
	unsigned int timestampCurrent_ { 0 };	///timestamp od current update call in milliseconds

	unsigned int cycleTime_ { 0 };	///time between last two update calls
};
}