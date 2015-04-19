// pdfreader.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"


CPdfApp g_app;

extern "C" 
{
	__declspec(dllexport) void  Open(char *srcfile, bool reload = false, char *password = NULL)
	{
		g_app.Open(srcfile,reload, password);
	}

	__declspec(dllexport) bool IsOpened()
	{
		return g_app.IsOpened();
	}

	__declspec(dllexport) void Close()
	{
		g_app.Close();
	}

	__declspec(dllexport) void Resize(int cx, int cy)
	{
		g_app.Resize(cx,cy);
	}

	__declspec(dllexport) void Blit(HDC hdc)
	{
		g_app.Blit(hdc);
	}

	__declspec(dllexport) int GetWidth()
	{
		return g_app.GetWidth();
	}

	__declspec(dllexport) int GetHeight()
	{
		return g_app.GetHeight();
	}

	__declspec(dllexport) void Go(int pageno)
	{
		g_app.Go(pageno);
	}

	__declspec(dllexport) int TotalPages()
	{
		return g_app.GetPagesCount();
	}

	__declspec(dllexport) void Previous()
	{
		g_app.Prev();
	}

	__declspec(dllexport) void Next()
	{
		g_app.Next();

	}

	__declspec(dllexport) void ZoomIn()
	{
		g_app.ZoomIn();
	}

	__declspec(dllexport) void ZoomOut()
	{
		g_app.ZoomOut();
	}

	__declspec(dllexport) void OriginalSize()
	{
		g_app.OriginalSize();
	}

	__declspec(dllexport) int SearchText(char *text)
	{
		return g_app.Search(text);
	}

	__declspec(dllexport) int SearchNext()
	{
		 return g_app.SearchNext();
	}

	__declspec(dllexport) int SearchPrevious()
	{
		return g_app.SearchPrev();
	}

	__declspec(dllexport) RECT GetHitRect()
	{
		return g_app.GetHitRect();
	}
	

	__declspec(dllexport) HBITMAP GetHBITMAP(int pageno)
	{
		return g_app.GetPageBitmap(pageno);
	}

	__declspec(dllexport) void StartGrab()
	{
		return g_app.StartGrab();
	}

	__declspec(dllexport) void StopGrab()
	{
		return g_app.StopGrab();
	}

	__declspec(dllexport) void BeginPrinting(HDC hdc)
	{
		g_app.BeginPrinting(hdc);
	}
	
	__declspec(dllexport) void EndPrinting(HDC hdc)
	{
		g_app.EndPrinting(hdc);
	}

	__declspec(dllexport) void PrintPage(HDC hdc, int pageno)
	{
		g_app.PrintPage(hdc,pageno);
	}

	__declspec(dllexport) char* PDFGetKeyword()
	{
		return g_app.GetKeyword();
	}
	__declspec(dllexport) char* PDFGetAuthor()
	{
		return g_app.GetAuthor();
	}

	__declspec(dllexport) char* PDFGetTitle()
	{
		return g_app.GetTitle();
	}

	__declspec(dllexport) char* PDFGetSubject()
	{
		return g_app.GetSubject();
	}

	__declspec(dllexport) char* PDFGetProducer()
	{
		return g_app.GetProducer();
	}

	__declspec(dllexport) char* PDFGetCreationDate()
	{
		return g_app.GetCreationDate();
	}

	__declspec(dllexport) char* PDFGetModifyDate()
	{
		return g_app.GetModifyDate();
	}

	__declspec(dllexport) bool PDFIsEncrypted()
	{
		return g_app.IsEncrypted();
	}

	__declspec(dllexport) double PDFGetVersionNo()
	{
		return g_app.GetVersionNo();
	}

}