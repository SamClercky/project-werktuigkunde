all: build upload

build:
	arduino-cli compile --fqbn arduino:avr:uno

upload:
	arduino-cli upload -t -b arduino:avr:uno -p /dev/ttyUSB0
