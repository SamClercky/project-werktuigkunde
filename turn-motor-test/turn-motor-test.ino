/******************************
* Motor 1 -> bank B M1
* Motor 2 -> bank B M2
* Color 1 -> bank A S1
* Color 2 -> bank A S2
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

// GLOBALE SETTINGS
constexpr int baseDuty = 25;
constexpr int accelDuty = 25;
int BACKGROUND = 0;
int LIJN = 5;
int FINAL = 1;

void sendCommandToMotor(int motor, int speed) {
	evshield.bank_b.motorRunUnlimited(motor,
			speed < 0 ? SH_Direction_Forward : SH_Direction_Reverse,
			abs(speed));
}

char* getColor(float sensorData) {
	switch (round(sensorData)) {
		case 1:
			return "zwart";
		case 6:
			return "wit";
		case 7:
			return "wit";
		default:
			return "onbekend";
	}
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
		sendCommandToMotor(SH_Motor_1, baseDuty);
		sendCommandToMotor(SH_Motor_2, -baseDuty);
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

	color_sensor1.init(&evshield, SH_BAS1);
	color_sensor1.setMode(MODE_Color_MeasureColor);

	color_sensor2.init(&evshield, SH_BAS2);
	color_sensor2.setMode(MODE_Color_MeasureColor);

	evshield.bank_b.motorReset();

	// END initialize hardware
	
	Serial.println("Init hardware succeeded");

	// Wait for start command
	Serial.println("Start GO commando om te beginnen");
	evshield.waitForButtonPress(BTN_GO);

	// Calibration
	calibrationRoutine();

	// start in vooruit
	sendCommandToMotor(SH_Motor_1, baseDuty);
	sendCommandToMotor(SH_Motor_2, baseDuty);
}

void loop() {
	int color1 = color_sensor1.getVal();
	int color2 = color_sensor2.getVal();

	if (color1 != BACKGROUND) {
		// Start logica
		sendCommandToMotor(SH_Motor_1, -accelDuty);
		sendCommandToMotor(SH_Motor_2, +accelDuty);
		Serial.println("LIJN 1");
		delay(50);
	}

	if (color2 != BACKGROUND) {
		// Start logica
		sendCommandToMotor(SH_Motor_1, +accelDuty);
		sendCommandToMotor(SH_Motor_2, -accelDuty);
		Serial.println("LIJN 2");
		delay(50);
	}

	if (color1 == BACKGROUND && color2 == BACKGROUND) {
		// Start logica
		sendCommandToMotor(SH_Motor_1, baseDuty);
		sendCommandToMotor(SH_Motor_2, baseDuty);
		Serial.println("GEEN LIJN");
	}

	Serial.print("Kleur gedetecteerd: ");
	Serial.print(getColor(color_sensor1.getVal()));
	Serial.print(", ");
	//Serial.print(getColor(color_sensor2.getVal()));
	Serial.println();

	delay(10);
}

