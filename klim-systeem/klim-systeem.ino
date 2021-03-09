/*********************************
* Mtr klep 1 -> bank A M1
* Mtr klim 2 -> bank A M2
* Echo sens  -> bank B S1
*********************************/

#include <Wire.h>
#include <EVShield.h>
#include <EVShieldAGS.h>

#include <EVs_EV3Ultrasonic.h>

// EV shield
EVShield evshield(0x34, 0x36);

// sensors
EVs_EV3Ultrasonic ultrasonic;

constexpr long pressDelay = 1000;
constexpr float triggerDistance = 5; // 5cm

#define klimDelay delay(10)
constexpr int klimSpeedInc = 1;
int klimSpeed = 0;

enum {
	STATE_SEARCH = 0,
	STATE_CLIMB,
} state = STATE_SEARCH;

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
	evshield.bank_b.motorReset();

	// END initialize hardware
	
	Serial.println("Init hardware succeeded");
}

void searchRoutine() {
	if (ultrasonic.getDist() < triggerDistance) {
		state = STATE_CLIMB;
	}
	Serial.println("Te ver om te klimmen");
}

void klimRoutine() {
	if (klimSpeed < 100) { // alleen maar updaten onder 100 snelheid
		Serial.println("klimspeed verhogen");

		// update klimSpeed
		klimSpeed = min(klimSpeed+klimSpeedInc, 100);

		// update motor
		sendCommandToMotor(SH_Motor_Both, klimSpeed);

		// vertraag update
		klimDelay;
	}
}

void loop() {
	switch (state) {
		case STATE_SEARCH:
			searchRoutine();
			break;
		case STATE_CLIMB:
			klimRoutine();
			break;
		default: // zou normaal nooit moeten worden aangeroepen
			Serial.println("ERROR: Invalid state");
			break;
	}
}
