#include <Wire.h>
#include <EVShield.h>
#include <EVShieldAGS.h>

// Toevoegen van sensors
#include <EVs_EV3Ultrasonic.h>
#include <EVs_EV3Color.h>
#include <EVs_NXTTouch.h>

// init shield
EVShield evshield(0x34, 0x36);

// init sensors
EVs_EV3Ultrasonic ultrasonic_sensor;
EVs_EV3Color color_sensor;
EVs_NXTTouch touch_sensor;

int currentSensor = 0;
constexpr int maxSensor = 3;

constexpr long delayTime = 500;
long nexTime = 0;

void setup() {
	// setup serial comm (aka logging)
	Serial.begin(9600);
	// Wait for serial to start working
	delay(500);
	
	Serial.println("Init hardware");

	// initialize hardware
	evshield.init(SH_HardwareI2C);

	ultrasonic_sensor.init(&evshield, SH_BAS2);
	ultrasonic_sensor.setMode(MODE_Sonar_CM); // set unit to cm
	
	color_sensor.init(&evshield, SH_BAS1);
	color_sensor.setMode(MODE_Color_MeasureColor);

	touch_sensor.init(&evshield, SH_BBS1);

	// END initialize hardware
	
	Serial.println("Init hardware succeeded");

	// Wait for start command
	Serial.println("Start GO commando om te beginnen");
	evshield.waitForButtonPress(BTN_GO);
}

void loop() {
	switch ( currentSensor ) {
		case 0:
			if (nexTime < millis()) {
				Serial.print("Ultrasoon: ");
				Serial.print(ultrasonic_sensor.getDist());
				Serial.println("cm");

				nexTime = millis() + delayTime;
			}
			break;
		case 1:
			if (nexTime < millis()) {
				Serial.print("Colorsensor: ");
				Serial.print(getColor(color_sensor.getVal()));
				Serial.println();

				nexTime = millis() + delayTime;
			}
			break;
		case 2:
			if (touch_sensor.isPressed()) {
				Serial.println("Touched registered");
			}
			break;
	}

	// Control sensor read
	if (evshield.getButtonState(BTN_LEFT)) {
		currentSensor = ( currentSensor + 1 ) % maxSensor;
	}
	if (evshield.getButtonState(BTN_RIGHT)) {
		currentSensor = (currentSensor + maxSensor - 1) % maxSensor;
	}
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
