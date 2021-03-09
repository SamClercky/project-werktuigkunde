/*********************************
* Motor 1   -> bank A M1
* Motor 2   -> bank A M2
* Echo sens -> bank B S1
*********************************/

#include <Wire.h>
#include <EVShield.h>
#include <EVShieldAGS.h>

#include <EVs_EV3Ultrasonic.h>

// EV shield
EVShield evshield(0x34, 0x36);

// constants
constexpr int baseDuty = 25;
constexpr int accelDuty = 50;
constexpr float MAX_ROPE_DIST = 200; // 2m

// sensors
EVs_EV3Ultrasonic ultrasonic;

void sendCommandToMotor(int motor, int speed) {
	evshield.bank_a.motorRunUnlimited(motor,
			speed < 0 ? SH_Direction_Reverse : SH_Direction_Forward,
			abs(speed));
}

void setup() {
	// setup serial comm (aka logging)
	Serial.begin(9600);
	// Wait for serial to start working
	delay(500);
	
	Serial.println("Init hardware");

	// initialize hardware
	evshield.init(SH_HardwareI2C);

	ultrasonic.init(&evshield, SH_BAS1);
	ultrasonic.setMode(MODE_Sonar_CM);
	evshield.bank_a.motorReset();

	// END initialize hardware
	
	Serial.println("Init hardware succeeded");
	
	// Find rope

	// turn away
	sendCommandToMotor(SH_Motor_1, baseDuty+accelDuty);
	sendCommandToMotor(SH_Motor_2, baseDuty-accelDuty);
	delay(1000); // wait to turn

	while (ultrasonic.getDist() > MAX_ROPE_DIST) {
		// rope not found yet

		// turn back, slowly
		sendCommandToMotor(SH_Motor_1, baseDuty-accelDuty);
		sendCommandToMotor(SH_Motor_2, baseDuty+accelDuty);
		delay(10); // wait to turn (much shorter then previous)
	}

	// Correctly aligned
	Serial.println("Rope found!!!");
}

void loop() {
}
