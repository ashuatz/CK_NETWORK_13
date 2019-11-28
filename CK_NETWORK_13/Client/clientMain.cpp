#include "stdafx.h"

#include "network.h"
#include "vector.h"
#include "matrix.h"
#include "Timer.h"
#include "object.h"
#include "packet.h"

#include <Windows.h>
#include <sstream>
#include <string>
#include <iomanip>

#include <algorithm>

int PID;
bool g_bIsActive;
int g_nClientWidth = 640;
int g_nClientHeight = 480;

// 전역 변수:
HWND hRootWnd;
TCHAR szWndAppName[] = TEXT("Client");
HINSTANCE g_Instance;

//유틸
#define RectToParam(rect) rect.left,rect.top,rect.right,rect.bottom
#define Vector2ToParam(vector) vector.x,vector.y

//정방향 선언
void FixedUpdate();
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR    lpCmdLine, int       nCmdShow)
{
	// 1. 윈도우 클래스 정의 
	WNDCLASS wc = { 0 };
	wc.style = 0;			   									// 윈도우 클래스의 스타일 종류
	wc.lpfnWndProc = WndProc;									// 메시지 처리 함수인 윈도우 프로시저 포인터	
	wc.cbClsExtra = 0;											// 윈도우 클래스 데이터 예약 영역 초기값 0  
	wc.cbWndExtra = 0;											// 윈도우 데이터 예약 영역, 초기값 0  
	wc.hInstance = hInstance; 									// 해당 윈도우 클래스를 등록한 응용프로그램의 인스턴스
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);					// 윈도우가 최소화 될때 보여줄 아이콘 	
	wc.hCursor = LoadCursor(0, IDC_CROSS);						// 해당 윈도우 클래스로 만들어진 윈도우의 마우스 커서 핸들, IDC_ARROW, IDC_CROSS, IDC_IBEAM, IDC_NO, IDC_WAIT
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		// 윈도우 작업 영역을 칠할 배경 브러쉬 핸들 (흰색)	
	wc.lpszMenuName = NULL;										// 사용할 메뉴를 지정한다. 	
	wc.lpszClassName = szWndAppName;							// 등록하고자 하는 윈도우 클래스 이름 

	if (!RegisterClass(&wc))
		return -1;

	// 2. 윈도우 생성 
	hRootWnd = CreateWindow(
		szWndAppName,					    //lpClassName
		szWndAppName, 					    //lpWindowName
		WS_OVERLAPPEDWINDOW ^ (WS_MINIMIZEBOX | WS_MAXIMIZEBOX),			    //dwStyle
		//(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU  | WS_MINIMIZEBOX | WS_MAXIMIZEBOX ),
		(1920 / 2) - (g_nClientWidth / 2),				//x
		(1080 / 2) - (g_nClientHeight / 2), 			//y
		g_nClientWidth -1, 					            //Width
		g_nClientHeight -1, 								//Height
		NULL,							    //hWndParent
		NULL,							    //hMenu
		hInstance,						    //hInstance
		NULL);	     					    //lpParam

	g_Instance = hInstance;

	// 3. 윈도우 화면 출력 
	ShowWindow(hRootWnd, nCmdShow);

	// 4. 메시지 루프 

	MSG Message;
	HDC hdc = GetDC(hRootWnd);
	while (true)
	{
		//System

		//physics
		FixedUpdate();

		//time
		Time::GetInstance().CheckCounter();

		//io
		if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			if (Message.message == WM_QUIT) break;

			TranslateMessage(&Message);       //WM_KEYDOWN이고 키가 문자 키일 때 WM_CHAR 발생
			DispatchMessage(&Message);        //콜백 프로시저가 수행할 수 있게 디스패치 시킴
		}
		else
		{
			WaitMessage();
		}
	}

	ReleaseDC(hRootWnd, hdc);

	return 0;
}

//test : temp variable

Matrix3x3 mat;

Player me;
Player other;

Bullet bullet;

World world;

constexpr float targetRenderTime = 0.016;
float renderTime = 0;

