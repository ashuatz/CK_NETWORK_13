
/*
13주차
	파일입출력 :

	훅 :
	SendMessageCallback(...);

	HHOOK hKeyHook = SetWindowsHookEx(WH_KEYBOARD, KeyHookProc, 0, GetCurrentThreadId());
	UnhookWindowsHookEx(hKeyHook);
		WindowHook_<EVENT_NAME> => WH_EVENT


	메시지 루프 :
	while (true)
	{
		if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			//메시지 처리 루프
			if (Message.message == WM_QUIT) break;

			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			//일반 업데이트 처리 루프
			...
		}
	}


12주차
	테트리스

11주차
대화상자
	Modal		: DialogBox		: 전환 불가
	Modeless	: CreateDialog	: 전환 가능
		GetDlgItemText(hWnd, IDC_STR, str, 128);
		SetDlgItemText(hWnd, IDC_STR, str);

		x = GetDlgItemInt(hWnd, IDC_X, 0, 0);
		SetDlgItemInt(hWnd, IDC_X, x, 0);

		EndDialog(hWnd, IDOK);
			=>result of "DialogBox(g_Instance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AboutDlgProc)"

공통 대화 상자
	파일열기 폰트선택 색상선택 인쇄 찾기 등
	BOOL GetOpenFileName( LPOPENFILENAME lpofn );

	파일열기
		OPENFILENAME ofn;
		TCHAR str[256];
		TCHAR strFile[512] = TEXT("");

		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFilter = TEXT("Every File(*.*)\0*.*\0Text File(.txt)\0*.txt\0");
		ofn.lpstrFile = strFile;
		ofn.nMaxFile = 256;
		ofn.lpstrInitialDir = TEXT("c:\\");
		if (GetOpenFileName(&ofn) != 0)
		{
			wsprintf(str, TEXT("%s 파일을 선택했습니다."), ofn.lpstrFile);
			MessageBox(hWnd, str, TEXT("파일 열기 성공"), MB_OK);
		}

	색상
		static COLORREF color = RGB(0, 0, 225);
		static CHOOSECOLOR c;
		static COLORREF colorTemp[16];
		memset(&c, 0, sizeof(CHOOSECOLOR));
		c.lStructSize = sizeof(CHOOSECOLOR);
		c.hwndOwner = hWnd;
		c.lpCustColors = colorTemp;
		c.Flags = 0;

		if (ChooseColor(&c) != 0)
		{
			color = c.rgbResult;
			InvalidateRect(hWnd, 0, 1);
		}
		~~~
		~~~
		MyBrush = CreateSolidBrush(color);
		OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);

		Rectangle(hdc, 10, 10, 300, 200);
		SelectObject(hdc, OldBrush);
		DeleteObject(MyBrush);


*/


