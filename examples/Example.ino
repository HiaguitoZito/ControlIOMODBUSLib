#include <Arduino.h>
#include "ControlIOModbus.h"

int** registers = new int* [1];
ControlIOMODBUSSlave slave = ControlIOMODBUSSlave(1);
void setup() {
	Serial.begin(9600);
	slave.SetHoldingRegisters(registers);
}

void loop() {
	*registers[0] = 100;
}
