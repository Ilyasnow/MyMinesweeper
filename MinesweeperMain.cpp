#include"MinesweeperHeader.h"

void OpenEmptyTiles(HWND hWnd, int *iTableMatrix, int i, int j, HBITMAP hBitmap);
LRESULT CALLBACK MinesweeperWndProc(HWND,UINT,UINT,LONG);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR ipCmdLine, int nCmdShow)
{
	HWND hWnd, hStatusWnd;
	WNDCLASS WndClass;
	MSG Msg;
	char szClassName[]="MinesweeperClass";
	WndClass.style=CS_HREDRAW| CS_VREDRAW;
	WndClass.lpfnWndProc=MinesweeperWndProc;	
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hInstance=hInstance;
	WndClass.hIcon=LoadIcon(NULL, IDI_ASTERISK/*IDI_APPLICATION*/);
	WndClass.hCursor=LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName=NULL;
	WndClass.lpszClassName=szClassName;

if(!RegisterClass(&WndClass))
{
 MessageBox(NULL, "Невозможно зарегистрировать класс", "Ошибка", MB_OK);
 return 0;
}

hWnd=CreateWindow(szClassName, "Сапёр",
				  WS_MINIMIZEBOX | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
				  100, 100,
				  (SCREEN_X_TILES*20)+6, (SCREEN_Y_TILES*20)+28,
				  NULL, NULL,
				  hInstance, NULL);
//hStatusWnd = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "Количество мин:0/"+MINE_NUMBER, hWnd, 1);
if(!hWnd)
{
 MessageBox(NULL, "Невозможно создать окно ", "Ошибка", MB_OK);
	 return 0;
}

/*hStatusWnd=CreateWindow(szClassName, "",
				  WS_CHILD | WS_VISIBLE | WS_BORDER,
				  0, 0,
				  0, 0,
				  hWnd, NULL,
				  hInstance, NULL);*/

ShowWindow(hWnd, nCmdShow);
UpdateWindow(hWnd);

while(GetMessage(&Msg,NULL,0,0))
{
	TranslateMessage(&Msg);
    DispatchMessage(&Msg);
}
 return Msg.wParam;
}

