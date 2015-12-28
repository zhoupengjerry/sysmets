/*
* sysmets.c -- Final System Metrics Display Program
* Copyright(C) Catsoft(R) Studio, 2015
*/

#include	<windows.h>
#include	"sysmets.h"

LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


int	WINAPI	WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR szCmdLine, int nCmdShow)
{
	TCHAR		szAppName[] = TEXT("SysMets");
	HWND		hWnd;
	MSG			uMsg;
	WNDCLASS	wcWndClass;

	wcWndClass.style = CS_HREDRAW | CS_VREDRAW;
	wcWndClass.lpfnWndProc = WndProc;
	wcWndClass.cbClsExtra = 0;
	wcWndClass.cbWndExtra = 0;
	wcWndClass.hInstance = hInstance;
	wcWndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcWndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcWndClass.lpszMenuName = NULL;
	wcWndClass.lpszClassName = szAppName;
	if (!RegisterClass(&wcWndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return	0;
	}
	hWnd = CreateWindow(szAppName, TEXT("Get System Metrics"),
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL,	hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&uMsg, NULL, 0, 0))
	{
		TranslateMessage(&uMsg);
		DispatchMessage(&uMsg);
	}
	return	uMsg.wParam;
}

LRESULT	CALLBACK	WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int			i, x, y, nVertPos, nHorzPos, nPaintBeg, nPaintEnd;
	static	int	cxChar, cxCaps, cyChar, cxClient, cyClient, nMaxWidth;
	static	int	nDeltaPerLine, nAccumDelta;
	TCHAR		szBuffer[10];
	HDC			hdc;
	PAINTSTRUCT	ps;
	SCROLLINFO	si;
	TEXTMETRIC	tm;
	ULONG		ulScrollLines;

	switch (uMsg)
	{
	case	WM_CREATE:
		hdc = GetDC(hWnd);
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cxCaps = (tm.tmPitchAndFamily & TMPF_FIXED_PITCH ? 3 : 2) * cxChar / 2;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		ReleaseDC(hWnd, hdc);
		// 保存3列的宽度
		nMaxWidth = 40 * cxChar + 22 * cxCaps;
	case	WM_SETTINGCHANGE:
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);
		if (ulScrollLines)
			nDeltaPerLine = WHEEL_DELTA / ulScrollLines;
		else
			nDeltaPerLine = 0;
		return	0;
	case	WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		// 设置垂直滚动条
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = NUMLINES - 1;
		si.nPage = cyClient / cyChar;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		// 设置水平滚动条
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = 2 + nMaxWidth / cxChar;
		si.nPage = cxClient / cxChar;
		SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
		return	0;
	case	WM_VSCROLL:
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);
		nVertPos = si.nPos;
		switch (LOWORD(wParam))
		{
		case	SB_TOP:
			si.nPos = si.nMin;
			break;
		case	SB_BOTTOM:
			si.nPos = si.nMax;
			break;
		case	SB_LINEUP:
			si.nPos--;
			break;
		case	SB_LINEDOWN:
			si.nPos++;
			break;
		case	SB_PAGEUP:
			si.nPos -= si.nPage;
			break;
		case	SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;
		case	SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
		default:
			break;
		}
		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hWnd, SB_VERT, &si);
		if (si.nPos != nVertPos)
		{
			ScrollWindow(hWnd, 0, cyChar * (nVertPos - si.nPos), NULL, NULL);
			UpdateWindow(hWnd);
		}
		return	0;
	case	WM_HSCROLL:
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_HORZ, &si);
		nHorzPos = si.nPos;
		switch (LOWORD(wParam))
		{
		case	SB_LINELEFT:
			si.nPos--;
			break;
		case	SB_LINERIGHT:
			si.nPos++;
			break;
		case	SB_PAGELEFT:
			si.nPos -= si.nPage;
			break;
		case	SB_PAGERIGHT:
			si.nPos += si.nPage;
			break;
		case	SB_THUMBPOSITION:
			si.nPos = si.nTrackPos;
		default:
			break;
		}
		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
		GetScrollInfo(hWnd, SB_HORZ, &si);
		if (si.nPos != nHorzPos)
		{
			ScrollWindow(hWnd, cxChar * (nHorzPos - si.nPos), 0, NULL, NULL);
		}
		return	0;
	case	WM_KEYDOWN:
		switch (wParam)
		{
		case	VK_HOME:
			SendMessage(hWnd, WM_VSCROLL, SB_TOP, 0);
			break;
		case	VK_END:
			SendMessage(hWnd, WM_VSCROLL, SB_BOTTOM, 0);
			break;
		case	VK_PRIOR:
			SendMessage(hWnd, WM_VSCROLL, SB_PAGEUP, 0);
			break;
		case	VK_NEXT:
			SendMessage(hWnd, WM_VSCROLL, SB_PAGEDOWN, 0);
			break;
		case	VK_UP:
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, 0);
			break;
		case	VK_DOWN:
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, 0);
			break;
		case	VK_LEFT:
			SendMessage(hWnd, WM_HSCROLL, SB_PAGEUP, 0);
			break;
		case	VK_RIGHT:
			SendMessage(hWnd, WM_HSCROLL, SB_PAGEDOWN, 0);
		}
		return	0;
	case	WM_MOUSEWHEEL:
		if (!nDeltaPerLine)
			break;
		nAccumDelta += (short)HIWORD(wParam);
		while (nAccumDelta >= nDeltaPerLine)
		{
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, 0);
			nAccumDelta -= nDeltaPerLine;
		}
		while (nAccumDelta <= -nDeltaPerLine)
		{
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, 0);
			nAccumDelta += nDeltaPerLine;
		}
		return	0;
	case	WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(hWnd, SB_VERT, &si);
		nVertPos = si.nPos;
		GetScrollInfo(hWnd, SB_HORZ, &si);
		nHorzPos = si.nPos;
		nPaintBeg = max(0, nVertPos + ps.rcPaint.top / cyChar);
		nPaintEnd = min(NUMLINES - 1,
			nVertPos + ps.rcPaint.bottom / cyChar);
		for (i = nPaintBeg; i <= nPaintEnd; i++)
		{
			x = cxChar * (1 - nHorzPos);
			y = cyChar * (i - nVertPos);
			TextOut(hdc, x, y,
				sysmetrics[i].szLabel,
				lstrlen(sysmetrics[i].szLabel));
			TextOut(hdc, x + 22 * cxCaps, y,
				sysmetrics[i].szDesc,
				lstrlen(sysmetrics[i].szDesc));
			SetTextAlign(hdc, TA_RIGHT | TA_TOP);
			TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer,
				wsprintf(szBuffer, TEXT("%5d"),
					GetSystemMetrics(sysmetrics[i].iIndex)));
			SetTextAlign(hdc, TA_LEFT | TA_TOP);
		}
		EndPaint(hWnd, &ps);
		return	0;
	case	WM_DESTROY:
		PostQuitMessage(0);
		return	0;
	default:
		return	DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return	0;
}