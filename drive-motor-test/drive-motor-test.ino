#include <Wire.h>
#include <EVShield.h>
#include <EVShieldAGS.h>

#include <EVs_NXTTouch.h>

// EV shield
EVShield evshield(0x34, 0x36);

// sensors
EVs_NXTTouch touch_sensor1;
EVs_NXTTouch touch_sensor2;

bool isWorking1 = false;
bool isWorking2 = false;

long lastPressed1 = 0;
long lastPressed2 = 0;

constexpr long pressDelay = 1000;

char speed = 0;
bool needUpdate = false;

void setup() {
	// setup serial comm (aka logging)
	Serial.begin(9600);
	// Wait for serial to start working
	delay(500);
	
	Serial.println("Init hardware");

	// initialize hardware
	evshield.init(SH_HardwareI2C);

	touch_sensor1.init(&evshield, SH_BBS1);
	touch_sensor2.init(&evshield, SH_BBS2);
	evshield.bank_a.motorReset();

	// END initialize hardware
	
	Serial.println("Init hardware succeeded");
}

void loop() {
	if (evshield.getButtonState(BTN_LEFT)) {
		speed = max(-100, speed -10);
		needUpdate = true;
	}
	if (evshield.getButtonState(BTN_RIGHT)) {
		speed = min(100, speed +10);
		needUpdate = true;
	}

	if (touch_sensor1.isPressed() && lastPressed1 < millis()) {
		Serial.println("Touch pressed");
		isWorking1 = !isWorking1;

		if (!isWorking1) {
			sendCommandToMotor(SH_Motor_1, 0); // STOP
		} else {
			needUpdate = true;
		}

		lastPressed1 = millis() + pressDelay;
	}
	if (touch_sensor2.isPressed() && lastPressed2 < millis()) {
		Serial.println("Touch pressed");
		isWorking2 = !isWorking2;

		if (!isWorking2) {
			sendCommandToMotor(SH_Motor_2, 0); // STOP
		} else {
			needUpdate = true;
		}

		lastPressed2 = millis() + pressDelay;
	}

	if (needUpdate) {
		Serial.println("Updating motor");
		Serial.print("Speed: ");
		Serial.println(speed);

		if (isWorking1)
			 sendCommandToMotor(SH_Motor_1, speed);
		if (isWorking2)
			 sendCommandToMotor(SH_Motor_2, speed);

		needUpdate = false;
	}
}

void sendCommandToMotor(int motor, int speed) {
	evshield.bank_a.motorRunUnlimited(motor,
			speed < 0 ? SH_Direction_Reverse : SH_Direction_Forward,
			abs(speed));
}
