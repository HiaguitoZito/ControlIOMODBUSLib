// ControlIOModbus.h

#ifndef _ControlIOModbus_h
#define _ControlIOModbus_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif


#endif

class ControlIOMODBUSPoll {
public:
	ControlIOMODBUSPoll() {}

	/// <summary>
	/// Reads the input registers of your choice.
	/// </summary>
	/// <param name="slaveID">the slave's id</param>
	/// <param name="initialRegister">the first register you want to read</param>
	/// <param name="amount">the amount of registers you want to read from the first register</param>
	/// <returns>an array of int representing the registers</returns>
	int* ReadInputRegisters(int slaveID, int initialRegister, unsigned int amount);

	/// <summary>
	/// Reads the Holding registers of your choice.
	/// </summary>
	/// <param name="slaveID">the slave's id</param>
	/// <param name="initialRegister">the first register you want to read</param>
	/// <param name="amount">the amount of registers you want to read from the first register</param>
	/// <returns>an array of int representing the registers</returns>
	int* ReadHoldingRegisters(int slaveID, int initialRegister, unsigned int amount);

	/// <summary>
	/// Reads the coils of your choice.
	/// </summary>
	/// <param name="slaveID">the slave's id</param>
	/// <param name="initialCoil">the first coil you want to read</param>
	/// <param name="amount">the amount coils you want to read from the first coil</param>
	/// <returns>an array of bool representing the coils</returns>
	bool* ReadCoils(int slaveID, int initialCoil, unsigned int amount);

	/// <summary>
	/// Reads the coils of your choice.
	/// </summary>
	/// <param name="slaveID">the slave's id</param>
	/// <param name="initialCoil">the first coil you want to read</param>
	/// <param name="amount">the amount coils you want to read from the first coil</param>
	/// <returns>an array of bool representing the coils</returns>
	bool* ReadInputStatus(int slaveID, int initialCoil, unsigned int amount);

	/// <summary>
	/// Writes whatever coil you want.
	/// </summary>
	/// <param name="slaveID">the slave's id</param>
	/// <param name="coil">the coil you want to write</param>
	/// <param name="value">the value you want to write in it</param>
	/// <returns>true if it went write, false if not</returns>
	bool WriteSingleCoil(int slaveID, int coil, bool value);

	/// <summary>
	/// Writes whatever register you want.
	/// </summary>
	/// <param name="slaveID">the slave's id</param>
	/// <param name="coil">the register you want to write</param>
	/// <param name="value">the value you want to write in it</param>
	/// <returns>true if it went write, false if not</returns>
	bool WriteSingleRegister(unsigned int slaveID, unsigned int reg, unsigned int value);

private:

	/// <summary>
	/// Does the crc of a byte use a lot of bytes to do the crc of a message.
	/// </summary>
	/// <param name="piece">1 byte of the message</param>
	/// <param name="value">the value buffer, the first value needs to be 0xFFFF</param>
	/// <returns>the CRC of the piece</returns>
	int DoCRC(int piece, unsigned short value);
};

class ControlIOMODBUSSlave {
public:
	static bool** coils;
	static int** inputStatus;
	static unsigned int** holdingRegisters;
	static int** inputRegisters;
	static int coilAmount;
	static int inputStatusAmount;
	static int holdingRegistersAmount;
	static int inputRegistersAmount;
	/// <summary>
	/// Creates a new Slave.
	/// </summary>
	/// <param name="slaveID">the slave id</param>
	ControlIOMODBUSSlave(unsigned int slaveID, int coilAmount, int inputStatusAmount, int holdingRegistersAmount, int inputRegistersAmount) {
		ControlIOMODBUSSlave::_SlaveID = slaveID;
		this->coilAmount = coilAmount;
		this->inputStatusAmount = inputStatusAmount;
		this->holdingRegistersAmount = holdingRegistersAmount;
		this->inputRegistersAmount = inputRegistersAmount;
		delete[] coils;
		delete[] inputStatus;
		delete[] holdingRegisters;
		delete[] inputRegisters;
		// Creates the registers/coils
		coils = new bool* [coilAmount];
		for (int i = 0; i < coilAmount; i++) {
			coils[i] = new bool{ false };
		}
		inputRegisters = new int* [inputRegistersAmount];
		for (int i = 0; i < inputRegistersAmount; i++) {
			inputRegisters[i] = new int{ 10 };
		}
		holdingRegisters = new unsigned int* [holdingRegistersAmount];
		for (int i = 0; i < holdingRegistersAmount; i++) {
			holdingRegisters[i] = new unsigned int{ 0 };
		}
		inputStatus = new int* [inputStatusAmount];
		for (int i = 0; i < inputStatusAmount; i++) {
			inputStatus[i] = new int{ 0 };
		}
	}
	/// <summary>
	/// Read the serial buffer to return another message in the serial if the message recieved is good.
	/// </summary>
	static void ReadMessage();

private:
	static unsigned int _SlaveID;

	/// <summary>
	/// Writes a coil.
	/// </summary>
	/// <param name="coil">the coil you want to write</param>
	/// <param name="value">the value you want that coil to be</param>
	static void WriteCoil(int coil, unsigned int value);

	/// <summary>
	/// Writes a register.
	/// </summary>
	/// <param name="reg">the register you want to write</param>
	/// <param name="value">the value you want that register to be</param>
	static void WriteRegister(int reg, int value);


	/// <summary>
	/// Does the crc of a byte use a lot of bytes to do the crc of a message.
	/// </summary>
	/// <param name="piece">1 byte of the message</param>
	/// <param name="value">the value buffer, the first value needs to be 0xFFFF</param>
	/// <returns>the CRC of the piece</returns>
	static int DoCRC(short piece, unsigned int value);
};