/*
10주차
+PostMessage(MessageQueue) / SendMessage(Direct)
+LOWORD / HIWORD <>MakeLong(hi,lo)

에디트
	hEdit_1 = CreateWindow(_T("edit"), 0, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 10, 256, 32, hWnd, (HMENU)ID_EDIT_1, g_Instance, 0);
	WM_COMMAND:
		LOWORD(wParam) is EDIT ID (HMENU's value)
		HIWORD(wParam) is Edit Notify Message (EN_CHANGE, etc.)

		case EN_CHANGE:
			GetWindowText(hEdit_1, str_edit_1, 128);

리스트박스
	hList = CreateWindow(_T("listBox"), 0, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY,10, 10, 100, 200, hWnd, (HMENU)ID_LISTBOX, g_Instance, 0);
		SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)<string>);
	WM_COMMAND:
		LOWORD(wParam) is EDIT ID (HMENU's value)
		HIWORD(wParam) is List Box Notify Message (LBN_CHANGE, etc.)

		case CBN_SELCHANGE:
			int idx = SendMessage(hList, CB_GETCURSEL, 0, 0);
				//get index
			SendMessage(hList, CB_GETLBTEXT, idx, (LPARAM)str);
				//get string of index

	LB/LBN/LBS(List Box Style,Notify)
콤보박스
	hCombo= CreateWindow(_T("ComboBox"), 0, WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN, 10, 10, 100, 200, hWnd, (HMENU)ID_COMBOBOX, g_Instance, 0);

	//리스트박스 + 에디트와 거의 동일

스크롤바
	SetScrollRange(hWNd,ID, --- ---- -- --- --
	WM_<axis>SCROLL:
		LOWORD(wParam) : clicked position
		HIWORD(wParam) : nowPosition
		lParam : handle of Scrollbar

		SB_CTL : ScrollBar_ConTroL
	hRed = CreateWindow(_T("ScrollBar"), 0, WS_CHILD | WS_VISIBLE | SBS_HORZ, 10, 10, 200, 20, hWnd, (HMENU)ID_SCRRED, g_Instance, 0);
	SetScrollRange(hRed, SB_CTL, 0, 255, 1);
	SetScrollPos(hBlue, SB_CTL, 0, 1);

스태틱

9주차
부모윈도우 / 자식윈도우 : 자식에서 부모를 설정. unity와 유사
	WM_CREATE
컨트롤 : 명령을 받아 처리하는 하나의 윈도우 & 인터페이스
	버튼/텍스트/스크롤바/리스트박스 etc.
	CreateWindow의 menu(Control ID) => WM_COMMAND의 LOWORD(wParam)
		해당Window의 Notification => WM_COMMAND의 HIWORD(wParam)
		해당Window의 윈도우 핸들=> WM_COMMAND의 lParam에 들어감.
	Style
		[B]utton[S]tyle_<Type>

	switch (message)
	{
		case WM_CREATE:
		{
			CreateWindow(_T("Button"), _T("ClickMe"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 20, 100, 25, hWnd, 0, g_Instance, 0);
			CreateWindow(_T("Button"), _T("Me Too"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 50, 100, 25, hWnd, (HMENU)1, g_Instance, 0);
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case 0:
					MessageBox(hWnd, _T("First Button Clicked"), _T("Button"), MB_OK);
					break;
				case 1:
					MessageBox(hWnd, _T("Second Button Clicked"), _T("Button"), MB_OK);
					break;
				default:
					break;
			}
			return 0;
		}
	}

	//Create
	c1 = CreateWindow(_T("Button"), _T("Draw Ellipse?"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 20, 20, 160, 25, hWnd, 0, g_Instance, 0);
	//Proc->WM_COMMAND
	if (SendMessage(c1, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
		SendMessage(c1, BM_SETCHECK, BST_CHECKED, 0);

	//r[0] = CreateWindow(_T("Button"), names[0], WS_CHILD | WS_VISIBLE | WS_GROUP, 10 , 20 , 100, 30, hWnd, (HMENU)ID_R1, g_Instance, 0);
	CheckRadioButton(hWnd, ID_R1, ID_R3, ID_R1);

8주차
*중간고사

7주차
*복습


6주차
//다음주까지 수업하고 시험
그리기모드 : ROP : Rasterize operation (래스터연산)
	SetROP2(hdc,R2_<TYPE>)

맵핑모드 : MapingMode - MM_<Type>
	SetMapMode(hdc,fnMapMode)
	GetMapMode(hdc)

	논리좌표 : DC핸들을 인수로 사용
	물리좌표 : 출력되는 좌표. 픽셀단위

뷰포트 :
	윈도우 : 논리좌표 사용 영역
	뷰포트 : 물리좌표가 사용되는 영역

	Set<Viewport/Window>OrgEx(hdc,x,y,*Point last)

비트맵:
	Memdc = CreateCompatibleDC
	myBit = LoadBitmap(g_Instance, MAKEINTRESOURCE(IDB_BITMAP2));
	oldBit = (HBITMAP)SelectObject(MemDC, myBit);

			--<Render>--
			BitBlt / StretchBlt
			--<Render>--

	SelectObject(MemDC, oldBit);
	DeleteObject(myBit);
	DeleteDC(MemDC);

	//동일영역 복사, 좌표만 다름  | 1픽셀당 1픽셀
	BOOL BitBlt( HDC hdcDest, // 복사 대상 DC
		int nXDest,			// 복사 대상 X
		int nYDest,			// 복사 대상 Y
		int nWidth,			// 복사 대상 Width
		int nHeight,		// 복사 대상 Height
		HDC hdcSrc,			// 복사 원본 DC
		int nXSrc,			// 복사 원본 X
		int nYSrc,			// 복사 원본 Y
		DWORD dwRop );		// 래스터 연산 방법

	//영역,좌표 둘다 다르게 가져올 수 있음 | 1픽셀당 N픽셀
	BOOL StretchBlt( HDC hdcDest, // 복사 대상 DC
		int nXOriginDest,	// 복사 대상 X
		int nYOriginDest,	// 복사 대상 Y
		int nWidthDest,		// 복사 대상 Width
		int nHeightDest,	// 복사 대상 Height
		HDC hdcSrc,			// 복사 원본 DC
		int nXOriginSrc,	// 복사 원본 X
		int nYOriginSrc,	// 복사 원본 Y
		int nWidthSrc,		// 복사 원본 Width
		int nHeightSrc,		// 복사 원본 Height
		DWORD dwRop );		// 래스터 연산 방법


FONT :
	생성
		HFONT CreateFont(
			int nHeight,
			int nWidth,
			int nEscapement				<전체문자 기울기>,
			int nOrientation			<개별 문자 기울기>,
			int fnWeight 두께,
			DWORD fdwItalic,
			DWORD fdwUnderline,
			DWORD fdwStrikeOut,
			DWORD fdwCharSet <ANSI_CHARSET / OEM_CHARSET>,
			DWORD fdwOutputPrecision,
			DWORD fdwClipPrecision,
			DWORD fdwQuality,
			DWORD fdwPitchAndFamily,
			LPCTSTR lpszFace  );		<글꼴이름>

		HFONT CreateFontIndirect( CONST LOGFONT *lplf); //구조체를 사용한 간접생성.
	색상
		COLORREF SetTextColor( HDC hdc, COLORREF crColor );
		COLORREF SetBkColor( HDC hdc, COLORREF crColor );
		int SetBkMode( HDC hdc, int iBkMode );
*/

