#include "ControlIOModbus.h"

void serialEvent() {
	ControlIOMODBUSSlave::ReadMessage();
}

// Variables Declaration.
unsigned int ControlIOMODBUSSlave::_SlaveID = 1;

bool** ControlIOMODBUSSlave::coils = new bool* [ControlIOMODBUSSlave::coilAmount];
int** ControlIOMODBUSSlave::inputStatus = new int* [ControlIOMODBUSSlave::inputStatusAmount];
unsigned int** ControlIOMODBUSSlave::holdingRegisters = new unsigned int* [ControlIOMODBUSSlave::holdingRegistersAmount];
int** ControlIOMODBUSSlave::inputRegisters = new int* [ControlIOMODBUSSlave::inputStatusAmount];

int ControlIOMODBUSSlave::coilAmount = 1;
int ControlIOMODBUSSlave::inputStatusAmount = 1;
int ControlIOMODBUSSlave::holdingRegistersAmount = 1;
int ControlIOMODBUSSlave::inputRegistersAmount = 1;

void ControlIOMODBUSSlave::WriteCoil(int coil, unsigned int value) {
	*ControlIOMODBUSSlave::coils[coil] = value > 0 ? true : false;
}

void ControlIOMODBUSSlave::WriteRegister(int reg, int value) {
	*ControlIOMODBUSSlave::holdingRegisters[reg] = value;
}



