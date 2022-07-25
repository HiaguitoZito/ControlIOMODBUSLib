#include "ControlIOModbus.h"

bool* ControlIOMODBUSPoll::result = new bool[1];
int* ControlIOMODBUSPoll::resultRegisters = new int[1];

bool ControlIOMODBUSPoll::WriteSingleCoil(int slaveID, int coil, bool value) {
	// Creates the message to send to slave.
	short* message = new short[6];
	message[0] = slaveID; // The slave id.
	message[1] = 0x05; // Write coil function code (0x05).
	message[2] = coil / 256; // Most significant 2 bytes of the coils to write.
	message[3] = coil % 256; // Less significant 2 bytes of the coils to write.
	message[4] = value ? 0xFF : 0x00; // if value = true then message[4] = 0xFF then elsewise message[4] = 0x00.
	message[5] = 0x00;

	// Does the CRC and send the message at the same time.
	unsigned int CRC = 0xFFFF;
	for (int i = 0; i < 6; i++) {
		CRC = DoCRC(message[i], CRC);
		Serial.write(message[i]);
		delay(2);
	}
	// Sends the CRC.
	Serial.write(CRC);
	delay(2);
	Serial.write((unsigned int)CRC / 256);
	
	// Checks if the message reached the slave.
	for (int i = 0; i < 14; i++) {
		if (Serial.available() > 7) break;
		delay(5);
	}
	if (Serial.available() < 8) {
		delete[] message;
		return false;
	}
	for (int i = 0; i < 6; i++) {
		if (Serial.read() != message[i]) { // if message is wrong return false.
			delete[] message;
			return false;
		}
	}
	if (Serial.read() != CRC % 256 || Serial.read() != (unsigned int)CRC / 256) { // if message is wrong return false.
		delete[] message;
		return false;
	}
	delete[] message;
	return true;
}
bool ControlIOMODBUSPoll::WriteSingleRegister(unsigned int slaveID, unsigned int reg, unsigned int value) {
	// Creates the message to send to slave.
	unsigned short* message = new unsigned short[6];
	message[0] = slaveID; // The slave id.
	message[1] = 0x06; // Write coil function code (0x05).
	message[2] = reg / 256; // Most significant 2 bytes of the registers to write.
	message[3] = reg % 256; // Less significant 2 bytes of the registers to write.
	message[4] = value / 256;
	message[5] = value % 256;

	// Does the CRC and send the message at the same time.
	unsigned int CRC = 0xFFFF;
	for (int i = 0; i < 6; i++) {
		CRC = DoCRC(message[i], CRC);
		Serial.write(message[i]);
		delay(2);
	}
	// Sends the CRC.
	Serial.write(CRC);
	delay(2);
	Serial.write((unsigned int)CRC / 256);

	// Checks if the message reached the slave.
	for (int i = 0; i < 14; i++) {
		if (Serial.available() > 7) break;
		delay(5);
	}
	if (Serial.available() < 8) { // if message did not reach then return false.
		delete[] message;
		return false;
	}
	for (int i = 0; i < 6; i++) {
		if (Serial.read() != message[i]) { // if message is wrong return false.
			delete[] message;
			return false;
		}
	}
	if (Serial.read() != CRC % 256) { // if message is wrong return false.
		delete[] message;
		return false;
	}
	if (Serial.read() != (unsigned int)CRC / 256) { // if message is wrong return false.
		delete[] message;
		return false;
	}
	delete[] message;
	return true; // if it`s right return true.
}



