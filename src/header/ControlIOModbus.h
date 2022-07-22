// ControlIOModbus.h

#ifndef _ControlIOModbus_h
#define _ControlIOModbus_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif


#endif
template <typename T>
class Node {
public:
	Node next;
	T val;
	Node(T val, Node* next = nullptr) {
		this->val = val;
		this->*next = next;
	}
};
#include <stdio.h>

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
	
	/// <summary>
	/// Creates a new Slave.
	/// </summary>
	/// <param name="slaveID">the slave id</param>
	ControlIOMODBUSSlave(unsigned int slaveID) {
		ControlIOMODBUSSlave::_SlaveID = slaveID;
	}
	
	/// <summary>
	/// Sets the coil to whatever pin you want.
	/// </summary>
	/// <param name="pinCoils">array of number representing the pin of the arduino you want to set as a coil</param>
	void SetCoils(bool** pinCoils);

	/// <summary>
	/// Sets the input status to whatever pin you want.
	/// </summary>
	/// <param name="pinRegisters">array of number representing the pin of the arduino you want to set as a input status</param>
	void SetInputStatus(int** pinRegisters);

	/// <summary>
	/// Sets the holding registers to whatever pin you want.
	/// </summary>
	/// <param name="pinRegisters">array of number representing the pin of the arduino you want to set as a register</param>
	void SetHoldingRegisters(int** pinRegisters);

	/// <summary>
	/// Sets the input registers to whatever pin you want.
	/// </summary>
	/// <param name="pinRegisters">array of number representing the pin of the arduino you want to set as a register</param>
	void SetInputRegisters(int** pinRegisters);
	/// <summary>
	/// Read the serial buffer to return another message in the serial if the message recieved is good.
	/// </summary>
	static void ReadMessage();

private:
	static unsigned int _SlaveID;

	static bool** coils;
	static int** inputStatus;
	static int** holdingRegisters;
	static int** inputRegisters;

	/// <summary>
	/// Returns the coils.
	/// </summary>
	/// <param name="first">first coil</param>
	/// <param name="amount">amount of coils to read</param>
	/// <returns>the coils you asked for</returns>
	static bool** ReturnCoils(int first, int amount);

	/// <summary>
	/// Returns the input status.
	/// </summary>
	/// <param name="first">first input status</param>
	/// <param name="amount">amount of input status to read</param>
	/// <returns>the input status you asked for</returns>
	static int** ReturnInputStatus(int first, int amount);

	/// <summary>
	/// Returns the holding registers.
	/// </summary>
	/// <param name="first">first register</param>
	/// <param name="amount">amount of registers to read</param>
	/// <returns>the registers you asked for</returns>
	static int** ReturnHoldingRegisters(int first, int amount);

	/// <summary>
	/// Returns the input registers.
	/// </summary>
	/// <param name="first">first register</param>
	/// <param name="amount">amount of registers to read</param>
	/// <returns>the registers you asked for</returns>
	static int** ReturnInputRegisters(int first, int amount);

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
	static void WriteRegister(int reg, unsigned int value);


	/// <summary>
	/// Does the crc of a byte use a lot of bytes to do the crc of a message.
	/// </summary>
	/// <param name="piece">1 byte of the message</param>
	/// <param name="value">the value buffer, the first value needs to be 0xFFFF</param>
	/// <returns>the CRC of the piece</returns>
	static int DoCRC(int piece, unsigned int value);
};