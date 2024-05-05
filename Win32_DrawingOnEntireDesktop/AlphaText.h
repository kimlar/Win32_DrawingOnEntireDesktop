#pragma once

#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>

// WEBSOURCE(Author: cplotts): https://stackoverflow.com/questions/5309914/updatelayeredwindow-and-drawtext/10148741
HBITMAP CreateAlphaTextBitmap(char* inText, HFONT inFont, COLORREF inColour);
void TestAlphaText(HDC inDC, int inX, int inY);
