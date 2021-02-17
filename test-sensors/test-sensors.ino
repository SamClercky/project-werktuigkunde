#include <Wire.h>
#include <EVShield.h>
#include <EVShieldAGS.h>

// Toevoegen van sensors
#include <EVs_EV3Ultrasonic.h>
#include <EVs_EV3Color.h>

// init shield
EVShield evshield(0x34, 0x36);

// init sensors
EVs_EV3Ultrasonic ultrasonic_sensor;
EVs_EV3Color color_sensor;

int currentSensor = 0;
int maxSensor = 2;

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

	// END initialize hardware
	
	Serial.println("Init hardware succeeded");

	// Wait for start command
	Serial.println("Start GO commando om te beginnen");
	evshield.waitForButtonPress(BTN_GO);
}

void loop() {
	switch ( currentSensor ) {
		case 0:
			Serial.print("Ultrasoon: ");
			Serial.print(ultrasonic_sensor.getDist());
			Serial.println("cm");
			break;
		case 1:
			Serial.print("Colorsensor: ");
			Serial.print(getColor(color_sensor.getVal()));
			Serial.println();
			break;
	}

	// Control sensor read
	if (evshield.getButtonState(BTN_LEFT)) {
		currentSensor = ( currentSensor + 1 ) % maxSensor;
	}
	if (evshield.getButtonState(BTN_RIGHT)) {
		currentSensor = (currentSensor + maxSensor - 1) % maxSensor;
	}

	delay(500);
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