int ControlIOMODBUSSlave::DoCRC(short piece, unsigned int value) {
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

int Pow(int a, int b) {
	int result = 1;
	for (int i = 0; i < b; i++) {
		result *= a;
	}
	return result;
}

void ControlIOMODBUSSlave::ReadMessage() {
	if (Serial.available() != 0) {
		// waits 20 milliseconds to see if it's a real message, if not it'll clean the serial buffer.
		for (int i = 0; i < 200; i++) {
			if (Serial.available() >= 8) break;
			delay(1);
		}
		if (Serial.available() < 8) {
			// This will clean the serial buffer.
			while (Serial.available() != 0) {
				Serial.read();
			}
			return;
		}

		// Read the message and calculates the crc.
		int x = Serial.available();
		unsigned int CRC2Compare = 0xFFFF;
		unsigned short* message = new unsigned short[6];
		delay(1);
		for (int i = 0; i < x - 2; i++) {
			unsigned int s = Serial.read(); // Reads serial buffer and stores the byte in an integer named "s".
			message[i] = s; // Stores it in the array "message".
			CRC2Compare = DoCRC(s, CRC2Compare); // Does the CRC to compare later on.
		}
		int crc1 = Serial.read();
		crc1 += Serial.read() * 256;
		// Check for CRC if it's correct then it'll read the message.
		if (CRC2Compare == crc1) {

			// Checks if the message ID is equal to this is slave ID or 00.
			if (message[0] == _SlaveID || message[0] == 0x00) {

				// Return the coils (Function 0x01).
				if (message[1] == 0x01) {
					// Will read how many coils the master wants.
					short initialCoil = message[2] * 256 + message[3];
					short coilAmount = message[4] * 256 + message[5];
					delete[] message;
					// creates the message.
					message = new unsigned short[(int)ceil(coilAmount / (double)8)];
					message[0] = _SlaveID; // Slave ID.
					message[1] = 0x01; // Function.
					message[2] = (int)ceil(coilAmount / (double)8); // Amount of Coils in Bytes.

					// Add the coils to the message.
					int b = 0;
					for (b = 0; b < floor(coilAmount / (double)8); b++) {
						unsigned int messageCoil = 0x00;
						for (int i = 0; i < 8; i++) {
							messageCoil += *coils[i + (b * 8)] ? Pow(2, i) : 0; // Turn the coils into Hex value.
						}
						message[3 + b] = messageCoil;
					}

					// Add the remaining coils.
					int messageCoil = 0x00;
					for (int i = 0; i < coilAmount % 8; i++) {
						messageCoil += *coils[i + (b * 8)] * Pow(2, i); // Turn the coils into binary value.
					}
					message[3 + b] = messageCoil;

					// Will make the CRC of the message and send it a the same time.
					unsigned int CRC = 0xFFFF;
					for (int i = 0; i < 3 + (int)ceil(coilAmount / (double)8); i++) {
						CRC = DoCRC(message[i], CRC);
						Serial.write(message[i]);
						delay(2);
					}

					// Will send the CRC with the message.
					Serial.write(CRC);
					delay(2);
					Serial.write(CRC / 256);


					delete[] message;
				}

				// Read discrete input status (Function 0x02).
				if (message[1] == 0x02) {
					// Will read how many input Status the master wants.
					short initialRegister = message[2] * 256 + message[3];
					short amount = message[4] * 256 + message[5];
					delete[] message;
					// Creates the message.
					message = new unsigned short[3 + (int)ceil(amount / (double)8)];
					message[0] = _SlaveID; // Slave ID.
					message[1] = 0x02; // Function.
					message[2] = ceil(amount / (double)8); // Amount of Registers in Bytes.

					// Add the coils to the message.
					int b = 0;
					for (b = 0; b < floor(amount / (double)8); b++) {
						unsigned int messageCoil = 0x00;
						for (int i = 0; i < 8; i++) {
							messageCoil += *inputStatus[i + (b * 8)] * Pow(2, i); // Turn the coils into binary value.
						}
						message[3 + b] = messageCoil;
					}

					// Add the remaining coils.
					int messageCoil = 0x00;
					for (int i = 0; i < amount % 8; i++) {
						messageCoil += *inputStatus[i + (b * 8)] * Pow(2, i); // Turn the coils into binary value.
					}
					message[3 + b] = messageCoil;

					// Will make the CRC of the message and send it a the same time.
					unsigned int CRC = 0xFFFF;
					for (int i = 0; i < 3 + (int)ceil(amount / (double)8); i++) {
						CRC = DoCRC(message[i], CRC); // Does the CRC and send the message at the same time.
						Serial.write(message[i]);
						delay(2);
					}

					// Will send the CRC with the message.
					Serial.write(CRC);
					delay(2);
					Serial.write((unsigned int)CRC / 256);

					delete[] message;
				}

				// Read holding registers (Function 0x03).
				if (message[1] == 0x03) {

					// Reads the registers
					unsigned short initialRegister = message[2] * 256 + message[3];
					unsigned short amount = message[4] * 256 + message[5];
					delete[] message;
					if (amount - initialRegister > holdingRegistersAmount) {
						Serial.write(0x01);
						delay(2);
						Serial.write(0x83);
						delay(2);
						Serial.write(0x02);
						delay(2);
						Serial.write(0xC0);
						delay(2);
						Serial.write(0xF1);
						return;
					}
					// Creates the message.
					message = new unsigned short[3 + amount * 2];
					message[0] = ControlIOMODBUSSlave::_SlaveID; // Slave ID.
					message[1] = 0x03; // Function.
					message[2] = amount * 2; // Amount of registers.

					// Adds the registers to the message.
					for (int i = 0; i < amount * 2; i += 2) {
						message[i + 3] = *holdingRegisters[initialRegister + i / 2] / 256;
						message[i + 4] = *holdingRegisters[initialRegister + i / 2] % 256;
					}
					// Does the crc of the message and sends the message.
					unsigned int CRC2Send = 0xFFFF;
					for (int i = 0; i < 3 + amount * 2; i++) {
						Serial.write(message[i]); // Writes a chunk of the message in the serial buffer.
						CRC2Send = DoCRC(message[i], CRC2Send);
						delay(2);
					}

					// Sends the CRC.
					Serial.write(CRC2Send);
					delay(2);
					Serial.write(CRC2Send / 256);
					delete[] message;
				}

				// Read Input Registers (Function 0x04).
				if (message[1] == 0x04) {
					// Reads the registers
					short initialRegister = message[2] * 256 + message[3];
					short amount = message[4] * 256 + message[5];
					delete[] message;
					// Creates the message.
					message = new unsigned short[3 + amount * 2];
					message[0] = _SlaveID; // Slave ID.
					message[1] = 0x04; // Function.
					message[2] = amount * 2; // Amount of registers.

					// Adds the registers to the message.
					for (int i = 0; i < amount * 2; i += 2) {
						message[i + 3] = *inputRegisters[initialRegister + i / 2] / 256;
						message[i + 4] = *inputRegisters[initialRegister + i / 2] % 256;
					}
					// Does the crc of the message and sends the message.
					unsigned int CRC2Send = 0xFFFF;
					for (int i = 0; i < 3 + amount * 2; i++) {
						Serial.write(message[i]); // Writes a chunk of the message in the serial buffer.
						CRC2Send = DoCRC(message[i], CRC2Send);
						delay(2);
					}

					// Sends the CRC.
					Serial.write(CRC2Send);
					delay(2);
					Serial.write(CRC2Send / 256);
					delete[] message;
				}

				// Write single coil (Function 0x05).
				if (message[1] == 0x05) {
					int coil = message[2] * 256 + message[3]; // Gets the coil the master want to write.
					int value = message[4] * 256 + message[5]; // Gets the value the master wants to write in the coil.
					WriteCoil(coil, value); // Writes the coil.

					// Resends the message the master sent so it can see that the coil went write.
					for (int i = 0; i < 6; i++) {
						Serial.write(message[i]);
						delay(2);
					}
					Serial.write(crc1);
					delay(2);
					Serial.write((unsigned int)crc1 / 256);
					delete[] message;
				}

				// Write single register (Function 0x06).
				if (message[1] == 0x06) {
					int reg = message[2] * 256 + message[3]; // Gets the register the master want to write.
					int value = message[4] * 256 + message[5]; // Gets the value the master wants to write in the coil.
					WriteRegister(reg, value); // Writes the coil.

					// Resends the message the master sent so it can see that the coil went write.
					for (int i = 0; i < 6; i++) {
						Serial.write(message[i]);
						delay(2);
					}
					Serial.write(crc1);
					delay(2);
					Serial.write((unsigned int)crc1 / 256);
					delete[] message;
				}
			}
		}
	}
}