LRESULT CALLBACK MinesweeperWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HDC hDC, hMemDC;
	static HRGN *hRgn;
	static HBITMAP hBitmap, hOldBitmap;
	static BITMAP Bitmap;
	static int *iTableMatrix;
	static int cxClient, cyClient, iRgnCount=0;
	int i, j, ixCursor, iyCursor, iMineNumber;
	PAINTSTRUCT PaintStruct;
	BOOLEAN boolPlaying = TRUE;
	switch(msg)
	{
	case WM_CREATE:
		srand(time(0));
		hRgn = (HRGN*)malloc(SCREEN_X_TILES * SCREEN_Y_TILES * sizeof(HRGN));
		iTableMatrix = (int*)malloc(SCREEN_X_TILES * SCREEN_Y_TILES * sizeof(int));
		if(!(hBitmap = (HBITMAP)LoadImage(NULL, "Minesweeper1.bmp", IMAGE_BITMAP, 0,0, LR_LOADFROMFILE)))
			{
				MessageBox(NULL, "Файл графики не прочитан", "Ошибка", MB_OK);
				return 0;
			}
		for(j = 0; j < SCREEN_Y_TILES; j++)
			for(i = 0; i < SCREEN_X_TILES; i++)
			{
				*(hRgn + j * SCREEN_X_TILES + i) = CreateRectRgn(i * 20, j * 20, (i * 20)+20, (j *20) + 20);
				iRgnCount++;
				//FillRgn(hDC, *(hRgn + (j/20) * SCREEN_X_TILES + (i/20)), (HBRUSH)GetStockObject(DKGRAY_BRUSH));
			}
		if( hRgn == NULL )
			MessageBox(hWnd, "Создать регион не удалось", "Ошибка", MB_OK);
		iMineNumber = MINE_NUMBER;
		boolPlaying = TRUE;
		while(iMineNumber > 0)
			{
				int ixRandom = rand() % SCREEN_X_TILES;
				int iyRandom = rand() % SCREEN_Y_TILES;
				if(*(iTableMatrix + (iyRandom * SCREEN_X_TILES + ixRandom)) != 9)
				{
					*(iTableMatrix + (iyRandom * SCREEN_X_TILES + ixRandom)) = 9;
					iMineNumber--;
				}
			}
		//-------------iTableMatrix values-----------	
		// 0 - Clear
		// 1..8 - Number of mines around
		// 9 - Mine
		// 10 - Opened Clear
		// 11..18 - Opened Number of mines around
		// 19 - Opened Mine
		// 20 - Flagged Clear
		// 21..28 - Flagged Number of mines around
		// 29 - Flagged Mine
		for(j = 0; j < SCREEN_Y_TILES; j++)
			for(i = 0; i < SCREEN_X_TILES; i++)
			{
				if(*(iTableMatrix + j * SCREEN_X_TILES + i) != 9)
					*(iTableMatrix + j * SCREEN_X_TILES + i) = 0;
			}
		for(j = 0; j < SCREEN_Y_TILES; j++)
			for(i = 0; i < SCREEN_X_TILES; i++)
				if(*(iTableMatrix + j * SCREEN_X_TILES + i) == 9)
				{
					if((*(iTableMatrix + (j - 1) * SCREEN_X_TILES + (i - 1))) != 9 && j > 0                    && i > 0                   ) (*(iTableMatrix + (j - 1) * SCREEN_X_TILES + (i - 1)))++;
					if((*(iTableMatrix + (j - 1) * SCREEN_X_TILES + (i    ))) != 9 && j > 0                                               ) (*(iTableMatrix + (j - 1) * SCREEN_X_TILES + (i    )))++;
					if((*(iTableMatrix + (j - 1) * SCREEN_X_TILES + (i + 1))) != 9 && j > 0                    && i < (SCREEN_X_TILES - 1)) (*(iTableMatrix + (j - 1) * SCREEN_X_TILES + (i + 1)))++;
					if((*(iTableMatrix + (j    ) * SCREEN_X_TILES + (i - 1))) != 9                             && i > 0                   ) (*(iTableMatrix + (j    ) * SCREEN_X_TILES + (i - 1)))++;
					if((*(iTableMatrix + (j    ) * SCREEN_X_TILES + (i + 1))) != 9                             && i < (SCREEN_X_TILES - 1)) (*(iTableMatrix + (j    ) * SCREEN_X_TILES + (i + 1)))++;
					if((*(iTableMatrix + (j + 1) * SCREEN_X_TILES + (i - 1))) != 9 && j < (SCREEN_Y_TILES - 1) && i > 0                   ) (*(iTableMatrix + (j + 1) * SCREEN_X_TILES + (i - 1)))++;
					if((*(iTableMatrix + (j + 1) * SCREEN_X_TILES + (i    ))) != 9 && j < (SCREEN_Y_TILES - 1)                            ) (*(iTableMatrix + (j + 1) * SCREEN_X_TILES + (i    )))++;
					if((*(iTableMatrix + (j + 1) * SCREEN_X_TILES + (i + 1))) != 9 && j < (SCREEN_Y_TILES - 1) && i < (SCREEN_X_TILES - 1)) (*(iTableMatrix + (j + 1) * SCREEN_X_TILES + (i + 1)))++;
				}
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hDC=BeginPaint(hWnd,&PaintStruct);
		hMemDC=CreateCompatibleDC(hDC);
		SelectObject(hMemDC, hBitmap);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC,hBitmap);
		for(j = 0; j < SCREEN_Y_TILES; j++)
			for(i = 0; i < SCREEN_X_TILES; i++)
			{
				if(boolPlaying)
				{
					if((*(iTableMatrix + j * SCREEN_X_TILES + i)) < 10)
						BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 20, 0, SRCCOPY);
					else
						switch(*(iTableMatrix + j * SCREEN_X_TILES + i))
						{
						case 10:
							BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 60, 0, SRCCOPY);
							break;
						case 19:
							BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 0, 0, SRCCOPY);
							break;
						default:
							if((*(iTableMatrix + j * SCREEN_X_TILES + i)) < 20)
								BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 60+(((*(iTableMatrix + j * SCREEN_X_TILES + i))-10)*20), 0, SRCCOPY);
							else
								if((*(iTableMatrix + j * SCREEN_X_TILES + i)) < 30)
									BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 40, 0, SRCCOPY);
							break;
						}
				}
				else
				{
					if((*(iTableMatrix + j * SCREEN_X_TILES + i)) < 10)
						(*(iTableMatrix + j * SCREEN_X_TILES + i)) += 10;
					switch(*(iTableMatrix + j * SCREEN_X_TILES + i))
						{
						case 10:
							BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 60, 0, SRCCOPY);
							break;
						case 19:
							BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 0, 0, SRCCOPY);
							break;
						default:
							if((*(iTableMatrix + j * SCREEN_X_TILES + i)) < 20)
								BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 60+(((*(iTableMatrix + j * SCREEN_X_TILES + i))-10)*20), 0, SRCCOPY);
							else
								if((*(iTableMatrix + j * SCREEN_X_TILES + i)) < 29)
									BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 260, 0, SRCCOPY);
								else
									if((*(iTableMatrix + j * SCREEN_X_TILES + i)) == 29)
										BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 40, 0, SRCCOPY);
							break;
						}
				}
				/*if(*(iTableMatrix + j * SCREEN_X_TILES + i) == 9)
					FillRgn(hDC, *(hRgn + j * SCREEN_X_TILES + i), (HBRUSH)GetStockObject(DKGRAY_BRUSH));*/
			}
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
		EndPaint(hWnd,&PaintStruct);
		return 0;
	case WM_SIZE:
		//hDC = GetDC(hWnd);
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		/*for(j = 0; j < SCREEN_Y_TILES; j++)
			for(i = 0; i < SCREEN_X_TILES; i++)
			{
				if(*(iTableMatrix + j * SCREEN_X_TILES + i) == 9)
					FillRgn(hDC, *(hRgn + j * SCREEN_X_TILES + i), (HBRUSH)GetStockObject(DKGRAY_BRUSH));
			}*/
		return 0;
	case WM_LBUTTONUP:
		if(!boolPlaying) return 0;
		ixCursor = LOWORD(lParam);
		iyCursor = HIWORD(lParam);
		hDC = GetDC(hWnd);
		hMemDC=CreateCompatibleDC(hDC);
		SelectObject(hMemDC, hBitmap);
		for(j = 0; j < SCREEN_Y_TILES; j++)
			for(i = 0; i < SCREEN_X_TILES; i++)
				if(PtInRegion(*(hRgn + j * SCREEN_X_TILES + i), ixCursor, iyCursor))
					if((*(iTableMatrix + j * SCREEN_X_TILES + i)) <= 9)
					switch(*(iTableMatrix + j * SCREEN_X_TILES + i))
					{
					case 0:
						BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 60, 0, SRCCOPY);
						//OpenEmptyTiles(hWnd, iTableMatrix, i, j, hBitmap);
						(*(iTableMatrix + j * SCREEN_X_TILES + i)) = 10;
						break;
					case 9:
						BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 0, 0, SRCCOPY);
						(*(iTableMatrix + j * SCREEN_X_TILES + i)) = 19;
						if(MessageBox(hWnd, "Вы умерли. Начать заново?", "R.I.P.", MB_YESNO || MB_APPLMODAL) == IDYES)
						{
							SendMessage(hWnd, WM_CREATE, NULL, NULL);
						}
						else
						{
							boolPlaying = FALSE;
							InvalidateRect(hWnd, NULL, TRUE);
							//SendMessage(hWnd, WM_DESTROY, NULL, NULL);
						}
						break;
					default:
						if((*(iTableMatrix + j * SCREEN_X_TILES + i)) < 20)
						{
							BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 60+((*(iTableMatrix + j * SCREEN_X_TILES + i))*20), 0, SRCCOPY);
							(*(iTableMatrix + j * SCREEN_X_TILES + i)) += 10;
						}
						break;
					}
		DeleteDC(hMemDC);
		return 0;
	case WM_RBUTTONUP:
		if(!boolPlaying) return 0;
		ixCursor = LOWORD(lParam);
		iyCursor = HIWORD(lParam);
		hDC = GetDC(hWnd);
		hMemDC=CreateCompatibleDC(hDC);
		SelectObject(hMemDC, hBitmap);
		for(j = 0; j < SCREEN_Y_TILES; j++)
			for(i = 0; i < SCREEN_X_TILES; i++)
				if(PtInRegion(*(hRgn + j * SCREEN_X_TILES + i), ixCursor, iyCursor))
					if((*(iTableMatrix + j * SCREEN_X_TILES + i)) < 20)
					{
						BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 40, 0, SRCCOPY);
						(*(iTableMatrix + j * SCREEN_X_TILES + i)) += 20;
						break;
					}
					else
					if((*(iTableMatrix + j * SCREEN_X_TILES + i)) >= 20)
					{
						BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 20, 0, SRCCOPY);
						(*(iTableMatrix + j * SCREEN_X_TILES + i)) -= 20;
						break;
					}
		DeleteDC(hMemDC);
		return 0;
	case WM_DESTROY:		
		PostQuitMessage(0);
		free(hRgn);
		free(iTableMatrix);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void OpenEmptyTiles(HWND hWnd, int *iTableMatrix, int i, int j, HBITMAP hBitmap)
{
	HDC hDC, hMemDC;
	if((*(iTableMatrix + j * SCREEN_X_TILES + i)) == 0 || (*(iTableMatrix + j * SCREEN_X_TILES + i)) == 20)
	{
		hDC = GetDC(hWnd);
		hMemDC=CreateCompatibleDC(hDC);
		SelectObject(hMemDC, hBitmap);
		if((*(iTableMatrix + j * SCREEN_X_TILES + i)) <= 9)
		switch(*(iTableMatrix + j * SCREEN_X_TILES + i))
		{
		case 0:
			BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 60, 0, SRCCOPY);
			(*(iTableMatrix + j * SCREEN_X_TILES + i)) = 10;
			if(j > 0                    && i > 0                   ) OpenEmptyTiles(hWnd, iTableMatrix, i - 1, j - 1, hBitmap);
			if(j > 0                                               ) OpenEmptyTiles(hWnd, iTableMatrix, i    , j - 1, hBitmap);
			if(j > 0                    && i < (SCREEN_X_TILES - 1)) OpenEmptyTiles(hWnd, iTableMatrix, i + 1, j - 1, hBitmap);
			if(                            i > 0                   ) OpenEmptyTiles(hWnd, iTableMatrix, i - 1, j    , hBitmap);
			if(                            i < (SCREEN_X_TILES - 1)) OpenEmptyTiles(hWnd, iTableMatrix, i + 1, j    , hBitmap);
			if(j < (SCREEN_Y_TILES - 1) && i > 0                   ) OpenEmptyTiles(hWnd, iTableMatrix, i - 1, j + 1, hBitmap);
			if(j < (SCREEN_Y_TILES - 1)                            ) OpenEmptyTiles(hWnd, iTableMatrix, i    , j + 1, hBitmap);
			if(j < (SCREEN_Y_TILES - 1) && i < (SCREEN_X_TILES - 1)) OpenEmptyTiles(hWnd, iTableMatrix, i + 1, j + 1, hBitmap);
			break;
		default:
			if((*(iTableMatrix + j * SCREEN_X_TILES + i)) < 20)
			{
				BitBlt(hDC, i * 20, j * 20, 20, 20, hMemDC, 60+((*(iTableMatrix + j * SCREEN_X_TILES + i))*20), 0, SRCCOPY);
				//DeleteDC(hMemDC);
				(*(iTableMatrix + j * SCREEN_X_TILES + i)) += 10;
			}
			break;
		}
		DeleteDC(hMemDC);
	}
}