int* ControlIOMODBUSPoll::ReadHoldingRegisters(int slaveID, int initialRegister, unsigned int amount) {
	// Creates the message to send to slave.
	short* message = new short[6];
	message[0] = slaveID; // slave id.
	message[1] = 0x03; // function code.
	message[2] = initialRegister / 256; // last 2 bytes of initial register.
	message[3] = initialRegister % 256; // first 2 bytes of initial register.
	message[4] = amount / 256; // last 2 bytes of amount of registers.
	message[5] = amount % 256; // first 2 bytes of amount of registers.
	// Does the crc and send the message at the same time.
	unsigned int CRC = 0xFFFF;
	for (int i = 0; i < 6; i++) {
		Serial.write(message[i]);
		CRC = DoCRC(message[i], CRC);
		delay(3);
	}
	// Sends the CRC.
	Serial.write(CRC);
	delay(2);
	Serial.write((unsigned int)CRC / 256);
	delete[] message;

	// Checks for the message 14 times.
	for (int i = 0; i < 14; i++) {
		if (Serial.available() >= 5+amount*2) break;
		delay(5);
	}
	if (Serial.available() < 5 + amount * 2) return new int[0]; // if nothing returns a new int with 1 element.

	// Reads the registers.
	message = new short[5 + amount * 2];
	CRC = 0XFFFF;
	for (int i = 0; i < 3 + amount * 2; i++) {
		message[i] = Serial.read();
		CRC = DoCRC(message[i], CRC);
	}
	unsigned int crc1 = Serial.read();
	crc1 += Serial.read() * 256;
	if (crc1 == CRC) { // Checks for the crc if right read the Registers elsewise will return new int with 1 element.
		delete[] resultRegisters;
		resultRegisters = new int[amount];
		for (int i = 0; i < amount * 2; i += 2) {
			resultRegisters[i] = message[3 + i] * 256 + message[4 + i];
		}
		delete[] message;
		return resultRegisters;
	} else {
		return new int[0];
	}
}
int* ControlIOMODBUSPoll::ReadInputRegisters(int slaveID, int initialRegister, unsigned int amount) {
	// Creates the message to send to slave.
	short* message = new short[6];
	message[0] = slaveID; // slave id.
	message[1] = 0x04; // function code.
	message[2] = initialRegister / 256; // last 2 bytes of initial register.
	message[3] = initialRegister % 256; // first 2 bytes of initial register.
	message[4] = amount / 256; // last 2 bytes of amount of registers.
	message[5] = amount % 256; // first 2 bytes of amount of registers.
	// Does the crc and send the message at the same time.
	unsigned int CRC = 0xFFFF;
	for (int i = 0; i < 6; i++) {
		Serial.write(message[i]);
		CRC = DoCRC(message[i], CRC);
		delay(3);
	}
	// Sends the CRC.
	Serial.write(CRC);
	delay(2);
	Serial.write((unsigned int)CRC / 256);
	delete[] message;

	// Checks for the message 14 times.
	for (int i = 0; i < 14; i++) {
		if (Serial.available() >= 5 + amount * 2) break;
		delay(5);
	}
	if (Serial.available() < 5 + amount * 2) return new int[0]; // if nothing returns a new int with 1 element.

	// Reads the registers.
	message = new short[5 + amount * 2];
	CRC = 0XFFFF;
	for (int i = 0; i < 3 + amount * 2; i++) {
		message[i] = Serial.read();
		CRC = DoCRC(message[i], CRC);
	}
	unsigned int crc1 = Serial.read();
	crc1 += Serial.read() * 256;
	if (crc1 == CRC) { // Checks for the crc if right read the Registers elsewise will return new int with 1 element.
		delete[] resultRegisters;
		resultRegisters = new int[amount];
		for (int i = 0; i < amount * 2; i += 2) {
			resultRegisters[i] = message[3 + i] * 256 + message[4 + i];
		}
		delete[] message;
		return resultRegisters;
	} else {
		return new int[0];
	}
}

