/*********************************
* Motor 1   -> bank B M1
* Motor 2   -> bank B M2
* Echo sens -> bank B S1
*********************************/

#include <Wire.h>
#include <EVShield.h>
#include <EVShieldAGS.h>

#include <EVs_EV3Ultrasonic.h>

// EV shield
EVShield evshield(0x34, 0x36);

// constants
constexpr int baseDuty = -25;
constexpr int accelDuty = -50;
constexpr float MAX_ROPE_DIST = 50; // 50cm
constexpr float GOOD_ROPE_DIST = 20; // 20cm

constexpr int sendDelay = 10;

bool hasFound = false;
int sendTime = sendDelay;
float currDist = MAX_ROPE_DIST*2; // way to high

// sensors
EVs_EV3Ultrasonic ultrasonic;

void sendCommandToMotor(int motor, int speed) {
	evshield.bank_b.motorRunUnlimited(motor,
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

	ultrasonic.init(&evshield, SH_BBS1);
	ultrasonic.setMode(MODE_Sonar_CM);
	evshield.bank_b.motorReset();

	// END initialize hardware
	
	Serial.println("Init hardware succeeded");

	Serial.println("Wait for GO button");
	evshield.waitForButtonPress(BTN_GO);
	
	// Find rope

	Serial.println("Start scan");

	currDist = ultrasonic.getDist();
	Serial.print("Eerste afstandslezing: ");
	Serial.println(currDist);
//	while (currDist > GOOD_ROPE_DIST) {
//		while (currDist > MAX_ROPE_DIST) {
//			// rope not found yet
//			Serial.print("Current Dist: ");
//			Serial.println(currDist);
//
//			// turn back, slowly
//			sendCommandToMotor(SH_Motor_1, ( baseDuty-accelDuty )/2);
//			sendCommandToMotor(SH_Motor_2, ( baseDuty+accelDuty)/2 );
//			delay(10); // wait to turn (much shorter then previous)
//
//			// update dist
//			currDist = ultrasonic.getDist();
//		}
//
//		Serial.print("Current Dist: ");
//		Serial.println(currDist);
//
//		// go straight
//		Serial.println("Going straight");
//		sendCommandToMotor(SH_Motor_Both, baseDuty);
//		delay(10);
//
//		// update dist
//		currDist = ultrasonic.getDist();
//	}
}

void loop() {
	if (hasFound) return;

	// update dist
	currDist = ultrasonic.getDist();

	if (sendTime == 0) {
		sendTime = sendDelay;

		Serial.print("Current Dist: ");
		Serial.println(currDist);
	}
	sendTime--;

	// Find correct direction
	if (currDist > MAX_ROPE_DIST) {

		// turn away
		sendCommandToMotor(SH_Motor_1, baseDuty+accelDuty);
		sendCommandToMotor(SH_Motor_2, baseDuty-accelDuty);
		delay(500); // wait to turn

		// fix direction
		while (currDist > MAX_ROPE_DIST) {
			// rope not found yet
			// turn back, slowly
			sendCommandToMotor(SH_Motor_1, baseDuty-accelDuty);
			sendCommandToMotor(SH_Motor_2, baseDuty+accelDuty);
			delay(10); // wait to turn (much shorter then previous)

			// update
			currDist = ultrasonic.getDist();
		}
	}

	// Rij naar touw
	if (currDist > GOOD_ROPE_DIST && currDist < MAX_ROPE_DIST) {
		Serial.print("Current Dist: ");
		Serial.println(currDist);

		// go straight
		Serial.println("Going straight");
		sendCommandToMotor(SH_Motor_Both, baseDuty);
		delay(10);
	}

	if (currDist < GOOD_ROPE_DIST && currDist != 0) {
		hasFound = true;
		// Correctly aligned
		Serial.println("Rope found!!!");
		
		// reset motors
		evshield.bank_b.motorReset();
	}
}
