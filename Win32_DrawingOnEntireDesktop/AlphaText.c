#include "AlphaText.h"
#pragma comment(lib, "Msimg32.lib")

HBITMAP CreateAlphaTextBitmap(char* inText, HFONT inFont, COLORREF inColour)
{
	int TextLength = (int)strlen(inText);
	if (TextLength <= 0) return NULL;

	// Create DC and select font into it 
	HDC hTextDC = CreateCompatibleDC(NULL);
	HFONT hOldFont = (HFONT)SelectObject(hTextDC, inFont);
	HBITMAP hMyDIB = NULL;

	// Get text area 
	RECT TextArea = { 0, 0, 0, 0 };
	DrawTextA(hTextDC, inText, TextLength, &TextArea, DT_CALCRECT);
	if ((TextArea.right > TextArea.left) && (TextArea.bottom > TextArea.top))
	{
		BITMAPINFOHEADER BMIH;
		memset(&BMIH, 0x0, sizeof(BITMAPINFOHEADER));
		void* pvBits = NULL;

		// Specify DIB setup 
		BMIH.biSize = sizeof(BMIH);
		BMIH.biWidth = TextArea.right - TextArea.left;
		BMIH.biHeight = TextArea.bottom - TextArea.top;
		BMIH.biPlanes = 1;
		BMIH.biBitCount = 32;
		BMIH.biCompression = BI_RGB;

		// Create and select DIB into DC 
		hMyDIB = CreateDIBSection(hTextDC, (LPBITMAPINFO)&BMIH, 0, (LPVOID*)&pvBits, NULL, 0);
		HBITMAP hOldBMP = (HBITMAP)SelectObject(hTextDC, hMyDIB);
		if (hOldBMP != NULL)
		{
			// Set up DC properties 
			SetTextColor(hTextDC, 0x00FFFFFF);
			SetBkColor(hTextDC, 0x00000000);
			SetBkMode(hTextDC, OPAQUE);

			// Draw text to buffer 
			DrawTextA(hTextDC, inText, TextLength, &TextArea, DT_NOCLIP);
			BYTE* DataPtr = (BYTE*)pvBits;
			BYTE FillR = GetRValue(inColour);
			BYTE FillG = GetGValue(inColour);
			BYTE FillB = GetBValue(inColour);
			BYTE ThisA;
			for (int LoopY = 0; LoopY < BMIH.biHeight; LoopY++) {
				for (int LoopX = 0; LoopX < BMIH.biWidth; LoopX++) {
					ThisA = *DataPtr; // Move alpha and pre-multiply with RGB 
					*DataPtr++ = (FillB * ThisA) >> 8;
					*DataPtr++ = (FillG * ThisA) >> 8;
					*DataPtr++ = (FillR * ThisA) >> 8;
					*DataPtr++ = ThisA; // Set Alpha 
				}
			}

			// De-select bitmap 
			SelectObject(hTextDC, hOldBMP);
		}
	}

	// De-select font and destroy temp DC 
	SelectObject(hTextDC, hOldFont);
	DeleteDC(hTextDC);

	// Return DIBSection 
	return hMyDIB;
}

void TestAlphaText(HDC inDC, int inX, int inY)
{
	//const char* DemoText = L"Hello World!\0";
	RECT TextArea = { 0, 0, 0, 0 };
	HFONT TempFont = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial\0");
	HBITMAP MyBMP = CreateAlphaTextBitmap("Hello World!\0", TempFont, 0xFF);
	DeleteObject(TempFont);
	if (MyBMP)
	{
		// Create temporary DC and select new Bitmap into it 
		HDC hTempDC = CreateCompatibleDC(inDC);
		HBITMAP hOldBMP = (HBITMAP)SelectObject(hTempDC, MyBMP);
		if (hOldBMP)
		{
			// Get Bitmap image size
			BITMAP BMInf;
			GetObject(MyBMP, sizeof(BITMAP), &BMInf);

			// Fill blend function and blend new text to window 
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 0x80;
			bf.AlphaFormat = AC_SRC_ALPHA;
			AlphaBlend(inDC, inX, inY, BMInf.bmWidth, BMInf.bmHeight, hTempDC, 0, 0, BMInf.bmWidth, BMInf.bmHeight, bf);

			// Clean up 
			SelectObject(hTempDC, hOldBMP);
			DeleteObject(MyBMP);
			DeleteDC(hTempDC);
		}
	}
}
