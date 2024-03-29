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

//정방향 선언
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

//GameLoop
void Awake();
void MessageLoop();
void Update();
void FixedUpdate();

//Input / Game Event
void OnMouseDown(int id);
void OnMouseUp(int id);
void OnKeyboardInput(HWND hWnd, WPARAM wParam, bool isDown);
void Render(HWND hwnd, HDC hdc);

//procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConnectDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//converters
#define RectToParam(rect) rect.left,rect.top,rect.right,rect.bottom
#define Vector2ToParam(vector) vector.x,vector.y
const RECT GetRect(const vector2& position, const vector2& size);
const vector2 GetRenderPosition(const vector2& position);
const vector2 GetViewPortPosition(const vector2& position);
const vector2 ScreenToWorldPosition(const vector2& position);
const COLORREF ToColor(const vector3Int& color32);
const COLORREF ToColor(const vector3& color);
const bool aabb(RECT a, RECT b);

#pragma region Math

template<typename T>
const T abs(const T& rhs)
{
	return rhs > 0 ? rhs : -rhs;
}

template<typename T>
const int sign(const T& val)
{
	return (T(0) < val) - (val < T(0));
}

static float lerp(const float& a, const float& b, const float& f)
{
	return a + f * (b - a);
}

static float clamp(const float& min, const float& max, const float& value)
{
	return value < min ? min : (value > max ? max : value);
}

#pragma endregion

#pragma region Windows

bool g_bIsActive;
int g_nClientWidth = 640;
int g_nClientHeight = 480;

// 전역 변수:
HWND hRootWnd;
HWND hConnectDialog;

TCHAR szWndAppName[] = TEXT("Client");
HINSTANCE g_Instance;

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
		g_nClientWidth - 1, 					            //Width
		g_nClientHeight - 1, 								//Height
		NULL,							    //hWndParent
		NULL,							    //hMenu
		hInstance,						    //hInstance
		NULL);	     					    //lpParam

	g_Instance = hInstance;

	// 3. 윈도우 화면 출력 
	ShowWindow(hRootWnd, nCmdShow);

	// 4. 메시지 루프 
	MessageLoop();

	return 0;
}

#pragma endregion

#pragma region GameLoop

bool isEnd;

//move
Player* mover;
bool bAllowMove;
int moveDir;
float allowedMoveTime;
constexpr float moveSpeed = 5;
constexpr float maxMoveTime = 3;

//turn
bool isMyTurn;
float turnTime = 0;

//fire
bool isPressed;
float PressTime = 0;
constexpr float maxPressTime = 1.5f;

//camera movement
float screenFreezeTime;

//connection
bool StartConnection;
bool isConnected;

//entities
Player me;
Player other;

Bullet bullet;

World world;

vector2 mousePosition;

constexpr float targetRenderTime = 0.016;
float renderTime = 0;

