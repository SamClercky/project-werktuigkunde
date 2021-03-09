/******************************
* Motor 1 -> bank B M1
* Motor 2 -> bank B M2
* Color 1 -> bank A S1
* Color 2 -> bank A S2
* Btn   1 -> bank B S1
* Btn   2 -> bank B S2
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
EVs_NXTTouch touch_sensor1;
EVs_NXTTouch touch_sensor2;

EVs_EV3Color color_sensor1;
EVs_EV3Color color_sensor2;

// GLOBALE SETTINGS
constexpr long pressDelay = 1000;
constexpr bool colorSensorsActive = true;
constexpr int baseDuty = 25;
constexpr int accelDuty = 50;
constexpr int LIJN = 5;
constexpr int FINAL = 1;

long lastPressed1 = 0;
long lastPressed2 = 0;

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

void setup() {
	Serial.begin(9600);
	delay(500);

	Serial.println("Init hardware");

	// initialize hardware
	evshield.init(SH_HardwareI2C);

	touch_sensor1.init(&evshield, SH_BBS1);
	touch_sensor2.init(&evshield, SH_BBS2);

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

	if (colorSensorsActive) {
		// start in vooruit
		sendCommandToMotor(SH_Motor_1, baseDuty);
		sendCommandToMotor(SH_Motor_2, baseDuty);
	}
}

void loop() {
	if (colorSensorsActive) {
		int color1 = color_sensor1.getVal();
		int color2 = color_sensor2.getVal();

		if (color1 == LIJN) {
			// Start logica
			sendCommandToMotor(SH_Motor_1, baseDuty - accelDuty);
			sendCommandToMotor(SH_Motor_2, baseDuty + accelDuty);
			Serial.println("LIJN 1");
			delay(300);
		}

		if (color2 == LIJN) {
			// Start logica
			sendCommandToMotor(SH_Motor_1, baseDuty + accelDuty);
			sendCommandToMotor(SH_Motor_2, baseDuty - accelDuty);
			Serial.println("LIJN 2");
			delay(300);
		}

		if (color1 != LIJN && color2 != LIJN) {
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
	} else {
		if (touch_sensor1.isPressed() && lastPressed1 < millis()) {
			Serial.println("Naar voor");
			sendCommandToMotor(SH_Motor_1, baseDuty);
			sendCommandToMotor(SH_Motor_2, baseDuty);

			lastPressed1 = millis() + pressDelay;
		}
		if (touch_sensor2.isPressed() && lastPressed2 < millis()) {
			Serial.println("Draaien");
			sendCommandToMotor(SH_Motor_1, baseDuty);
			sendCommandToMotor(SH_Motor_2, -baseDuty);

			lastPressed2 = millis() + pressDelay;
		}
	}
}

