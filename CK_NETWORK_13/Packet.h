#pragma once
#include <string>

#include "network.h"
#include "vector.h"

/*
 * OperationCode : 호출 명령
 * ErrorCode : 서버내에서 오류 발생시 나타낼 코드
*/

/*
 * Request : 클라이언트가 서버에게 요청할 때 전달할 메시지.
 * Response : 서버가 클라이언트에게 전달할 메시지
*/

enum class OpCodes
{
	None = 0,

	//connected

	//match making
	kRequestPID = 10,	//pid 생성 및 전달 요청
	kResponsePID,

	kRequestFirst, // 누가 선공인가
	kResponseFirst,

	kStart,

	//in game
	kFire = 20,

	kTurnOver,
	kTurnEnd,

	kMove,

	kRequestHit,
	kResponseHit,


	kGameEnd,
};

enum ErrorCodes
{
	None = 0,

	kOK = 100,
	kUnknown = 101,
	kFieldMissing = 102,
	kBadPacket = 103,
	kNotConnectedServer = 104,

	kVersionMismatch = 106,
	kNotSupported = 107,
	kUnauthorized = 108,

	kInvalidIdx = 115,
	kInvalidId = 117,
	kBadRequest = 118,
	kBadLength = 119,

	kWrongReceipt = 130,

	kAlreadyProgress = 140,
	kTransaction = 144
};

struct PIDMessage
{
	int pid;
};

struct FireMessage
{
	//fire target
	int pid;

	//fired position
	vector2 position;

	//shot angle
	float angle;
};

struct MoveMessage
{
	//move target
	int pid;

	//Start move ? or moving end?
	bool isKeyDown;

	//starting / destination position
	vector2 position;

	//vk
	int KeyCode;
};

struct HitMessage
{
	int pid;

	float amount;

	vector2 position;
	vector2 size;
};

struct TurnOverMessage
{
	//move target
	int pid;

	//next pid

};

struct GameEndMessage
{
	
};

struct namae
{
	//move target
	int pid;

	//next pid
	const char* tt;

};

struct Packet
{
	Packet() : opcode(OpCodes::None), error_code(ErrorCodes::None), request{ 0, }, response{ 0, }	{	}

	Packet(const Packet& packet) : request{ 0, }, response{ 0, }
	{
		memcpy_s(this, sizeof(*this), &packet, sizeof(packet));
	}

	OpCodes opcode;

	ErrorCodes error_code;

	union 
	{
		//Request
		char request_data[NetworkModule::_PACKET_SIZE_ / 2];

		PIDMessage pid_message;

		FireMessage fire_message;
		HitMessage hit_message;
		MoveMessage move_message;

		TurnOverMessage turn_over_message;
		GameEndMessage game_end_message;

	} request;

	union
	{
		//Response
		char request_data[NetworkModule::_PACKET_SIZE_ / 2];

		PIDMessage pid_message;

		FireMessage fire_message;
		HitMessage hit_message;
		MoveMessage move_message;

		TurnOverMessage turn_over_message;
		GameEndMessage game_end_message;
	} response;

	const std::string ToString()
	{
		return std::string((char*)this, sizeof(*this));
	}

	static const Packet ToPacket(std::string message)
	{
		Packet packet;

		auto temp = reinterpret_cast<Packet*>(const_cast<char*>(message.c_str()));
		memcpy_s(&packet, sizeof(packet), temp, message.size());

		return packet;
	}
};
