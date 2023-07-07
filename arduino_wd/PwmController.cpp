#include "PwmController.hpp"
#include "Config.h"

#include <Arduino.h>



const unsigned int AD_MAX = 255;
const float ADC_MAX = (float)((1 << ADC_RES) - 1);
const int AVG_READ_CNT = 5;

namespace bringauto::watchdog {
void PwmController::init(const PwmSettings &settings) {
	settings_ = settings;

	readTemperature();

	for(int i = 0; i < settings_.pwm1_pwmThresholds.size(); i++) {
		if(temperature_ > settings.pwm1_pwmThresholds[i].temperatureLimit_down &&
		   temperature_ < settings.pwm1_pwmThresholds[i].temperatureLimit_up) {
			pwm1_.currentThresholdIndex = i;
			break;
		}
	}

	for(int i = 0; i < settings_.pwm2_pwmThresholds.size(); i++) {
		if(temperature_ > settings.pwm2_pwmThresholds[i].temperatureLimit_down &&
		   temperature_ < settings.pwm2_pwmThresholds[i].temperatureLimit_up) {
			pwm2_.currentThresholdIndex = i;
			break;
		}
	}

	///Setup GPIO
	pinMode(PIN_RS1, INPUT_PULLUP);
	pinMode(PIN_RS2, INPUT_PULLUP);
	pinMode(PIN_RS4, INPUT_PULLUP);
	pinMode(PIN_RS8, INPUT_PULLUP);

	pinMode(PIN_PWM1_ENABLE, INPUT_PULLUP);
	pinMode(PIN_PWM2_ENABLE, INPUT_PULLUP);

	analogReadResolution(ADC_RES);
}

void PwmController::update() {
	timer_.update();
	updateTimer_ += timer_.getCycleTime();

	if(updateTimer_ >= settings_.updateTime) {
		readPwmState();
		readVoltage();
		readCurrent();
		readTemperature();

		updateAutomaticPwmState(pwm1_, settings_.pwm1_pwmThresholds);
		updateAutomaticPwmState(pwm2_, settings_.pwm2_pwmThresholds);
		setPwmOutputs();

		updateTimer_ = 0;
	}
}

void PwmController::readPwmState() {
	bitWrite(selectorState_, 0, !digitalRead(PIN_RS1));
	bitWrite(selectorState_, 1, !digitalRead(PIN_RS2));
	bitWrite(selectorState_, 2, !digitalRead(PIN_RS4));
	bitWrite(selectorState_, 3, !digitalRead(PIN_RS8));

	pwm1_.automaticEnabled = !digitalRead(PIN_PWM1_ENABLE);
	pwm2_.automaticEnabled = !digitalRead(PIN_PWM2_ENABLE);

	setTargetOutputs();

	if(Serial) {
		Serial.flush();
		Serial.printf("Selector state: %X\r\n", selectorState_);
		Serial.printf("TEMP_CTL: PWM1 %s\tPWM2 %s\r\n", pwm1_.automaticEnabled ? "true" : "false",
					  pwm2_.automaticEnabled ? "true" : "false");
		Serial.printf("TARGET PWM: PWM1 %s\tPWM2 %s\r\n", outputPwmLvlToString.at(pwm1_.targetOutput),
					  outputPwmLvlToString.at(pwm2_.targetOutput));
	}
}

void PwmController::setTargetOutputs() {
	switch(selectorState_) {
		case 0x0:
			pwm1_.maximumOutput = OutputPWMLevel::OFF;
			pwm2_.maximumOutput = OutputPWMLevel::OFF;
			break;
		case 0x1:
			pwm1_.maximumOutput = OutputPWMLevel::LOW;
			pwm2_.maximumOutput = OutputPWMLevel::OFF;
			break;
		case 0x2:
			pwm1_.maximumOutput = OutputPWMLevel::HIGH;
			pwm2_.maximumOutput = OutputPWMLevel::OFF;
			break;
		case 0x3:
			pwm1_.maximumOutput = OutputPWMLevel::OFF;
			pwm2_.maximumOutput = OutputPWMLevel::LOW;
			break;
		case 0x4:
			pwm1_.maximumOutput = OutputPWMLevel::LOW;
			pwm2_.maximumOutput = OutputPWMLevel::LOW;
			break;
		case 0x5:
			pwm1_.maximumOutput = OutputPWMLevel::HIGH;
			pwm2_.maximumOutput = OutputPWMLevel::LOW;
			break;
		case 0x6:
			pwm1_.maximumOutput = OutputPWMLevel::OFF;
			pwm2_.maximumOutput = OutputPWMLevel::HIGH;
			break;
		case 0x7:
			pwm1_.maximumOutput = OutputPWMLevel::LOW;
			pwm2_.maximumOutput = OutputPWMLevel::HIGH;
			break;
		case 0x8:
			pwm1_.maximumOutput = OutputPWMLevel::HIGH;
			pwm2_.maximumOutput = OutputPWMLevel::HIGH;
			break;
		case 0x9:
		case 0xA:
		case 0xB:
		case 0xC:
		case 0xD:
		case 0xE:
		case 0xF:
		default:
			pwm1_.maximumOutput = OutputPWMLevel::OFF;
			pwm2_.maximumOutput = OutputPWMLevel::OFF;
			break;
	}

}

void PwmController::readVoltage() {
	int rawVoltage = analogRead(PIN_VOLTAGE_MEASUREMENT);
	for(int c = 0; c < AVG_READ_CNT; c++) {
		rawVoltage += analogRead(PIN_VOLTAGE_MEASUREMENT);
		rawVoltage /= 2;
		delay(2);
	}
	voltage_ = 10.0f*ADC_V_REF*(1.1f - float(rawVoltage)/ADC_MAX);  ///- Vin = 10 * Vref * (1.1 - X / (2^ADC_res - 1))

	if(Serial) {
		Serial.flush();
		Serial.printf("Voltage: %.2fV\r\n", voltage_);

	}
}

void PwmController::readCurrent() {
	int rawCurrent = analogRead(PIN_CURRENT_MEASUREMENT);
	for(int c = 0; c < AVG_READ_CNT; c++) {
		rawCurrent += analogRead(PIN_CURRENT_MEASUREMENT);
		rawCurrent /= 2;
		delay(2);
	}
	current_ = ADC_V_REF/0.604f*float(rawCurrent)/
			   ADC_MAX;        /// I = (Vref * R1) / (R2 * Rb) * X / (2^ADC_res - 1); where: R1 = 10k, R2 = 604k, Rb = 0.01

	if(Serial) {
		Serial.flush();
		Serial.printf("Current: %.2fA\r\n", current_);
	}
}

void PwmController::readTemperature() {
	temperature_ = analogReadTemp(ADC_V_REF);
	if(Serial) {
		Serial.flush();
		Serial.printf("Temperature: %.2fC\r\n", temperature_);
	}

}

void PwmController::updateAutomaticPwmState(PwmOutput &pwmOutput, const std::vector <PwmThreshold> &pwmThresholds) {
	if(temperature_ < pwmThresholds[pwmOutput.currentThresholdIndex].temperatureLimit_down - settings_.hysteresis) {
		pwmOutput.currentThresholdIndex = (pwmOutput.currentThresholdIndex <= 0 ? 0 : pwmOutput.currentThresholdIndex -
																					  1);
	}

	if(temperature_ > pwmThresholds[pwmOutput.currentThresholdIndex].temperatureLimit_up) {
		pwmOutput.currentThresholdIndex = (pwmOutput.currentThresholdIndex > pwmThresholds.size()
										   ? pwmThresholds.size() - 1 : pwmOutput.currentThresholdIndex + 1);
	}

	pwmOutput.targetOutput = pwmThresholds[pwmOutput.currentThresholdIndex].targetLevel;
	if(pwmOutput.automaticEnabled) {
		if(pwmOutput.targetOutput > pwmOutput.maximumOutput) {
			pwmOutput.targetOutput = pwmOutput.maximumOutput;
		}
	}
}

void PwmController::setPwmOutputs() {
	unsigned int tmp_adOut1, tmp_adOut2;

	tmp_adOut1 = calculateDutyCycle(pwm1_.targetOutput);
	tmp_adOut2 = calculateDutyCycle(pwm2_.targetOutput);

	analogWrite(PIN_PWM1, tmp_adOut1);
	analogWrite(PIN_PWM2, tmp_adOut2);
}

unsigned int PwmController::calculateDutyCycle(OutputPWMLevel targetLevel) {
	PwmDutyCycleParameters params;
	unsigned int tmp_adOut;
	if(targetLevel == OutputPWMLevel::HIGH) {
		params = PWM_12V_OUT_PARAMS;
	} else if(targetLevel == OutputPWMLevel::LOW) {
		params = PWM_5V_OUT_PARAMS;
	} else {
		return 0;
	}
	///duty cycle = (a*voltage+b)/(c*voltage+d);
	tmp_adOut = AD_MAX*(params.a*voltage_ + params.b)/(params.c*voltage_ + params.d);
	tmp_adOut = tmp_adOut > AD_MAX ? AD_MAX : tmp_adOut;
	return tmp_adOut;
}
}