void MessageLoop()
{
	MSG Message;
	while (true)
	{
		//System
		Update();

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
}

//client initialize
void Awake()
{
	StartConnection = false;
	isConnected = false;

	isEnd = false;
	//test world input
	isMyTurn = true;

	me.PlayerPosition = vector2(0, 0);
	me.PlayerSize = vector2(20, 20);

	bullet.Init(vector2(1, 1), &me);

	world.defaultWorldOffset = me.PlayerPosition;
}

//client update
void Update()
{
	//connection stat check
	if (StartConnection)
	{
		StartConnection = false;

		//init
		auto init_message = NetworkModule::GetInstance().SyncDequeueMessage();
		Packet received = Packet::ToPacket(init_message);
		me = received.response.Initialize_message.player_info;
		other = received.response.Initialize_message.other_info;

		isMyTurn = received.response.Initialize_message.isFirst;
		turnTime = received.response.Initialize_message.default_turn_time;

		world.defaultWorldOffset = isMyTurn ? -me.PlayerPosition : -other.PlayerPosition;

		if (isMyTurn)
		{
			bAllowMove = true;
			allowedMoveTime = maxMoveTime;
		}
	}

	//message control
	auto message = NetworkModule::GetInstance().dequeueMessage();
	if (message.length() > 0)
	{
		Packet packet = Packet::ToPacket(message);
		if (packet.error_code == ErrorCodes::kOK)
		{
			switch (packet.opcode)
			{
				case OpCodes::kFire:
				{
					Player* shotter = me.pid == packet.response.fire_message.pid ? &me : &other;
					bullet.power = packet.response.fire_message.power;
					bullet.Init(packet.response.fire_message.target_position, shotter);
					bullet.isAlive = true;

					break;
				}

				case OpCodes::kResponseTurnEnd:
				{
					isMyTurn = packet.response.turn_over_message.current_pid == me.pid;
					world.defaultWorldOffset = isMyTurn ? -me.PlayerPosition : -other.PlayerPosition;

					if (isMyTurn)
					{
						bAllowMove = true;
						allowedMoveTime = maxMoveTime;
					}

					break;
				}

				case OpCodes::kResponseHit:
				{
					Player& damaged = me.pid == packet.response.fire_message.pid ? me : other;
					damaged.HP -= packet.response.hit_message.amount;

					if (damaged.HP < 0)
						damaged.HP = 0;


					break;
				}

				case OpCodes::kGameEnd:
				{
					if (packet.response.game_end_message.error)
					{
						if (MessageBox(hRootWnd, _T("상대와의 연결이 끊어졌습니다."), _T("Game End"), MB_OK) == IDOK)
						{
							NetworkModule::GetInstance().CleanUp();
							Awake();
						}

						break;
					}

					if (packet.response.game_end_message.pid == me.pid)
					{
						// lose
						if (MessageBox(hRootWnd, _T("패배하셨습니다"), _T("Game End"), MB_OK) == IDOK)
						{
							NetworkModule::GetInstance().CleanUp();
							Awake();
						}
					}
					else
					{
						// win
						if (MessageBox(hRootWnd, _T("승리하셨습니다"), _T("Game End"), MB_OK) == IDOK)
						{
							NetworkModule::GetInstance().CleanUp();
							Awake();
						}
					}

					break;
				}

				case OpCodes::kMove:
				{
					mover = me.pid == packet.response.move_message.pid ? &me : &other;
					mover->PlayerPosition = packet.response.move_message.position;
					//stop
					if (packet.response.move_message.isKeyDown == false)
					{
						moveDir = 0;
						bAllowMove = false;

						mover->PlayerPosition = packet.response.move_message.position;
					}
					else if (packet.response.move_message.KeyCode == VK_RIGHT)
					{
						moveDir = 1;
					}
					else if (packet.response.move_message.KeyCode == VK_LEFT)
					{
						moveDir = -1;
					}

					break;
				}

				default:
					break;
			}

		}
	}
}

//client physics update
void FixedUpdate()
{
	auto usdt = Time::GetInstance().GetDeltaTime() * 0.001f; // unscaled delta time
	auto sdt = usdt * 10; //scaled deltaTime
	renderTime += usdt;
	turnTime -= usdt;

	if (isPressed)
	{
		PressTime += usdt;
		PressTime = PressTime > maxPressTime ? maxPressTime : PressTime;
	}

	if (moveDir != 0)
	{
		mover->PlayerPosition += vector2(moveDir * moveSpeed * sdt, 0);
		allowedMoveTime -= usdt;

		world.defaultWorldOffset = -mover->PlayerPosition;

		if (allowedMoveTime < 0)
		{
			moveDir = 0;
			allowedMoveTime = 0;
			bAllowMove = false;
		}
	}

	//HP
	if ((int)(me.displayHP * 0.05f) != (int)(me.HP * 0.05f))
	{
		me.displayHP += (me.HP - me.displayHP) * usdt * 3;
	}
	else
	{
		me.lastDisplayHp = me.displayHP = me.HP;

		if (me.HP == 0 && !isEnd)
		{
			isEnd = true;

			GameEndMessage game_end_message;
			game_end_message.error = false;
			game_end_message.pid = me.pid;

			Packet end_packet;
			end_packet.opcode = OpCodes::kGameEnd;
			end_packet.request.game_end_message;

			NetworkModule::GetInstance().Send(end_packet.ToString());
		}
	}
	if ((int)(other.displayHP * 0.05f) != (int)(other.HP * 0.05f))
	{
		other.displayHP += (other.HP - other.displayHP) * usdt * 3;
	}
	else
	{
		other.lastDisplayHp = other.displayHP = other.HP;
	}

	if (bullet.isAlive)
	{
		//movement 
		bullet.position += (bullet.moveDirection * bullet.power + vector2(0, world.GetMaxGravity(bullet.gravityTime))) * sdt;

		//time for gravity
		bullet.gravityTime += sdt;

		//smoothDamp

		//camera follow bulletPosition
		world.worldOffset += (-bullet.position - world.worldOffset)  * sdt;

		if (isConnected && bullet.owner->pid == other.pid && aabb(GetRect(bullet.position, bullet.size), GetRect(me.PlayerPosition, me.PlayerSize)))
		{
			bullet.isAlive = false;
			screenFreezeTime = 2.4f;
		}

		if (isConnected && bullet.owner->pid == me.pid && aabb(GetRect(bullet.position, bullet.size), GetRect(other.PlayerPosition, other.PlayerSize)))
		{
			bullet.isAlive = false;
			screenFreezeTime = 2.4f;

			//hit
			HitMessage hit_message;
			hit_message.pid = other.pid;
			hit_message.position = (bullet.position + other.PlayerPosition) * 0.5f;
			hit_message.amount = bullet.power * bullet.gravityTime * 1.5f;

			Packet hit_packet;
			hit_packet.opcode = OpCodes::kRequestHit;
			hit_packet.request.hit_message = hit_message;

			NetworkModule::GetInstance().Send(hit_packet.ToString());

			//turn end
			TurnOverMessage turn_over_message;
			turn_over_message.last_pid = me.pid;

			Packet end_packet;
			end_packet.opcode = OpCodes::kRequestTurnEnd;
			end_packet.request.turn_over_message = turn_over_message;

			NetworkModule::GetInstance().Send(end_packet.ToString());
		}

		else if (bullet.position.y < 0)
		{
			//release bullet
			bullet.isAlive = false;
			screenFreezeTime = 0.7f;

			//if myshoot
			if (bullet.owner->pid == me.pid)
			{
				TurnOverMessage turn_over_message;
				turn_over_message.last_pid = me.pid;

				Packet packet;
				packet.opcode = OpCodes::kRequestTurnEnd;
				packet.request.turn_over_message = turn_over_message;

				NetworkModule::GetInstance().Send(packet.ToString());
			}
		}
	}
	else if (screenFreezeTime > 0)
	{
		//continue move
		world.worldOffset += (-bullet.position - world.worldOffset)  * sdt;

		//reduce time
		screenFreezeTime -= usdt;
	}
	else
	{
		//move to default offset
		world.worldOffset += (world.defaultWorldOffset - world.worldOffset)  * sdt;
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
			.append((std::stringstream() << std::setw(8) << std::setfill(_T(' ')) << std::setprecision(5) << (Time::GetInstance().GetMaxDeltaTime())).str());

		SetWindowText(hRootWnd, titleName.c_str());
	}
}

//id : 0(L) 2(Wheel) 1(R)
void OnMouseDown(int id)
{
	if (!isMyTurn)
		return;

	if (moveDir != 0)
		return;

	switch (id)
	{
		case 0: //Lbutton Down
		{
			PressTime = 0;
			isPressed = true;

			break;
		}

		case 1: // Rbutton Down
		{

			break;
		}
	}

}

//id : 0(L) 2(Wheel) 1(R)
void OnMouseUp(int id)
{
	if (!isMyTurn)
		return;

	if (moveDir != 0)
		return;

	switch (id)
	{
		case 0: //Lbutton Up
		{
			if (bullet.isAlive)
				break;

			float pressRate = (PressTime > 2 ? 2 : PressTime) / maxPressTime;
			float power = 30 + (120 * pressRate);

			//testworld code
			if (!isConnected)
			{
				bullet.Init(mousePosition, &me);
				bullet.power = power;
				bullet.isAlive = true;

				isPressed = false;
				break;
			}

			FireMessage fire_message;
			fire_message.pid = me.pid;
			fire_message.position = me.PlayerPosition;
			fire_message.target_position = mousePosition;
			fire_message.power = power;

			Packet packet;
			packet.opcode = OpCodes::kFire;
			packet.request.fire_message = fire_message;

			NetworkModule::GetInstance().Send(packet.ToString());

			isPressed = false;

			break;
		}

		case 1: // Rbutton UP
		{
			break;
		}
	}
}

//call by WM
void OnKeyboardInput(HWND hWnd, WPARAM wParam, bool isDown)
{
	if (!isMyTurn)
		return;

	if (!bAllowMove)
		return;

	if (isPressed)
		return;

	//send message to server (IO)

	//Game logic MUST be handled by the MessageQueue.

	//camera offset test code

	switch (wParam)
	{
		case VK_SPACE:
		{
			break;
		}

		case VK_LEFT:
		{
			MoveMessage move_message;
			move_message.KeyCode = VK_LEFT;
			move_message.isKeyDown = isDown;
			move_message.pid = me.pid;
			move_message.position = me.PlayerPosition;

			Packet packet;
			packet.opcode = OpCodes::kMove;
			packet.request.move_message = move_message;

			NetworkModule::GetInstance().Send(packet.ToString());
			break;
		}

		case VK_RIGHT:
		{
			MoveMessage move_message;
			move_message.KeyCode = VK_RIGHT;
			move_message.isKeyDown = isDown;
			move_message.pid = me.pid;
			move_message.position = me.PlayerPosition;

			Packet packet;
			packet.opcode = OpCodes::kMove;
			packet.request.move_message = move_message;

			NetworkModule::GetInstance().Send(packet.ToString());
			break;
		}

		default:
			break;
	}

	InvalidateRect(hWnd, 0, 1);

	//offset test code end
}

//call by WM
void Render(HWND hwnd, HDC hdc)
{
	//horizon
	MoveToEx(hdc, Vector2ToParam(GetRenderPosition(vector2(-INT16_MAX, 0))), 0);
	LineTo(hdc, Vector2ToParam(GetRenderPosition(vector2(INT16_MAX, 0))));

	for (int i = -2000; i <= 2000; i += 100)
	{
		MoveToEx(hdc, Vector2ToParam(GetRenderPosition(vector2(i, 0))), 0);
		LineTo(hdc, Vector2ToParam(GetRenderPosition(vector2(i, 1000))));
	}

	//aim line
	if (isMyTurn && !bullet.isAlive)
	{
		auto myRenderPos = GetRenderPosition(me.PlayerPosition);
		auto mouseRenderPos = GetRenderPosition(mousePosition);

		MoveToEx(hdc, Vector2ToParam(myRenderPos), 0);
		LineTo(hdc, Vector2ToParam(((mouseRenderPos - myRenderPos).Normalized() * 1000 + myRenderPos)));
	}

	//player
	HBRUSH brush = CreateSolidBrush(ToColor(me.color32));
	HBRUSH last = (HBRUSH)SelectObject(hdc, brush);
	Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(me.PlayerPosition), me.PlayerSize)));
	SelectObject(hdc, last);
	DeleteObject(brush);

	//other
	brush = CreateSolidBrush(ToColor(other.color32));
	last = (HBRUSH)SelectObject(hdc, brush);
	Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(other.PlayerPosition), other.PlayerSize)));
	SelectObject(hdc, last);
	DeleteObject(brush);

	//bullet
	if (bullet.isAlive)
	{
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(bullet.position), bullet.size)));
	}

	//ui

	//status
	if (!isConnected)
	{
		SetTextAlign(hdc, TA_CENTER);
		std::string turnInfo("Test world");
		TextOut(hdc, g_nClientWidth * 0.5, 10, turnInfo.c_str(), turnInfo.length());

		SetTextAlign(hdc, TA_LEFT);
		std::string howToConnect("↑Game → Connect to Start");
		TextOut(hdc, 10, 10, howToConnect.c_str(), howToConnect.length());
	}
	else if (isMyTurn)
	{
		SetTextAlign(hdc, TA_CENTER);
		std::string turnInfo("My turn");
		TextOut(hdc, g_nClientWidth * 0.5, 10, turnInfo.c_str(), turnInfo.length());
	}
	else
	{
		SetTextAlign(hdc, TA_CENTER);
		std::string turnInfo("waiting..");
		TextOut(hdc, g_nClientWidth * 0.5, 10, turnInfo.c_str(), turnInfo.length());
	}
	
	//moving
	if (moveDir != 0)
	{
		vector2 barSize(60, 6);
		float rate = allowedMoveTime / maxMoveTime;

		brush = CreateSolidBrush(ToColor(vector3Int(54, 34, 22)));
		last = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(me.PlayerPosition - vector2(0, me.PlayerSize.y * 0.75f) - vector2(0, 8)), barSize + vector2Int(1, 1))));
		SelectObject(hdc, last);
		DeleteObject(brush);

		//HPBar
		brush = CreateSolidBrush(ToColor(vector3Int(185, 122, 87)));
		last = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(me.PlayerPosition + vector2(-(barSize.x * (1 - rate) * 0.5f), -me.PlayerSize.y * 0.75f) - vector2(0, 8)), vector2(barSize.x *rate, barSize.y))));
		SelectObject(hdc, last);
		DeleteObject(brush);
	}

	//Charging
	if (isPressed)
	{
		vector2 barSize(60, 6);
		float rate = PressTime / maxPressTime;

		brush = CreateSolidBrush(ToColor(vector3Int(0, 3, 38)));
		last = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(me.PlayerPosition + vector2(0, me.PlayerSize.y * 0.75f) + vector2(0, 8)), barSize + vector2Int(1, 1))));
		SelectObject(hdc, last);
		DeleteObject(brush);

		//HPBar
		brush = CreateSolidBrush(ToColor(vector3Int(123, 133, 255)));
		last = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(me.PlayerPosition + vector2(-(barSize.x * (1 - rate) * 0.5f), me.PlayerSize.y * 0.75f) + vector2(0, 8)), vector2(barSize.x *rate, barSize.y))));
		SelectObject(hdc, last);
		DeleteObject(brush);
	}

	//playerHP
	if ((int)(me.displayHP * 0.05f) != (int)(me.HP * 0.05f))
	{
		vector2 barSize(60, 6);
		float rate = me.displayHP * 0.0001f;
		float lastRate = me.lastDisplayHp * 0.0001f;

		//BackGround
		brush = CreateSolidBrush(ToColor(vector3Int(51, 0, 0)));
		last = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(me.PlayerPosition + vector2(0, me.PlayerSize.y * 0.75f)), barSize + vector2Int(1, 1))));
		SelectObject(hdc, last);
		DeleteObject(brush);

		//LastHPBar
		brush = CreateSolidBrush(ToColor(vector3Int(255, 244, 34)));
		last = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(me.PlayerPosition + vector2(-(barSize.x * (1 - lastRate) * 0.5f), me.PlayerSize.y * 0.75f)), vector2(barSize.x *lastRate, barSize.y))));
		SelectObject(hdc, last);
		DeleteObject(brush);

		//HPBar
		brush = CreateSolidBrush(ToColor(vector3Int(251, 58, 50)));
		last = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(me.PlayerPosition + vector2(-(barSize.x * (1 - rate) * 0.5f), me.PlayerSize.y * 0.75f)), vector2(barSize.x *rate, barSize.y))));
		SelectObject(hdc, last);
		DeleteObject(brush);

		//me.displayHP += (me.HP - me.displayHP) * Time::GetInstance().GetDeltaTime() * 0.001f;
	}
	if ((int)(other.displayHP * 0.05f) != (int)(other.HP * 0.05f))
	{
		vector2 barSize(60, 6);
		float rate = other.displayHP * 0.0001f;
		float lastRate = other.lastDisplayHp * 0.0001f;

		//BackGround
		brush = CreateSolidBrush(ToColor(vector3Int(51, 0, 0)));
		last = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(other.PlayerPosition + vector2(0, other.PlayerSize.y * 0.75f)), barSize + vector2Int(1, 1))));
		SelectObject(hdc, last);
		DeleteObject(brush);

		//LastHPBar
		brush = CreateSolidBrush(ToColor(vector3Int(255, 244, 34)));
		last = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(other.PlayerPosition + vector2(-(barSize.x * (1 - lastRate) * 0.5f), other.PlayerSize.y * 0.75f)), vector2(barSize.x *lastRate, barSize.y))));
		SelectObject(hdc, last);
		DeleteObject(brush);

		//HPBar
		brush = CreateSolidBrush(ToColor(vector3Int(251, 58, 50)));
		last = (HBRUSH)SelectObject(hdc, brush);
		Rectangle(hdc, RectToParam(GetRect(GetRenderPosition(other.PlayerPosition + vector2(-(barSize.x * (1 - rate) * 0.5f), other.PlayerSize.y * 0.75f)), vector2(barSize.x *rate, barSize.y))));
		SelectObject(hdc, last);
		DeleteObject(brush);

		//other.displayHP += (other.HP - other.displayHP) * Time::GetInstance().GetDeltaTime() * 0.001f;
	}

	//Player tracking ui
	vector2 otherScreenPosition = GetRenderPosition(other.PlayerPosition);

	vector2 dir = otherScreenPosition - GetViewPortPosition(vector2());
	vector2 screenSize(g_nClientWidth * 0.5f - 50, g_nClientHeight * 0.5f - 120);

	if (abs(dir.y) > screenSize.y || abs(dir.x) > screenSize.x)
	{
		float slope = dir.y / dir.x;


		dir.y = clamp(-screenSize.y, screenSize.y, dir.y);
		dir.x = slope == 0 ? dir.x : (dir.y / slope);
		if (abs(dir.x) > screenSize.x)
		{
			dir.x = clamp(-screenSize.x, screenSize.x, dir.x);
			dir.y = slope * dir.x;
		}

		brush = CreateSolidBrush(ToColor(other.color32));
		last = (HBRUSH)SelectObject(hdc, brush);
		Ellipse(hdc, RectToParam(GetRect((GetViewPortPosition(vector2()) + dir), other.PlayerSize * 0.5f)));
		SelectObject(hdc, last);
		DeleteObject(brush);

		//MoveToEx(hdc, Vector2ToParam(GetViewPortPosition( vector2())), 0);
		//LineTo(hdc, Vector2ToParam((GetViewPortPosition(vector2()) + dir)));
	}



	//test
	//std::string tt(std::to_string(turnTime));
	//std::string mp(std::to_string(mousePosition.x).append(",").append(std::to_string(mousePosition.y)));
	//std::string wd(std::string("world offset :").append(std::to_string(world.worldOffset.x).append(",").append(std::to_string(world.worldOffset.y))));
	//std::string pps(std::string("dir :").append(std::to_string(dir.x).append(",").append(std::to_string(dir.y))));
	//std::string vp(std::string("viewPort :").append(std::to_string(GetViewPortPosition(vector2()).x).append(",").append(std::to_string(GetViewPortPosition(vector2()).y))));

	//TextOut(hdc, g_nClientWidth * 0.7, 20, tt.c_str(), tt.length());
	//TextOut(hdc, g_nClientWidth * 0.7, 40, mp.c_str(), mp.length());
	//TextOut(hdc, g_nClientWidth * 0.7, 60, wd.c_str(), wd.length());
	//TextOut(hdc, g_nClientWidth * 0.7, 80, vp.c_str(), vp.length());
	//TextOut(hdc, g_nClientWidth * 0.7, 120, pps.c_str(), pps.length());
}