void FixedUpdate()
{
	auto dt = Time::GetInstance().GetDeltaTime() * 0.01; // 0.001f
	renderTime += dt;

	if (bullet.isAlive)
	{
		bullet.position += (bullet.moveDirection * bullet.power + vector2(0, world.GetMaxGravity(bullet.gravityTime))) * dt;

		bullet.gravityTime += dt;

		//smoothDamp

		//camera follow bulletPosition
		world.worldOffset += (-bullet.position - world.worldOffset) * 0.02f;

		//x = velocity * cos(θ) * t
		//y = velocity * sin(θ) * t  - 1 / 2g * t ^ 2

		//collision

		if (bullet.position.y < 0)
		{
			//release bullet
			bullet.isAlive = false;
			
			//auto packet = Packet();

			//packet.opcode = OpCodes::kTurnOver;
			//packet.request.turn_over_message = TurnOverMessage
			//{
			//	//pid
			//	1
			//};

			//NetworkModule::GetInstance().Send(packet.ToString());
		}
	}
	else
	{
		world.worldOffset = world.defaultWorldOffset;
	}

	if (renderTime > targetRenderTime)
	{
		renderTime -= targetRenderTime;
		InvalidateRect(hRootWnd, 0, 0);

		//debug : logging deltaTime , frameRate
		auto titleName = (std::stringstream() << std::setw(4) << std::setfill(_T(' ')) << Time::GetInstance().GetFrame()).str()
			.append(_T(" Frame    "))
			.append((std::stringstream() << std::setw(8) << std::setfill(_T(' ')) << std::setprecision(5) << (Time::GetInstance().GetDeltaTime())).str())
			.append(_T(" ms"))
			.append(_T(" max      "))
			.append((std::stringstream() << std::setw(8)<< std::setfill(_T(' '))<<std::setprecision(5)<<(Time::GetInstance().GetMaxDeltaTime())).str());

		SetWindowText(hRootWnd, titleName.c_str());
	}
}


const RECT GetRect(const vector2& position, const vector2& size)
{
/* 
	LONG    left;
	LONG    top;
	LONG    right;
	LONG    bottom;
 */

	return RECT{
		static_cast<LONG>(position.x - (size.x / 2)),
		static_cast<LONG>(position.y - (size.y / 2)),
		static_cast<LONG>(position.x + (size.x / 2)),
		static_cast<LONG>(position.y + (size.y / 2)) };
}

const vector2 GetRenderPosition(const vector2& position)
{
	return vector2(
		g_nClientWidth * 0.5f + position.x + world.worldOffset.x,
		g_nClientHeight * 0.5f - (position.y + world.worldOffset.y));
}


// 0.1. 윈도우 프로시저 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD dwRead;

	switch (message)
	{
		case WM_CREATE:
		{
			me.PlayerPosition = vector2(0, 0);
			me.PlayerScale = vector2(1, 1);
			me.PlayerSize = vector2(20, 20);

			bullet.Init(me.PlayerPosition, vector2(1, 1));

			return 0;
		}

		case WM_KEYDOWN:
		{
			//send message to server (IO)

			//Game logic MUST be handled by the MessageQueue.

			//camera offset test code

			switch (wParam) 
			{
				case VK_SPACE:
				{
					//test PID request function
					Packet packet;
					packet.opcode = OpCodes::kRequestPID;
					NetworkModule::GetInstance().Send(packet.ToString());

					const auto message = NetworkModule::GetInstance().SyncDequeueMessage();
					Packet received = Packet::ToPacket(message);
					if (received.error_code != ErrorCodes::kOK)
					{
						//error
						break;
					}

					PID = received.response.pid_message.pid;

					break;
				}
				case VK_DOWN: world.defaultWorldOffset += vector2(0, -2); break;
				case VK_UP:world.defaultWorldOffset += vector2(0, 2); break;

				case VK_LEFT:world.defaultWorldOffset += vector2(2, 0); break;
				case VK_RIGHT:world.defaultWorldOffset += vector2(-2, 0); break;

				default:
					break;
			}

			InvalidateRect(hWnd, 0, 1);

			//offset test code end

			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			//test : bullet fire code
			bullet.isAlive = true;

			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			//test : bullet initialize Code
			bullet.Init(me.PlayerPosition, vector2(1, 1));
			return 0;
		}

		case WM_PAINT:
		{
			static HDC memDC, hdc;
			static HBITMAP memoryMap, screenMap;
			static RECT bufferRT;

			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd, &ps);

			GetClientRect(hWnd, &bufferRT);
			memDC = CreateCompatibleDC(hdc);
			memoryMap = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
			screenMap = (HBITMAP)SelectObject(memDC, memoryMap);

			PatBlt(memDC, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);


			//HDC hdc = hMemoryDC;

			vector2 vector = vector2(1, 1);

			//test : render

			//horizon
			MoveToEx(memDC, Vector2ToParam(GetRenderPosition(vector2(-INT16_MAX, 0))), 0);
			LineTo(memDC, Vector2ToParam(GetRenderPosition(vector2(INT16_MAX, 0))));

			//player
			Rectangle(memDC, RectToParam(GetRect(GetRenderPosition(me.PlayerPosition), me.PlayerSize)));
			Rectangle(memDC, RectToParam(GetRect(GetRenderPosition(other.PlayerPosition), other.PlayerSize)));

			//bullet
			if (bullet.isAlive)
			{
				Rectangle(memDC, RectToParam(GetRect(GetRenderPosition(bullet.position), bullet.size)));
			}


			//
			GetClientRect(hWnd, &bufferRT);
			BitBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, memDC, 0, 0, SRCCOPY);
			SelectObject(memDC, screenMap);
			DeleteObject(memoryMap);
			DeleteDC(memDC);
			EndPaint(hWnd, &ps);
			return 0;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}