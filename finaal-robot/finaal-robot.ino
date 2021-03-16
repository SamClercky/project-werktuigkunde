/******************************
* Motor 1     -> bank A M1
* Motor 2     -> bank A M2
* Mtr Climb 1 -> bank B M1
* Mtr Klem  2 -> bank B M2
* Color 1     -> bank A S1
* Color 2     -> bank A S2
* Echo sens   -> bank B S1
* 
* GO btn to start
*******************************/

#include <Wire.h>
#include <EVShield.h>
#include <EVShieldAGS.h>

// Toevoegen van sensors
#include <EVs_EV3Ultrasonic.h>
#include <EVs_EV3Color.h>
#include <EVs_NXTTouch.h>

// EV shield
EVShield evshield(0x34, 0x36);

// sensors
EVs_EV3Color color_sensor1;
EVs_EV3Color color_sensor2;
EVs_EV3Ultrasonic ultrasonic;

enum {
	STATE_DRIVE_INIT,
	STATE_DRIVE,
	STATE_SEARCH,
	STATE_SEARCH_TO_CLIMB,
	STATE_CLIMB,
	STATE_GO_DOWN,
} state = STATE_DRIVE_INIT;

// GLOBALE SETTINGS RIJDEN
constexpr int baseDuty = 25;
constexpr int accelDuty = 25;
int BACKGROUND = 0;
int LIJN = 5;
int FINAL = 1;

// GLOBALE SETTINGS TOUW VINDEN
constexpr float MAX_ROPE_DIST = 50; // 50cm
constexpr float GOOD_ROPE_DIST = 20; // 20cm
constexpr int sendDelay = 10;

int sendTime = sendDelay;
float currDist = MAX_ROPE_DIST*2; // way to high

// GLOBALE SETTINGS KLIMMEN


void sendCommandToMotorDrive(int motor, int speed) {
	evshield.bank_a.motorRunUnlimited(motor,
			speed < 0 ? SH_Direction_Forward : SH_Direction_Reverse,
			abs(speed));
}

void sendCommandToMotorClimb(int motor, int speed) {
	evshield.bank_b.motorRunUnlimited(motor,
			speed < 0 ? SH_Direction_Forward : SH_Direction_Reverse,
			abs(speed));
}

void calibrationRoutine() {
	int c1;
	int c2;

	Serial.println("===== Start calibration ======");

	do {
		// detect color
		c1 = floor(color_sensor1.getVal());
		c2 = floor(color_sensor2.getVal());

		BACKGROUND = c1;

		// move away from a possible line
		sendCommandToMotorDrive(SH_Motor_1, baseDuty);
		sendCommandToMotorDrive(SH_Motor_2, -baseDuty);
		delay(200); // wait for acion to complete
	} while (c1 != c2);

	// background should be identified
	Serial.print("\nBackground found: ");
	Serial.println(BACKGROUND);
	Serial.println("===== End calibration ======");
}

void setup() {
	Serial.begin(9600);
	delay(500);

	Serial.println("Init hardware");

	// initialize hardware
	evshield.init(SH_HardwareI2C);

	color_sensor1.init(&evshield, SH_BBS1);
	color_sensor1.setMode(MODE_Color_MeasureColor);

	color_sensor2.init(&evshield, SH_BBS2);
	color_sensor2.setMode(MODE_Color_MeasureColor);

	ultrasonic.init(&evshield, SH_BAS1);
	ultrasonic.setMode(MODE_Sonar_CM);

	evshield.bank_a.motorReset();
	evshield.bank_b.motorReset();

	// END initialize hardware
	
	Serial.println("Init hardware succeeded");

	// Wait for start command
	Serial.println("Start GO commando om te beginnen");
	evshield.waitForButtonPress(BTN_GO);

}

void driveRoutine() {
	int color1 = color_sensor1.getVal();
	int color2 = color_sensor2.getVal();

	if (color1 != BACKGROUND) {
		// Start logica
		sendCommandToMotorDrive(SH_Motor_1, -accelDuty);
		sendCommandToMotorDrive(SH_Motor_2, +accelDuty);
		Serial.println("LIJN 1");
		delay(50);
	}

	if (color2 != BACKGROUND) {
		// Start logica
		sendCommandToMotorDrive(SH_Motor_1, +accelDuty);
		sendCommandToMotorDrive(SH_Motor_2, -accelDuty);
		Serial.println("LIJN 2");
		delay(50);
	}

	if (color1 == BACKGROUND && color2 == BACKGROUND) {
		// Start logica
		sendCommandToMotorDrive(SH_Motor_1, baseDuty);
		sendCommandToMotorDrive(SH_Motor_2, baseDuty);
		Serial.println("GEEN LIJN");
	}

	if (color1 != BACKGROUND && color2 != BACKGROUND) {
		// Ready for next stage
		Serial.println("Starting SEACH STATE");
		state = STATE_SEARCH;
	}

	delay(10);
}

void searchRoutine() {
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
		sendCommandToMotorDrive(SH_Motor_1, baseDuty+accelDuty);
		sendCommandToMotorDrive(SH_Motor_2, baseDuty-accelDuty);
		delay(500); // wait to turn

		// fix direction
		while (currDist > MAX_ROPE_DIST) {
			// rope not found yet
			// turn back, slowly
			sendCommandToMotorDrive(SH_Motor_1, baseDuty-accelDuty);
			sendCommandToMotorDrive(SH_Motor_2, baseDuty+accelDuty);
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
		sendCommandToMotorDrive(SH_Motor_Both, baseDuty);
		delay(10);
	}

	if (currDist < GOOD_ROPE_DIST && currDist != 0) {
		// Correctly aligned
		Serial.println("Rope found!!!");
		
		// Go to next stage
		state = STATE_SEARCH_TO_CLIMB;
	}
}

void loop() {
	switch (state) {
		case STATE_DRIVE_INIT:
			{
				// Initiate Driving, calibrate
				// Calibration
				calibrationRoutine();

				// start in vooruit
				sendCommandToMotorDrive(SH_Motor_1, baseDuty);
				sendCommandToMotorDrive(SH_Motor_2, baseDuty);

				// Go to next state
				state = STATE_DRIVE;
				break;
			}
		case STATE_DRIVE:
			{
				driveRoutine();
				break;
			}
		case STATE_SEARCH:
			{
				searchRoutine();
				break;
			}
		case STATE_SEARCH_TO_CLIMB:
			{
				// TODO
				evshield.bank_b.motorReset();
				break;
			}
		case STATE_CLIMB:
			{
				// TODO
				break;
			}
		case STATE_GO_DOWN:
			{
				// TODO
				break;
			}
		default:
			{
				Serial.println("ERROR: Illegal state");
			}
	}
}
