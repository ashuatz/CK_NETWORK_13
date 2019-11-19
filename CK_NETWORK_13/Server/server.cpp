#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <mutex>

#pragma warning(disable:4996)
#define PORT 3317
#define BUFSIZE 512


void err_quit(const char *msg) {
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

void err_display(const char *msg) {
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

std::vector<Connection> connections;
std::mutex connectionsLock;

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
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buf);

		int len = strlen(buf);

		for (auto it : connections)
		{
			retVal = send(it.sock, (char*)&len, sizeof(int), 0);
			if (retVal == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}

			retVal = send(it.sock, buf, len, 0);
			if (retVal == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
		}
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