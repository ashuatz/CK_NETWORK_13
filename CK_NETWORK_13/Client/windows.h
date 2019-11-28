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
		wc.lpszMenuName = NULL;										// ����� �޴��� �����Ѵ�. 	
		wc.lpszClassName = szWndAppName;							// ����ϰ��� �ϴ� ������ Ŭ���� �̸� 

		if (!RegisterClass(&wc))
			return -1;

	}


	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{

	}
};