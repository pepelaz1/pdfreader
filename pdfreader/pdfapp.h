#pragma once
#include "stdafx.h"

enum panning
{
	DONT_PAN = 0,
	PAN_TO_TOP,
	PAN_TO_BOTTOM
};

class CPdfApp
{
private:
	char m_filename[MAX_PATH];
	bool m_opened;
	char *m_doctitle;
	fz_context *m_ctx;
	fz_document *m_doc;
	int m_pagecount;
	fz_outline *m_outline;

	// current view params 
	int m_resolution;
	int m_orig_resolut;
	int m_rotate;
	fz_pixmap *m_image;
	int m_grayscale;
	int m_invert;

	// current page params 
	int m_pageno;
	int m_org_page;
	fz_page *m_page;
	fz_rect m_page_bbox;
	fz_display_list *m_page_list;
	fz_text_page *m_page_text;
	fz_text_sheet *m_page_sheet;
	fz_link *m_page_links;

	// window system sizes 
	int m_winw, m_winh;
	int m_scrw, m_scrh;
	int m_shrinkwrap;
	int m_fullscreen;

	// event handling state 
	char m_number[256];
	int m_numberlen;

	// search state 
	int m_isediting;
	int m_searchdir;
	char m_search[512];
	int m_hit;
	int m_hitlen;


	// panning
	int m_ispanning;
	int m_panx, m_pany;

	int m_iscopying;
	int m_selx, m_sely;

	// Unknown
	fz_context *context;

	int m_justcopied;
	BITMAPINFO *m_dibinf;
	HBRUSH m_bgbrush;
    HBRUSH m_shbrush;


	// Printing
	int m_printw, m_printh;

	// Information
	char _keyword[256];
	char _author[256];
	char _title[256];
	char _subject[256];
	char _producer[256];
	char _creation_date[256];
	char _modify_date[256];
	bool _is_encrypted;
	double _version;


	CCritSec m_lock;

	void PanView(int newx, int newy);
	fz_matrix ViewCtm();
	void LoadPage();
	void ShowPage(int loadpage, int drawpage, int repaint);
	bool SearchForward(enum panning *panto);
	bool SearchBackward(enum panning *panto);
	int Textlen(fz_text_page *page);
	int Match(char *s, fz_text_page *page, int n);
	inline int Charat(fz_text_page *page, int idx);
	fz_text_char Textcharat(fz_text_page *page, int idx);
	inline fz_bbox Bboxcharat(fz_text_page *page, int idx);
	void InvertHit();
	void Invert(fz_bbox rect);
public:
	CPdfApp();
	~CPdfApp();
	void Init();
	void Open(char *filename, bool reload = false, char *password = NULL);
	bool IsOpened();
	void Close();
	void Resize(int w, int h);
	void Blit(HDC hdc);
	int GetWidth();
	int GetHeight();
	void Prev();
	void Next();
	int GetPagesCount();
	void Go(int pageno);
	void ZoomIn();
	void ZoomOut();
	void OriginalSize();
	void StartGrab();
	void StopGrab();
	HBITMAP GetPageBitmap(int pageno);
	int Search(char *text);
	int SearchNext();
	int SearchPrev();
	RECT GetHitRect();
	void PrintPage(HDC hdc, int pageno);
	void BeginPrinting(HDC hdc);
	void EndPrinting(HDC hdc);
	char* GetKeyword();
	char* GetAuthor();
	char* GetTitle();
	char* GetSubject();
	char* GetProducer();
	char* GetCreationDate();
	char* GetModifyDate();
	bool IsEncrypted();
	double GetVersionNo();
};