/*							5주차
	*과제에 대해 배운것을 사용하지 아니하면 감점 요인이 될 수 있음. - A+는 날라갔군...

	GetClientRect(hWnd,Rect)

WM_SIZE :
	lParam : changed Width,Height Data -> use LOWORD(x) / HIWORD(y)
	wParam : TypeOfEvent - SIZE_MAXHIDE, etc.

WM_MOVE : --window position change event--
	//anchor = screen base, device coordination
	lParam : changed Position Data -> use LOWORD(x) / HIWORD(y)

Resources :
	Menu,Bitmap,Accelerator,string,icon,cursor...etc.

	WInMain:
		wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
			//setMenu...

	WinProc :
		WM_COMMAND:
			ID = LOWORD(wParam)
			ID_FILE_MENU...
			HIWORD : 	메뉴 : 0	 / 엑셀러레이터 : 1

	Cursor :
		wc.hIcon
		wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));					// 윈도우가 최소화 될때 보여줄 아이콘
		wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(<IDC_Cursor_ID>));

	Accelerator :
		//Set ID to target ID in Accelerator

		//At winMain
		HACCEL hAccel;
		hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

		//in message Loop
		if(!TranslateAccelerator(hWnd, hAccel, &Message))
			//Keyboard Message to WM_COMMAND

	string Table :
			LoadString(gInstance, IDS_STRING133, str, 256);
				gInstance : global, proc contains not instance.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
6주차에 진행됨

GDI/StockObject :
	HBRUSH / HPEN

	BRUSH / GetStockObject :
		auto handle = GetStockObject(<COLOR>_<TYPE>) as HandleType
			일부 정의된 단색만
		auto handle = CreateSolidBrush(COLORREF) / CreateHatchBrush(<HatchStyle_Name>,COLORREF)
			사용자 지정 단색/ 일부 정의된 무늬까지

	PEN / CreatePen :
		auto handle = CreatePen(<PenStyle_Name>,size,COLORREF)
		Style : solid, dash, dot, dashdot dashdotdot

	SelectObject :
		Return = LastSelectedObject, param = hdc, target
		//종류별로 하나씩만 하면됨. 브러시/펜 동시 사용 가능

	//IN WndProc
		instance = (HBRUSH)GetStockObject(GRAY_BRUSH);
		last = (TYPE)SelectObject(hdc,instance);

				--<Something Render Code here>--

		(TYPE)SelectObject(hdc,last);
		DeleteObject(instance)
*/

/*							4주차

MessageBox :
	MessageBox(hWnd, _T(""), _T(""), MB_OK);
		//MB_OK / MB_ABORTRETRYIGNORE / MB_OKCANCEL ...
	Return value - IDOK, IDCANCEL... etc.

WM_CHAR :
	Input : wParam

	InvaliedRect(hWnd, <Rect(NULL == ALL)> ,bErase)
WM_KEYDOWN :
	Input : wParam (VK_~)
MOUSE EVENT :
	WM_<L/R>BUTTON<UP/DOWN/DBCLICK>
	WM_MOUSEMOVE
		x = LOWORD(lParam)
		y = HIWORD(lParam)
		wParame = combination Key (Mouse + keyboard) MK_CONTROL, MK_LBUTTON ...etc.

Timer Event :
	WM_TIMER
		wParam is ID

	SetTimer (hWnd,ID,Elapse<ms>,Callback)
		void CALLBACK TimerProc(HWND hWnd, UINT umsg, UINT idEvent, DWORD dwTime)
		//if Callback registered , call callback instead

	KillTimer (hWnd,ID)
	GetLocalTime(*SYSTEMTIME)
		SYSTEMTIME

SendMessage :
	SendMessage(hWnd,msg,wparam,lparam)
		msg = WM_<NAME>

*/

/*							3주차

Text :
	SetTextAlign(hdc,TA_<TYPE>);
	TextOut(hdc, 0, 100, "", <length of string>);

	DrawText(hdc, "", <length of string>, &RECT, DT_<TYPE>);	//<length of string> == -1 => length

	//	TA_UPDATECP == return EndPosition of string
Draw :
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd);
	EndPaint(hWnd, &ps);

	SetPixel(hdc,x,y,COLORREFF);
	MoveToEx(hdc,destination::x,destination::y,lpPoint &LastPosition);
	LineTo(hdc,vector::x,vector::y);
Cursor :
	POINT p;
	if GetCursorPos(&p)
	if ScreenToClient(hWnd, &p))

Rectangle :

*/