#pragma once

#include "Timer.hpp"
#include "Blink.hpp"



namespace bringauto::watchdog {
/**
 * @brief Watchdog state enum
 */
enum class WatchdogState {
	INVALID = -1,
	OS_BOOTING,	///slave OS is in booting state
	OS_RUNNING,	///slave OS is running
	POWER_OFF,	///slave is powered off
};

/**
 * @brief Watchdog settings structure
 */
struct WatchdogSettings {
	unsigned int timeout_OsBooting { 0 };	///timeout in seconds for slave OS booting state
	unsigned int timeout_OsRunning { 0 };	///timeout in seconds for slave OS running state - ping timeout
	unsigned int timeout_PowerOff { 0 };	///for how long the slave OS will be turned of during reset in seconds

	///settings for led blink times in different slave os states
	BlinkTime blinkTime_OsBooting { 0, 0 };
	BlinkTime blinkTime_OsRunning { 0, 0 };
	BlinkTime blinkTime_PowerOff { 0, 0 };
};

/**
 * @brief Watchdog handler class
 */
class Watchdog {
public:
	/**
	 * @brief Initialize watchdog
	 * @param settings watchdog specific settings
	 */
	void init(const WatchdogSettings &settings);

	/**
	 * @brief Update watchdog state
	 */
	void update();

private:
	WatchdogSettings settings_;	///watchdog specific settings
	WatchdogState state_ { WatchdogState::OS_BOOTING };	///actual state of the slave OS
	bringauto::watchdog::Blink blink_ { settings_.blinkTime_OsBooting };	///led blink controller
	bringauto::watchdog::Timer timer_ {};	///timer

	unsigned int stateTime_ { 0 };	///time spend in actual state

	/**
	 * @brief Update watchdog based on actual state
	 */
	void updateState();

	/**
	 * @brief reply to slave ping
	 */
	void reply();

	/**
	 * @brief Switch state
	 * @param state state to switch to
	 */
	void changeState(const WatchdogState &state);

	/**
	 * @brief reset slave OS - power cut
	 */
	void reset();
};
}