#pragma once
#include "singleton.h"

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

#define RectToParam(rect) rect.left,rect.top,rect.right,rect.bottom
#define Vector2ToParam(vector) vector.x,vector.y

class WindowClient : public Singleton<WindowClient>
{
	HWND hRootWnd;

	static constexpr TCHAR szWndAppName[] = TEXT("Client");
	HINSTANCE g_Instance;

	int g_nClientWidth = 640;
	int g_nClientHeight = 480;

	int DefineWindowClass(HINSTANCE hInstance)
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

	}


	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{

	}
};