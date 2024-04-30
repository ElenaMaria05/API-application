#undef UNICODE
#include <windows.h>
#include "resource.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#pragma warning(disable: 4996)
using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg,
	WPARAM wParam, LPARAM lParam);

BOOL dlgActive = FALSE; //variabila pentru caseta de dialog


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static char szAppName[] = "Fiu";
	HWND        hwnd;
	MSG         msg;
	WNDCLASSEX  wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;  //optiuni pentru stilul clasei 
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// incarca pictograma fisierelor de tip aplicatie
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); // incarca cursorul sageata  
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	// fondul ferstrei de culoare alba     
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(szAppName, //numele clasei inregistrat cu RegisterClass
		"ProcesFiu",    // text pentru bara de titlu a ferestrei
		WS_OVERLAPPEDWINDOW,   // stilul ferestrei
		CW_USEDEFAULT,      // pozitia orizontala implicitã
		CW_USEDEFAULT,      // pozitia verticala implicita
		CW_USEDEFAULT,       // latimea implicita
		CW_USEDEFAULT,       // inaltimea implicita
		NULL,               // handle-ul ferestrei parinte
		NULL,               // handle-ul meniului ferestrei
		hInstance,          // proprietara ferestrei 
		NULL);


	SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE);
	// schimba dimensiunea, pozitia si ordinea z a ferestrei copil, a ferestrei pop-up
	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
HWND hDialog;
LPCTSTR GetFromClip = "GetFromClipboard";
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static UINT WM_GETFROMCLIP;
	static HINSTANCE hInstance;
	switch (iMsg)
	{
	case WM_CREATE: // operatiile ce se executa la crearea ferestrei
		// se creaza caseta de dialog
		WM_GETFROMCLIP = RegisterWindowMessage(GetFromClip);
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		if (!dlgActive) {
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG),
				hwnd, (DLGPROC)DlgProc);
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			// insereaza un nou mesaj nou in coada de asteptare
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0); // insereaza un mesaj de incheiere 
		return 0;
	}
	if (iMsg == WM_GETFROMCLIP)
	{
		SendMessage(hDialog, WM_GETFROMCLIP, 0, 0);
		return 0;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////
int numarareCifre(int n)
{
	if (n == 0)
		return 1;
	int nrcif = 0;
	while (n != 0) {
		n = n / 10;
		++nrcif;
	}
	return nrcif;
}


BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	BOOL bAvailable;
	LPCTSTR GetFromClip = "GetFromClipboard";
	static UINT WM_GETFROMCLIP;
	HANDLE hClipMemory;
	char* pGlobalMemory;
	HGLOBAL hGlobalMemory;
	char* pMyCopy, * pClipMemory;
	static char szBufferReceive[105] = "";
	char* cuv;
	static int m[1005][1005], nrcif = 0, ct[1005], nr, nrcifmax = -1, k, sum;
	float ma[1005];
	char text[1005]="", text2[1005]="";
	switch (iMsg) {

	case WM_INITDIALOG:
		hDialog = hDlg;
		WM_GETFROMCLIP = RegisterWindowMessage(GetFromClip);
		// setari initiale	     
		return TRUE;

	case WM_CLOSE:
		dlgActive = FALSE;
		EndDialog(hDlg, 0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_CALCULEAZA:
			strcpy(text, "");
			for (int i = 1; i <= nrcifmax; i++)
			{
				sum = 0;
				for (int j = 1; j <= ct[i]; j++)
				{
					if (m[i][j] != 0)
					{
						sum = sum + m[i][j];
					}
				}
				ma[++k] = (float) sum / (ct[i]);
			}
			for (int i = 1; i <= k; i++)
			{
				sprintf(text2, "%f", ma[i]);
				strcat(text, text2);
				strcat(text, ",");
				strcat(text, " ");
			}
			SetDlgItemText(hDlg, IDC_EDIT, text);
			return TRUE;
		}
		break;
	}

	if (iMsg == WM_GETFROMCLIP)
	{
		bAvailable = IsClipboardFormatAvailable(CF_TEXT);
		if (bAvailable == FALSE) return 0;
		OpenClipboard(hDlg);
		hClipMemory = GetClipboardData(CF_TEXT); //obtinere handle la memoria clipboard
		pMyCopy = (char*)malloc(GlobalSize(hClipMemory)); //alocare dinamica de memorie (buffer)
		pClipMemory = (char*)GlobalLock(hClipMemory); //obtinere pointer la memoria clipboard
		strcpy(pMyCopy, pClipMemory); //copiere date
		GlobalUnlock(hClipMemory);
		CloseClipboard();
		strcpy_s(szBufferReceive, pMyCopy); //actualizare buffer pentru afisare
		cuv = strtok(szBufferReceive, " ");
		while (cuv != NULL)
		{
			nr = atoi(cuv);
			nrcif = numarareCifre(nr);
			ct[nrcif] = ct[nrcif] + 1;
			if (nrcif > nrcifmax) nrcifmax = nrcif;
			m[nrcif][ct[nrcif]] = nr;
			
					
			cuv = strtok(NULL, " ");
		}
		for (int i = 1; i <= nrcifmax; i++)
		{
			for(int j = 1; j <= ct[i]; j ++)
			{
				if (m[i][j] != 0)
				{
					sprintf(text2, "%i", m[i][j]);
					strcat(text, text2);
					strcat(text, " ");
				}
			}
			strcat(text, "\r\n");
		}
		SetDlgItemText(hDlg, IDC_EDIT, text);
		return TRUE;
	}

	return FALSE;
}
