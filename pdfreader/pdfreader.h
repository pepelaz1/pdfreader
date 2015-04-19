#pragma once
#include <windows.h>

extern "C" 
{
	__declspec(dllexport) void Open(char *srcfile, bool reload = false, char* userpassword = NULL);
	__declspec(dllexport) bool IsOpened();
	__declspec(dllexport) void Close();
	__declspec(dllexport) void Resize(int cx, int cy);
	__declspec(dllexport) void Blit(HDC hdc);
	__declspec(dllexport) int GetWidth();
	__declspec(dllexport) int GetHeight();
	__declspec(dllexport) void Go(int pageno);
	__declspec(dllexport) int TotalPages();
	__declspec(dllexport) void Previous();
	__declspec(dllexport) void Next();
	__declspec(dllexport) void ZoomIn();
	__declspec(dllexport) void ZoomOut();
	__declspec(dllexport) void OriginalSize();
	// Grabbing
	__declspec(dllexport) HBITMAP StartGrab();
	__declspec(dllexport) HBITMAP StopGrab();
	__declspec(dllexport) HBITMAP GetHBITMAP(int pageno);
	//Searching
	__declspec(dllexport) int SearchText(char *text);
	__declspec(dllexport) int SearchNext();
	__declspec(dllexport) int SearchPrevious();
	__declspec(dllexport) RECT GetHitRect();
	// Printing
	__declspec(dllexport) void BeginPrinting(HDC hdc);
	__declspec(dllexport) void PrintPage(HDC hdc, int pageno);
	__declspec(dllexport) void EndPrinting(HDC hdc);
	// Information
	__declspec(dllexport) char* PDFGetKeyword();
	__declspec(dllexport) char* PDFGetAuthor();
	__declspec(dllexport) char* PDFGetTitle();
	__declspec(dllexport) char* PDFGetSubject();
	__declspec(dllexport) char* PDFGetProducer();
	__declspec(dllexport) char* PDFGetCreationDate();
	__declspec(dllexport) char* PDFGetModifyDate();
	__declspec(dllexport) bool PDFIsEncrypted();
	__declspec(dllexport) double PDFGetVersionNo();
};