
/*
13����
	��������� :

	�� :
	SendMessageCallback(...);

	HHOOK hKeyHook = SetWindowsHookEx(WH_KEYBOARD, KeyHookProc, 0, GetCurrentThreadId());
	UnhookWindowsHookEx(hKeyHook);
		WindowHook_<EVENT_NAME> => WH_EVENT


	�޽��� ���� :
	while (true)
	{
		if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			//�޽��� ó�� ����
			if (Message.message == WM_QUIT) break;

			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			//�Ϲ� ������Ʈ ó�� ����
			...
		}
	}


12����
	��Ʈ����

11����
��ȭ����
	Modal		: DialogBox		: ��ȯ �Ұ�
	Modeless	: CreateDialog	: ��ȯ ����
		GetDlgItemText(hWnd, IDC_STR, str, 128);
		SetDlgItemText(hWnd, IDC_STR, str);

		x = GetDlgItemInt(hWnd, IDC_X, 0, 0);
		SetDlgItemInt(hWnd, IDC_X, x, 0);

		EndDialog(hWnd, IDOK);
			=>result of "DialogBox(g_Instance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AboutDlgProc)"

���� ��ȭ ����
	���Ͽ��� ��Ʈ���� ������ �μ� ã�� ��
	BOOL GetOpenFileName( LPOPENFILENAME lpofn );

	���Ͽ���
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
			wsprintf(str, TEXT("%s ������ �����߽��ϴ�."), ofn.lpstrFile);
			MessageBox(hWnd, str, TEXT("���� ���� ����"), MB_OK);
		}

	����
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
10����
+PostMessage(MessageQueue) / SendMessage(Direct)
+LOWORD / HIWORD <>MakeLong(hi,lo)

����Ʈ
	hEdit_1 = CreateWindow(_T("edit"), 0, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 10, 256, 32, hWnd, (HMENU)ID_EDIT_1, g_Instance, 0);
	WM_COMMAND:
		LOWORD(wParam) is EDIT ID (HMENU's value)
		HIWORD(wParam) is Edit Notify Message (EN_CHANGE, etc.)

		case EN_CHANGE:
			GetWindowText(hEdit_1, str_edit_1, 128);

����Ʈ�ڽ�
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
�޺��ڽ�
	hCombo= CreateWindow(_T("ComboBox"), 0, WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN, 10, 10, 100, 200, hWnd, (HMENU)ID_COMBOBOX, g_Instance, 0);

	//����Ʈ�ڽ� + ����Ʈ�� ���� ����

��ũ�ѹ�
	SetScrollRange(hWNd,ID, --- ---- -- --- --
	WM_<axis>SCROLL:
		LOWORD(wParam) : clicked position
		HIWORD(wParam) : nowPosition
		lParam : handle of Scrollbar

		SB_CTL : ScrollBar_ConTroL
	hRed = CreateWindow(_T("ScrollBar"), 0, WS_CHILD | WS_VISIBLE | SBS_HORZ, 10, 10, 200, 20, hWnd, (HMENU)ID_SCRRED, g_Instance, 0);
	SetScrollRange(hRed, SB_CTL, 0, 255, 1);
	SetScrollPos(hBlue, SB_CTL, 0, 1);

����ƽ

9����
�θ������� / �ڽ������� : �ڽĿ��� �θ� ����. unity�� ����
	WM_CREATE
��Ʈ�� : ����� �޾� ó���ϴ� �ϳ��� ������ & �������̽�
	��ư/�ؽ�Ʈ/��ũ�ѹ�/����Ʈ�ڽ� etc.
	CreateWindow�� menu(Control ID) => WM_COMMAND�� LOWORD(wParam)
		�ش�Window�� Notification => WM_COMMAND�� HIWORD(wParam)
		�ش�Window�� ������ �ڵ�=> WM_COMMAND�� lParam�� ��.
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

8����
*�߰����

7����
*����


6����
//�����ֱ��� �����ϰ� ����
�׸����� : ROP : Rasterize operation (�����Ϳ���)
	SetROP2(hdc,R2_<TYPE>)

���θ�� : MapingMode - MM_<Type>
	SetMapMode(hdc,fnMapMode)
	GetMapMode(hdc)

	����ǥ : DC�ڵ��� �μ��� ���
	������ǥ : ��µǴ� ��ǥ. �ȼ�����

����Ʈ :
	������ : ����ǥ ��� ����
	����Ʈ : ������ǥ�� ���Ǵ� ����

	Set<Viewport/Window>OrgEx(hdc,x,y,*Point last)

��Ʈ��:
	Memdc = CreateCompatibleDC
	myBit = LoadBitmap(g_Instance, MAKEINTRESOURCE(IDB_BITMAP2));
	oldBit = (HBITMAP)SelectObject(MemDC, myBit);

			--<Render>--
			BitBlt / StretchBlt
			--<Render>--

	SelectObject(MemDC, oldBit);
	DeleteObject(myBit);
	DeleteDC(MemDC);

	//���Ͽ��� ����, ��ǥ�� �ٸ�  | 1�ȼ��� 1�ȼ�
	BOOL BitBlt( HDC hdcDest, // ���� ��� DC
		int nXDest,			// ���� ��� X
		int nYDest,			// ���� ��� Y
		int nWidth,			// ���� ��� Width
		int nHeight,		// ���� ��� Height
		HDC hdcSrc,			// ���� ���� DC
		int nXSrc,			// ���� ���� X
		int nYSrc,			// ���� ���� Y
		DWORD dwRop );		// ������ ���� ���

	//����,��ǥ �Ѵ� �ٸ��� ������ �� ���� | 1�ȼ��� N�ȼ�
	BOOL StretchBlt( HDC hdcDest, // ���� ��� DC
		int nXOriginDest,	// ���� ��� X
		int nYOriginDest,	// ���� ��� Y
		int nWidthDest,		// ���� ��� Width
		int nHeightDest,	// ���� ��� Height
		HDC hdcSrc,			// ���� ���� DC
		int nXOriginSrc,	// ���� ���� X
		int nYOriginSrc,	// ���� ���� Y
		int nWidthSrc,		// ���� ���� Width
		int nHeightSrc,		// ���� ���� Height
		DWORD dwRop );		// ������ ���� ���


FONT :
	����
		HFONT CreateFont(
			int nHeight,
			int nWidth,
			int nEscapement				<��ü���� ����>,
			int nOrientation			<���� ���� ����>,
			int fnWeight �β�,
			DWORD fdwItalic,
			DWORD fdwUnderline,
			DWORD fdwStrikeOut,
			DWORD fdwCharSet <ANSI_CHARSET / OEM_CHARSET>,
			DWORD fdwOutputPrecision,
			DWORD fdwClipPrecision,
			DWORD fdwQuality,
			DWORD fdwPitchAndFamily,
			LPCTSTR lpszFace  );		<�۲��̸�>

		HFONT CreateFontIndirect( CONST LOGFONT *lplf); //����ü�� ����� ��������.
	����
		COLORREF SetTextColor( HDC hdc, COLORREF crColor );
		COLORREF SetBkColor( HDC hdc, COLORREF crColor );
		int SetBkMode( HDC hdc, int iBkMode );
*/