#pragma endregion

#pragma region Procedure

// 0.1. 윈도우 프로시저 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD dwRead;

	switch (message)
	{
		case WM_CREATE:
		{
			Awake();
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
			OnKeyboardInput(hWnd, wParam, true);

			return 0;
		}

		case WM_KEYUP:
		{
			OnKeyboardInput(hWnd, wParam, false);

			return 0;
		}

		case WM_MOUSEMOVE:
		{
			mousePosition = ScreenToWorldPosition(vector2(LOWORD(lParam), HIWORD(lParam)));

			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			mousePosition = ScreenToWorldPosition(vector2(LOWORD(lParam), HIWORD(lParam)));
			OnMouseDown(0);

			return 0;
		}

		case WM_LBUTTONUP:
		{
			mousePosition = ScreenToWorldPosition(vector2(LOWORD(lParam), HIWORD(lParam)));
			OnMouseUp(0);

			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			mousePosition = ScreenToWorldPosition(vector2(LOWORD(lParam), HIWORD(lParam)));
			OnMouseDown(1);

			return 0;
		}

		case WM_RBUTTONUP:
		{
			mousePosition = ScreenToWorldPosition(vector2(LOWORD(lParam), HIWORD(lParam)));
			OnMouseUp(1);

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

			Render(hWnd, memDC);

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

						StartConnection = true;
						isConnected = true;
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

#pragma endregion


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

const vector2 GetViewPortPosition(const vector2& position)
{
	return vector2(
		g_nClientWidth * 0.5f + position.x,
		g_nClientHeight * 0.5f - position.y);
}

const vector2 ScreenToWorldPosition(const vector2& position)
{
	vector2 worldPosition;

	auto renderOffset = GetRenderPosition(vector2());
	worldPosition.x = position.x - renderOffset.x;
	worldPosition.y = -(position.y - renderOffset.y);

	return worldPosition;
}

const COLORREF ToColor(const vector3Int& color32)
{
	return RGB(color32.x, color32.y, color32.z);
}

const COLORREF ToColor(const vector3& color)
{
	return ToColor(vector3Int(color * 255));
}

const bool aabb(RECT a, RECT b)
{
	return a.left < b.right && a.right > b.left &&
		a.top < b.bottom && a.bottom > b.top;
}