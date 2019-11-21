#include "network.h"
#include "vector.h"
#include "matrix.h"
#include "Timer.h"

#include <Windows.h>
#include <sstream>
#include <string>
#include <iomanip>

#include <algorithm>

HWND hRootWnd;

TCHAR szWndAppName[] = TEXT("Client");
HINSTANCE g_Instance;

int g_nClientWidth = 640;
int g_nClientHeight = 480;

template<typename T>
T clamp(const T& min, const T& max, const T& value)
{
	return value < min ? min : (value > max ? max : value);
}

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

	//while (GetMessage(&Message, 0, 0, 0))
	//{
	//	TranslateMessage(&Message);
	//	DispatchMessage(&Message);
	//}

	HDC hdc = GetDC(hRootWnd);
	while (true)
	{
		//io
		if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			if (Message.message == WM_QUIT) break;

			TranslateMessage(&Message);       //WM_KEYDOWN이고 키가 문자 키일 때 WM_CHAR 발생
			DispatchMessage(&Message);        //콜백 프로시저가 수행할 수 있게 디스패치 시킴
		}

		//System

		//physics
		FixedUpdate();

		//time
		Time::GetInstance().CheckCounter();

	}

	ReleaseDC(hRootWnd, hdc);

	return 0;
}

//test : temp variable
float maxGravity;

vector2 PlayerPosition;
vector2 PlayerScale;
vector2 PlayerSize;

bool isBulletAlive;
float bulletGravityTime;
float bulletPower;
vector2 bulletmoveDirection;

vector2 bulletPosition;
vector2 bulletScale;
vector2 bulletSize;

vector2 defaultWorldOffset;
vector2 worldOffset;
constexpr float Gravity = -9;

constexpr float targetRenderTime = 0.016;
float renderTime = 0;

void FixedUpdate()
{
	auto dt = Time::GetInstance().GetDeltaTime() * 0.01; // 0.001f
	renderTime += dt;

	if (isBulletAlive)
	{
		bulletPosition += (bulletmoveDirection * bulletPower + vector2(0, Gravity * bulletGravityTime < maxGravity ? Gravity * bulletGravityTime : maxGravity)) * dt;

		bulletGravityTime += dt;

		//smoothDamp
		//camera follow bulletPosition
		worldOffset += (-bulletPosition - worldOffset) * 0.005f;

		//x = velocity * cos(θ) * t
		//y = velocity * sin(θ) * t  - 1 / 2g * t ^ 2
	}
	else
	{
		worldOffset = defaultWorldOffset;
	}

	if (renderTime > targetRenderTime)
	{
		renderTime -= targetRenderTime;
		InvalidateRect(hRootWnd, 0, 1);

		//debug : logging deltaTime , frameRate
		auto titleName = (std::stringstream() << std::setw(4) << std::setfill(_T(' ')) << Time::GetInstance().GetFrame()).str()
			.append(_T(" Frame    "))
			.append((std::stringstream() << std::setw(8) << std::setfill(_T(' ')) << std::setprecision(5) << (Time::GetInstance().GetDeltaTime())).str())
			.append(_T(" ms"));

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
		g_nClientWidth * 0.5f + position.x + worldOffset.x,
		g_nClientHeight * 0.5f - (position.y + worldOffset.y));
}

#define RectToParam(rect) rect.left,rect.top,rect.right,rect.bottom
#define Vector2ToParam(vector) vector.x,vector.y

// 0.1. 윈도우 프로시저 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD dwRead;

	switch (message)
	{
		case WM_CREATE:
		{
			PlayerPosition = vector2(0, 0);
			PlayerScale = vector2(1, 1);
			PlayerSize = vector2(20, 20);

			bulletPower = 20;
			bulletmoveDirection = vector2(1, 1);

			bulletPosition = PlayerPosition;
			bulletScale = vector2(1, 1);
			bulletSize = vector2(5, 5);
			
			defaultWorldOffset = vector2(0, 0);

			isBulletAlive = false;

			return 0;
		}

		case WM_KEYDOWN:
		{
			//send message to server (IO)

			//Game logic MUST be handled by the MessageQueue.

			//camera offset test code

			switch (wParam) 
			{
				case VK_DOWN: defaultWorldOffset += vector2(0, -2); break;
				case VK_UP:defaultWorldOffset += vector2(0, 2); break;

				case VK_LEFT:defaultWorldOffset += vector2(2, 0); break;
				case VK_RIGHT:defaultWorldOffset += vector2(-2, 0); break;

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
			isBulletAlive = true;


			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			//test : bullet initialize Code

			isBulletAlive = false;
			bulletGravityTime = 0;

			bulletPosition = PlayerPosition;
			bulletScale = vector2(1,1);
			bulletSize = vector2(5,5);

			return 0;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			vector2 vector = vector2(1, 1);

			//test : render

			//horizon
			MoveToEx(hdc, Vector2ToParam(GetRenderPosition(vector2(-INT16_MAX, 0))), 0);
			LineTo(hdc, Vector2ToParam(GetRenderPosition(vector2(INT16_MAX, 0))));

			//player
			Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(PlayerPosition), PlayerSize)));

			//bullet
			if (isBulletAlive)
			{
				Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(bulletPosition), bulletSize)));
			}

			EndPaint(hWnd, &ps);
			return 0;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}