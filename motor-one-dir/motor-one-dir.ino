#include <Wire.h>
#include <EVShield.h>
#include <EVShieldAGS.h>

#include <EVs_NXTTouch.h>

// EV shield
EVShield evshield(0x34, 0x36);

// sensors
EVs_NXTTouch touch_sensor;

bool isWorking = false;

long lastPressed = 0;

constexpr long pressDelay = 500;

void setup() {
	// setup serial comm (aka logging)
	Serial.begin(9600);
	// Wait for serial to start working
	delay(500);
	
	Serial.println("Init hardware");

	// initialize hardware
	evshield.init(SH_HardwareI2C);

	touch_sensor.init(&evshield, SH_BBS1);
	evshield.bank_a.motorReset();

	// END initialize hardware
	
	Serial.println("Init hardware succeeded");
}

void loop() {
	if (touch_sensor.isPressed() && lastPressed < millis()) {
		Serial.println("Touch pressed");
		isWorking = !isWorking;

		if (!isWorking) {
			sendCommandToMotor(SH_Motor_Both, 0); // STOP
		} else {
			sendCommandToMotor(SH_Motor_1, -100); // RUN
			sendCommandToMotor(SH_Motor_2, 100); // RUN
		}

		lastPressed = millis() + pressDelay;
	}
}

void sendCommandToMotor(int motor, int speed) {
	evshield.bank_a.motorRunUnlimited(motor,
			speed < 0 ? SH_Direction_Reverse : SH_Direction_Forward,
			abs(speed));
}