bool* ControlIOMODBUSPoll::ReadCoils(int slaveID, int initialCoil, unsigned int amount) {
	// Creates the message to send to slave.
	short* message = new short[6];
	message[0] = slaveID; // slave id.
	message[1] = 0x01; // function code.
	message[2] = initialCoil / 256; // last 2 bytes of initial coil.
	message[3] = initialCoil % 256; // first 2 bytes of initial coil.
	message[4] = amount / 256; // last 2 bytes of amount of coils.
	message[5] = amount % 256; // first 2 bytes of amount of coils.
	// Does the crc and send the message at the same time.
	unsigned int CRC = 0xFFFF;
	for (int i = 0; i < 6; i++) {
		Serial.write(message[i]);
		CRC = DoCRC(message[i], CRC);
		delay(3);
	}
	// Sends the CRC.
	Serial.write(CRC);
	delay(2);
	Serial.write((unsigned int)CRC / 256);
	delete[] message;



	// Checks for the message 14 times.
	for (int i = 0; i < 14; i++) {
		if (Serial.available() >= 5+ceil(amount/(double)8)) break;
		delay(5);
	}
	if (Serial.available() < 5 + ceil(amount / (double)8)) return new bool[0]; // if nothing, returns a new bool with 1 element.

	// Read the coils.
	message = new short[5 + (int)ceil(amount / (double)8) - 2];
	CRC = 0XFFFF;
	for (int i = 0; i < 5 + ceil(amount / (double)8) - 2; i++) {
		message[i] = Serial.read();
		CRC = DoCRC(message[i], CRC);
	}
	unsigned int crc1 = Serial.read();
	crc1 += Serial.read() * 256;

	if (crc1 == CRC) { // Checks for the crc if right read the coils elsewise will return new bool with 1 element.
		delete[] result;
		result = new bool[amount];
		int b = 0;
		for (b = 0; b < floor(amount / (double)8); b++) {
			for (int i = 0; i < 8; i++) {
				result[i + (b * 8)] = message[b+3] % 2;
				message[b+3] /= 2;
			}
		}
		for (int i = 0; i < amount % 8; i++) {
			result[i + (b * 8)] = message[b+3] % 2;
			message[b+3] /= 2;
		}
		delete[] message;
		return result;
	} else {
		bool* result = new bool[0];
		result[0] = false;
		delete[] message;
		return new bool[0];
	}
	
}
bool* ControlIOMODBUSPoll::ReadInputStatus(int slaveID, int initialCoil, unsigned int amount) {
	// Creates the message to send to slave.
	short* message = new short[6];
	message[0] = slaveID; // slave id.
	message[1] = 0x02; // function code.
	message[2] = initialCoil / 256; // last 2 bytes of initial coil.
	message[3] = initialCoil % 256; // first 2 bytes of initial coil.
	message[4] = amount / 256; // last 2 bytes of amount of coils.
	message[5] = amount % 256; // first 2 bytes of amount of coils.
	// Does the crc and send the message at the same time.
	unsigned int CRC = 0xFFFF;
	for (int i = 0; i < 6; i++) {
		Serial.write(message[i]);
		CRC = DoCRC(message[i], CRC);
		delay(3);
	}
	// Sends the CRC.
	Serial.write(CRC);
	delay(2);
	Serial.write((unsigned int)CRC / 256);
	delete[] message;



	// Checks for the message 14 times.
	for (int i = 0; i < 14; i++) {
		if (Serial.available() >= 5 + ceil(amount / (double)8)) break;
		delay(5);
	}
	if (Serial.available() < 5 + ceil(amount / (double)8)) return new bool[0]; // if nothing, returns a new bool with 1 element.

	// Read the coils.
	message = new short[5 + (int)ceil(amount / (double)8) - 2];
	CRC = 0XFFFF;
	for (int i = 0; i < 5 + ceil(amount / (double)8) - 2; i++) {
		message[i] = Serial.read();
		CRC = DoCRC(message[i], CRC);
	}
	unsigned int crc1 = Serial.read();
	crc1 += Serial.read() * 256;

	if (crc1 == CRC) { // Checks for the crc if right read the coils elsewise will return new bool with 1 element.
		delete[] result;
		result = new bool[amount];
		int b = 0;
		for (b = 0; b < floor(amount / (double)8); b++) {
			for (int i = 0; i < 8; i++) {
				result[i + (b * 8)] = message[b + 3] % 2;
				message[b + 3] /= 2;
			}
		}
		for (int i = 0; i < amount % 8; i++) {
			result[i + (b * 8)] = message[b + 3] % 2;
			message[b + 3] /= 2;
		}
		delete[] message;
		return result;
	} else {
		bool* result = new bool[0];
		result[0] = false;
		delete[] message;
		return new bool[0];
	}
}

int ControlIOMODBUSPoll::DoCRC(int piece, unsigned short value) {
	value ^= piece; // XOR with the byte to the value
	for (int i = 0; i < 8; i++) {
		if (value % 2 == 1) {
			value = value / 2; // shifts the message 1 bit to the right.
			value = value ^ 0xA001; // XOR with 0xA001.
		} else {
			value = value / 2; // shifts the message 1 bit to the right.
		}
	}
	return value;
}