/*							5����
	*������ ���� ������ ������� �ƴ��ϸ� ���� ������ �� �� ����. - A+�� ���󰬱�...

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
			HIWORD : 	�޴� : 0	 / ������������ : 1

	Cursor :
		wc.hIcon
		wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));					// �����찡 �ּ�ȭ �ɶ� ������ ������
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
6������ �����

GDI/StockObject :
	HBRUSH / HPEN

	BRUSH / GetStockObject :
		auto handle = GetStockObject(<COLOR>_<TYPE>) as HandleType
			�Ϻ� ���ǵ� �ܻ���
		auto handle = CreateSolidBrush(COLORREF) / CreateHatchBrush(<HatchStyle_Name>,COLORREF)
			����� ���� �ܻ�/ �Ϻ� ���ǵ� ���̱���

	PEN / CreatePen :
		auto handle = CreatePen(<PenStyle_Name>,size,COLORREF)
		Style : solid, dash, dot, dashdot dashdotdot

	SelectObject :
		Return = LastSelectedObject, param = hdc, target
		//�������� �ϳ����� �ϸ��. �귯��/�� ���� ��� ����

	//IN WndProc
		instance = (HBRUSH)GetStockObject(GRAY_BRUSH);
		last = (TYPE)SelectObject(hdc,instance);

				--<Something Render Code here>--

		(TYPE)SelectObject(hdc,last);
		DeleteObject(instance)
*/

/*							4����

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

/*							3����

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