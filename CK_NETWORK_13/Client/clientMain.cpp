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

// ���� ����:
HWND hRootWnd;
HWND hConnectDialog;

TCHAR szWndAppName[] = TEXT("Client");
HINSTANCE g_Instance;

//��ƿ
#define RectToParam(rect) rect.left,rect.top,rect.right,rect.bottom
#define Vector2ToParam(vector) vector.x,vector.y

//������ ����
void FixedUpdate();
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConnectDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR    lpCmdLine, int       nCmdShow)
{
	// 1. ������ Ŭ���� ���� 
	WNDCLASS wc = { 0 };
	wc.style = 0;			   									// ������ Ŭ������ ��Ÿ�� ����
	wc.lpfnWndProc = WndProc;									// �޽��� ó�� �Լ��� ������ ���ν��� ������	
	wc.cbClsExtra = 0;											// ������ Ŭ���� ������ ���� ���� �ʱⰪ 0  
	wc.cbWndExtra = 0;											// ������ ������ ���� ����, �ʱⰪ 0  
	wc.hInstance = hInstance; 									// �ش� ������ Ŭ������ ����� �������α׷��� �ν��Ͻ�
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);					// �����찡 �ּ�ȭ �ɶ� ������ ������ 	
	wc.hCursor = LoadCursor(0, IDC_CROSS);						// �ش� ������ Ŭ������ ������� �������� ���콺 Ŀ�� �ڵ�, IDC_ARROW, IDC_CROSS, IDC_IBEAM, IDC_NO, IDC_WAIT
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		// ������ �۾� ������ ĥ�� ��� �귯�� �ڵ� (���)	
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);										// ����� �޴��� �����Ѵ�. 	
	wc.lpszClassName = szWndAppName;							// ����ϰ��� �ϴ� ������ Ŭ���� �̸� 

	if (!RegisterClass(&wc))
		return -1;

	// 2. ������ ���� 
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

	// 3. ������ ȭ�� ��� 
	ShowWindow(hRootWnd, nCmdShow);

	// 4. �޽��� ���� 

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

			TranslateMessage(&Message);       //WM_KEYDOWN�̰� Ű�� ���� Ű�� �� WM_CHAR �߻�
			DispatchMessage(&Message);        //�ݹ� ���ν����� ������ �� �ְ� ����ġ ��Ŵ
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
		//world.worldOffset += (-bullet.position - world.worldOffset) * 0.02f;
		world.worldOffset = (-bullet.position ) ;

		//x = velocity * cos(��) * t
		//y = velocity * sin(��) * t  - 1 / 2g * t ^ 2

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


// 0.1. ������ ���ν��� 
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