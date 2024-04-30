#undef UNICODE
#include <windows.h>
#include "resource.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#pragma warning(disable: 4996)
using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg,
	WPARAM wParam, LPARAM lParam);

BOOL dlgActive = FALSE; //variabila pentru caseta de dialog


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static char szAppName[] = "Parinte";
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
		"ProcesParinte",    // text pentru bara de titlu a ferestrei
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
LPCTSTR GetFromClip = "GetFromClipboard";
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	static UINT WM_GETFROMCLIP;

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


	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////


BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	static char szBuffer[1005];
	COPYDATASTRUCT cs;
	HWND hwndReceive;
	HGLOBAL hGlobalMemory;
	char* pGlobalMemory;
	bool bAvailable;
	LPCSTR GetFromClip = "GetFromClipboard";
	static UINT WM_GETFROMCLIP;
	HANDLE hClipMemory;
	switch (iMsg) {

	case WM_INITDIALOG:
		WM_GETFROMCLIP = RegisterWindowMessage(GetFromClip);
		// setari initiale	     
		return TRUE;

	case WM_CLOSE:
		dlgActive = FALSE;
		EndDialog(hDlg, 0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_TRANSMITERE: 
			hwndReceive = FindWindow("Fiu", "ProcesFiu");
			if (hwndReceive != NULL)
			{
				ifstream file("fisier.txt");
				string line;
				if (file.is_open()) {
					while (getline(file, line)) {
					strcpy(szBuffer, line.c_str());
					hGlobalMemory = GlobalAlloc(GHND, strlen(szBuffer) + 1); //alocare memorie in heap
					pGlobalMemory = (char*)GlobalLock(hGlobalMemory); //obtinerea unui pointer la memoria alocata
					for (UINT i = 0; i < strlen(szBuffer); i++) //copiere date in heap
						*(pGlobalMemory++) = szBuffer[i];
					GlobalUnlock(hGlobalMemory);
					OpenClipboard(hDlg); //deschidere clipboard
					SetClipboardData(CF_TEXT, hGlobalMemory); //transmitere date
					CloseClipboard(); //inchidere clipboard
					SendMessage(hwndReceive, WM_GETFROMCLIP, 0, 0); // notificare server pentru preluare date
						Sleep(3000);
					}
					MessageBox(hDlg, "Am transmis toate numerele cu succes!", "Numere transmise cu succes", MB_OK);
					file.close();
				}
				
			}
			else
				MessageBox(hDlg, "Nu am gasit fiul", "Eroare", MB_OK);
			
			return TRUE;
		}
		break;
	}
	return FALSE;
}
