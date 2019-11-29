#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <mutex>

#include <string>

#include <random>

#include "singleton.h"
#include "packet.h"

#pragma warning(disable:4996)
#define PORT 3317
#define BUFSIZE 512


#pragma region Utils / default functions


template <typename T>
int CustomRemove(std::vector<T> target, const T& rhs)
{
	auto lastSize = target.size();
	auto end = target.erase(std::remove_if(target.begin(), target.end(),
		[=](const T& t)
	{
		return t == rhs;
	}), target.end());

	return (lastSize - std::distance(target.begin(), end)) / sizeof(T);
}
void err_quit(const char *msg);
void err_display(const char *msg);
void Protocol(Packet& received, SOCKET sender);
void Send(Packet packet, SOCKET socket);
DWORD WINAPI ProcessClient(LPVOID arg);

void err_quit(const char *msg) 
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char *msg) 
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

struct Connection
{
	SOCKET sock;
	SOCKADDR_IN addr;
	int addrlen;
};

#pragma endregion

#pragma region ServerConnectionLogic

std::vector<Connection> connections;
std::mutex connectionsLock;

void Send(Packet packet, SOCKET socket)
{
	auto message = packet.ToString();
	int len = message.size();

	int retVal = send(socket, (char*)&len, sizeof(int), 0);
	if (retVal == SOCKET_ERROR)
	{
		err_display("send()");
		return;
	}

	retVal = send(socket, message.c_str(), len, 0);
	if (retVal == SOCKET_ERROR)
	{
		err_display("send()");
		return;
	}
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retVal;
	SOCKADDR_IN clientAddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	addrlen = sizeof(clientAddr);
	getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrlen);

	while (true)
	{
		retVal = recv(client_sock, buf, BUFSIZE, 0);
		if (retVal == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}

		else if (retVal == 0)
			break;

		buf[retVal] = '\0';

		//not string.
		//int len = strlen(buf);
		int len = retVal;

		//Packet access
		Packet received = Packet::ToPacket(std::string(buf, retVal));

		printf("[TCP/%s:%d] Send Protocol : %d \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), received.opcode);

		//error
		if (received.opcode == OpCodes::None)
		{
			received.error_code = ErrorCodes::kBadRequest;

			Send(received, client_sock);
			continue;
		}

		//settin

		Protocol(received, client_sock);
	}

	//remove target client
	connectionsLock.lock();

	connections.erase(std::remove_if(connections.begin(), connections.end(),
		[=](const Connection& t)
	{
		return t.sock == client_sock;
	}), connections.end());

	closesocket(client_sock);
	connectionsLock.unlock();

	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	return 0;
}

int main()
{
	int retVal;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 1;
	}

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(PORT);

	retVal = bind(listen_sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR) err_quit("bind()");

	retVal = listen(listen_sock, SOMAXCONN);
	if (retVal == SOCKET_ERROR) err_quit("listen()");

	SOCKET client_sock;
	SOCKADDR_IN clientAddr;
	int addrlen;
	HANDLE hThread;

	while (true)
	{
		addrlen = sizeof(clientAddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientAddr, &addrlen);
		if (retVal == SOCKET_ERROR)
		{
			err_display("accept()");
			break;
		}

		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		
		hThread = CreateThread(0, 0, ProcessClient, (LPVOID)client_sock, 0, 0);
		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
			connectionsLock.lock();

			connections.emplace_back(Connection
			{
				client_sock,
				clientAddr,
				addrlen
			});

			connectionsLock.unlock();
		}
	}

	closesocket(listen_sock);

	WSACleanup();
	return 0;
}


#pragma endregion



#pragma region GamePlayLogic


void Protocol(Packet& received, SOCKET sender)
{
	switch (received.opcode)
	{
		case OpCodes::kRequestInitialize:
		{
			if (connections.size() != 2)
				break;

			//setting Initialize data

			auto other = std::find_if(connections.begin(), connections.end(), [=](Connection temp)
			{
				return temp.sock != sender;
			});

			std::mt19937 mt((std::random_device())());
			std::uniform_int_distribution<> dist(0, 1);

			//make random
			auto random_value = dist(mt);

			Player a;
			a.PlayerPosition = vector2(-100, 0);
			a.color32 = vector3Int(226, 12, 103);
			a.bulletColor32 = vector3Int(148, 7, 67);
			a.pid = random_value;

			Player b;
			b.PlayerPosition = vector2(100, 0);
			b.color32 = vector3Int(151, 204, 40);
			b.bulletColor32 = vector3Int(99, 133, 27);
			b.pid = random_value == 0 ? 1 : 0;

			InitializeMessage init_msg_a;
			init_msg_a.isFirst = true;
			init_msg_a.player_info = a;
			init_msg_a.other_info = b;
			init_msg_a.pid = a.pid;
			init_msg_a.default_turn_time = 10.f;

			InitializeMessage init_msg_b;
			init_msg_b.isFirst = false;
			init_msg_b.player_info = b;
			init_msg_b.other_info = a;
			init_msg_b.pid = b.pid;
			init_msg_b.default_turn_time = 10.f;

			Packet packet_a;
			packet_a.opcode = OpCodes::kResponseInitialize;
			packet_a.error_code = ErrorCodes::kOK;
			packet_a.response.Initialize_message = init_msg_a;

			Packet packet_b;
			packet_b.opcode = OpCodes::kResponseInitialize;
			packet_b.error_code = ErrorCodes::kOK;
			packet_b.response.Initialize_message = init_msg_b;

			Send(random_value == 0 ? packet_a : packet_b, sender);
			Send(random_value == 0 ? packet_b : packet_a, (*other).sock);

			printf("kRequestInitialize sended \n");

			break;
		} // kRequestInitialize end

		case OpCodes::kFire:
		{
			Packet packet;
			packet.opcode = OpCodes::kFire;
			packet.error_code = ErrorCodes::kOK;
			packet.response.fire_message = received.request.fire_message;

			for (auto it : connections)
			{
				Send(packet, it.sock);
			}

			printf("kFire sended \n");

			break;
		}//kFire end

		case OpCodes::kRequestTurnEnd:
		{
			TurnOverMessage turn_over_message;
			turn_over_message.last_pid = received.request.turn_over_message.last_pid;
			turn_over_message.current_pid = received.request.turn_over_message.last_pid == 0 ? 1 : 0;

			Packet packet;
			packet.opcode = OpCodes::kResponseTurnEnd;
			packet.error_code = ErrorCodes::kOK;
			packet.response.turn_over_message = turn_over_message;

			for (auto it : connections)
			{
				Send(packet, it.sock);
			}

			break;
		}

		case OpCodes::kRequestPID:
		{

			break;
		}

		case OpCodes::kRequestFirst:
		{
			break;
		}

		case OpCodes::kRequestHit:
		{
			break;
		}

		default:
		{
			//default is RPC
			memcpy_s(received.response.request_data, 128, received.request.request_data, 128);
			received.error_code = ErrorCodes::kOK;
			for (auto it : connections)
			{
				Send(received, it.sock);
			}

			break;
		}
	}
}

#pragma endregion