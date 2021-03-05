#include <Wire.h>
#include <EVShield.h>
#include <EVShieldAGS.h>

#include <EVs_EV3Ultrasonic.h>

// EV shield
EVShield evshield(0x34, 0x36);

// sensors
EVs_EV3Ultrasonic ultrasonic;

bool isWorking = false;

constexpr long pressDelay = 1000;
constexpr float triggerDistance = 5; // 5cm

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
}

void loop() {
	// TODO
}

void sendCommandToMotor(int motor, int speed) {
	evshield.bank_a.motorRunUnlimited(motor,
			speed < 0 ? SH_Direction_Reverse : SH_Direction_Forward,
			abs(speed));
}
