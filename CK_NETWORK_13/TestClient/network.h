#pragma once
#include "singleton.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>

#include <iostream>
#include <string>
#include <queue>
#include <atomic>

#pragma comment(lib,"ws2_32")
#pragma warning(disable:4996)


class NetworkModule : public Singleton<NetworkModule>
{
private:

	std::atomic_bool isInitialized;
	std::atomic_bool isConnected;

	WSAData data;
	SOCKET serverSocket;
	SOCKADDR_IN targetAddr;

	LPTSTR lastError;

	std::queue<std::string> messageQueue;
	std::thread ReceiveThread;

	void err_display(const char *msg)
	{
		if (lastError != nullptr)
			LocalFree(lastError);

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			lastError, 0, NULL);
	}

public:

	//192.168.31.115 - 정석님
	//192.168.31.38 - 나
	constexpr static auto _LOOP_BACK_IP = "127.0.0.1";
	constexpr static auto _PACKET_SIZE_ = 256;
	constexpr static auto _TARGET_PORT_ = 3317;

	NetworkModule() : isInitialized(false), isConnected(false), lastError(nullptr) {}

	
	NetworkModule& Initialize(const char* ip, const int port)
	{
		if (isInitialized)
		{
			CleanUp();
		}

		//WSA Init
		WSAData data;
		WSAStartup(MAKEWORD(2, 2), &data);

		//Set socket
		serverSocket = socket(AF_INET, SOCK_STREAM, 0);

		//MakeAddress to bind
		ZeroMemory(&targetAddr, sizeof(targetAddr)); // clean up field
		targetAddr.sin_family = AF_INET;
		targetAddr.sin_port = htons(port);
		inet_pton(AF_INET, ip, &targetAddr.sin_addr.s_addr);
		//targetAddr.sin_addr.s_addr = inet_addr(_LOOP_BACK_IP);

		isInitialized = true;
		return *this;
	}

	bool TryConnect()
	{
		if (connect(serverSocket, (SOCKADDR*)&targetAddr, sizeof(targetAddr)) == SOCKET_ERROR)
		{
			err_display("connect");
			return false;
		}

		isConnected = true;

		//Make Receive / SendThread
		ReceiveThread = std::thread(std::bind(&NetworkModule::Receive, this));

		return true;
	}

	//Send / Receive routine

	void Send(std::string data)
	{
		int result = send(serverSocket, data.c_str(), data.length(), 0);
		if (result == SOCKET_ERROR)
		{
			err_display("send()");
			isConnected = false;
		}
	}


	//Receive and enqueue data.
	//variable data length
	void Receive()
	{
		while (isConnected)
		{
			int result;
			int len;

			char buf[_PACKET_SIZE_ + 1];

			result = recv(serverSocket, (char *)&len, sizeof(int), 0);
			if (result == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (result == 0)
			{
				break;
			}

			// 데이터 받기(가변 길이)
			result = recv(serverSocket, buf, len, 0);
			if (result == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (result == 0)
			{
				break;
			}

			buf[result] = '\0';
			auto data = std::string(buf).substr(0, result);
			messageQueue.push(data);
		}
	}

	//release

	void CleanUp()
	{
		if (!isInitialized)
			return;

		if (isConnected)
		{
			closesocket(serverSocket);
			isConnected = false;

			//how to terminate Receive thread??

			//wait for loop end
			if (ReceiveThread.joinable())
				ReceiveThread.join();
		}

		WSACleanup();
		isInitialized = false;
	}


	std::string dequeueMessage()
	{
		if (messageQueue.empty())
			return std::string();

		auto temp = messageQueue.front();
		messageQueue.pop();
		return temp;
	}

	std::string SyncDequeueMessage()
	{
		while (messageQueue.empty());

		auto temp = messageQueue.front();
		messageQueue.pop();
		return temp;
	}

	std::string getLastError()
	{
		return lastError;
	}

	const bool getIsConnected() const { return isConnected; }
	const bool getIsInitialized() const { return isInitialized; }

};


/*
 * 데이터 직렬화(serialization) 고민하기
 * 데이터를 어떻게 넘길 것인가 고민하기
 * 메시지큐 스레드 세이프하게 작성하기 (cs/mutex)
 * 가변 데이터 전달 로직
 *
 */