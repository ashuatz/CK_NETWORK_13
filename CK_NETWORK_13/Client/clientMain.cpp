#include "stdafx.h"

#include "network.h"
#include "vector.h"
#include "matrix.h"
#include "Timer.h"
#include "object.h"
#include "packet.h"

#include "resource.h"

#include <Windows.h>
#include <sstream>
#include <string>
#include <iomanip>

#include <Commctrl.h>

#include <algorithm>

int PID;
bool g_bIsActive;
int g_nClientWidth = 640;
int g_nClientHeight = 480;

// 전역 변수:
HWND hRootWnd;
HWND hConnectDialog;

TCHAR szWndAppName[] = TEXT("Client");
HINSTANCE g_Instance;

//유틸
#define RectToParam(rect) rect.left,rect.top,rect.right,rect.bottom
#define Vector2ToParam(vector) vector.x,vector.y

//정방향 선언
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
void FixedUpdate();
const RECT GetRect(const vector2& position, const vector2& size);
const vector2 GetRenderPosition(const vector2& position);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConnectDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


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
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);										// 사용할 메뉴를 지정한다. 	
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
			//WaitMessage();
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
		world.worldOffset += (-bullet.position - world.worldOffset)  * dt;

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

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_GAME_CONNECT:
				{
					if (!IsWindow(hConnectDialog))
					{
						hConnectDialog = (HWND)DialogBox(g_Instance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, ConnectDlgProc);
						//hConnectDialog = CreateDialog(g_Instance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, ConnectDlgProc);
						ShowWindow(hConnectDialog, SW_SHOW);
					}

					break;
				}

				default:
					break;
			}
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

//Connet Window Proc
BOOL CALLBACK ConnectDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			auto ipbox = GetDlgItem(hWnd, IDC_IPADDRESS1);
			LPARAM lpAdr = MAKEIPADDRESS(127, 0, 0, 1);
			SendMessage(ipbox, IPM_SETADDRESS, 0, lpAdr);

			SetDlgItemInt(hWnd, IDC_EDIT1, 3317, 0);

			return TRUE;
		}

		case WM_COMMAND:
			switch (wParam)
			{
				case IDOK:
				{
					auto ipbox = GetDlgItem(hWnd, IDC_IPADDRESS1);
					auto port = GetDlgItemInt(hWnd, IDC_EDIT1, 0, 0);

					DWORD CurAddress;

					LRESULT SM = SendMessage(ipbox, IPM_GETADDRESS, 0, (LPARAM)&CurAddress);

					BYTE IPPart1 = FIRST_IPADDRESS((LPARAM)CurAddress);
					BYTE IPPart2 = SECOND_IPADDRESS((LPARAM)CurAddress);
					BYTE IPPart3 = THIRD_IPADDRESS((LPARAM)CurAddress);
					BYTE IPPart4 = FOURTH_IPADDRESS((LPARAM)CurAddress);
					
					std::string ip = std::string()
						.append(std::to_string((int)IPPart1)).append(".")
						.append(std::to_string((int)IPPart2)).append(".")
						.append(std::to_string((int)IPPart3)).append(".")
						.append(std::to_string((int)IPPart4));

					if (NetworkModule::GetInstance().Initialize(ip.c_str(), port).TryConnect())
					{
						Packet packet;
						packet.opcode = OpCodes::kRequestInitialize;

						NetworkModule::GetInstance().Send(packet.ToString());

						EndDialog(hWnd, 0);
					}

					return TRUE;
				}


				case IDCANCEL:
					EndDialog(hWnd, 0);
					return TRUE;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return false;
}