#include "pdfapp.h"

#define MINRES 10
#define MAXRES 600
#define MAX_TITLE 256
#define ZOOMSTEP 1.142857

CPdfApp::CPdfApp()
{
	Init();
}


CPdfApp::~CPdfApp()
{
}

void CPdfApp::Init()
{
	m_ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	m_scrw = 640;
	m_scrh = 480;
	m_resolution = 72;
	m_pagecount = 0;
	m_justcopied = 0;

	m_dibinf = (BITMAPINFO *)malloc(sizeof(BITMAPINFO) + 12);
	m_dibinf->bmiHeader.biSize = sizeof(m_dibinf->bmiHeader);
	m_dibinf->bmiHeader.biPlanes = 1;
	m_dibinf->bmiHeader.biBitCount = 32;
	m_dibinf->bmiHeader.biCompression = BI_RGB;
	m_dibinf->bmiHeader.biXPelsPerMeter = 2834;
	m_dibinf->bmiHeader.biYPelsPerMeter = 2834;
	m_dibinf->bmiHeader.biClrUsed = 0;
	m_dibinf->bmiHeader.biClrImportant = 0;
	m_dibinf->bmiHeader.biClrUsed = 0;

	m_bgbrush = CreateSolidBrush(RGB(0x70,0x70,0x70));
	m_shbrush = CreateSolidBrush(RGB(0x40,0x40,0x40));
	m_org_page = 1;
	m_opened = false;
}

void ShowError(char *msg)
{
#ifdef VERBOSE
	MessageBox(NULL,msg,"Error", MB_OK);
#endif
}


void CPdfApp::LoadPage()
{
	fz_device *mdev;

	if (m_page_list)
		fz_free_display_list(m_ctx, m_page_list);
	if (m_page_text)
		fz_free_text_page(m_ctx, m_page_text);
	if (m_page_sheet)
		fz_free_text_sheet(m_ctx, m_page_sheet);
	if (m_page_links)
		fz_drop_link(m_ctx, m_page_links);
	if (m_page)
		fz_free_page(m_doc, m_page);

	m_page_list = NULL;
	m_page_text = NULL;
	m_page_sheet = NULL;
	m_page_links = NULL;
	m_page = NULL;

	fz_try(m_ctx)
	{
		m_page = fz_load_page(m_doc, m_pageno - 1);

		/* Create display list */
		m_page_list = fz_new_display_list(m_ctx);
		mdev = fz_new_list_device(m_ctx, m_page_list);
		fz_run_page(m_doc, m_page, mdev, fz_identity, NULL);
		fz_free_device(mdev);

		m_page_bbox = fz_bound_page(m_doc, m_page);
		m_page_links = fz_load_links(m_doc, m_page);
	}
	fz_catch(m_ctx)
	{
		//pdfapp_error(app, "cannot load page");
		ShowError("cannot load page");
	}
}

void CPdfApp::PanView(int newx, int newy)
{
	if (m_image)
	{
		int image_w = fz_pixmap_width(m_ctx, m_image);
		int image_h = fz_pixmap_height(m_ctx, m_image);

		if (newx > 0)
			newx = 0;
		if (newy > 0)
			newy = 0;

		if (newx + image_w < m_winw)
			newx = m_winw - image_w;
		if (newy + image_h < m_winh)
			newy = m_winh - image_h;

		if (m_winw >= image_w)
			newx = (m_winw - image_w) / 2;
		if (m_winh >= image_h)
			newy = (m_winh - image_h) / 2;

		//if (newx != m_panx || newy != m_pany)
		//	winrepaint(app);

		m_panx = newx;
		m_pany = newy;
	}
}



fz_matrix CPdfApp::ViewCtm()
{
	fz_matrix ctm;
	ctm = fz_scale(m_resolution/72.0f, m_resolution/72.0f);
	ctm = fz_concat(ctm, fz_rotate(m_rotate));
	return ctm;

}
void CPdfApp::ShowPage(int loadpage, int drawpage, int repaint)
{
//	char buf[MAX_TITLE];
	fz_device *idev;
	fz_device *tdev;
	fz_colorspace *colorspace;
	fz_matrix ctm;
	fz_bbox bbox;

	//wincursor(app, WAIT);
	
	if (loadpage)
	{
		///pdfapp_loadpage(app);
		LoadPage();

		//* Zero search hit position 
		m_hit = -1;
		m_hitlen = 0;

		// Extract text 
		m_page_sheet = fz_new_text_sheet(m_ctx);
		m_page_text = fz_new_text_page(m_ctx, m_page_bbox);
		tdev = fz_new_text_device(m_ctx, m_page_sheet, m_page_text);
		fz_run_display_list(m_page_list, tdev, fz_identity, fz_infinite_bbox, NULL);
		fz_free_device(tdev);
	}

	if (drawpage)
	{
	/*	char buf2[64];
		int len;

		sprintf(buf2, " - %d/%d (%d dpi)",
				m_pageno, m_pagecount, m_resolution);
		len = MAX_TITLE-strlen(buf2);
		if ((int)strlen(m_doctitle) > len)
		{
			snprintf(buf, len-3, "%s", m_doctitle);
			strcat(buf, "...");
			strcat(buf, buf2);
		}
		else
			sprintf(buf, "%s%s", app->doctitle, buf2);
		wintitle(app, buf);*/

		ctm = ViewCtm();
		bbox = fz_round_rect(fz_transform_rect(ctm, m_page_bbox));

		// Draw 
		if (m_image)
			fz_drop_pixmap(m_ctx, m_image);
		if (m_grayscale)
			colorspace = fz_device_gray;
		else
#ifdef _WIN32
			colorspace = fz_device_bgr;
#else
			colorspace = fz_device_rgb;
#endif
		m_image = fz_new_pixmap_with_bbox(m_ctx, colorspace, bbox);
		fz_clear_pixmap_with_value(m_ctx, m_image, 255);
		idev = fz_new_draw_device(m_ctx, m_image);
		fz_run_display_list(m_page_list, idev, ctm, bbox, NULL);
		fz_free_device(idev);
		if (m_invert)
			fz_invert_pixmap(m_ctx, m_image);
	}

	if (repaint)
	{
		///pdfapp_panview(app, app->panx, app->pany);
		PanView(m_panx, m_pany);

		if (m_shrinkwrap)
		{
			int w = fz_pixmap_width(m_ctx, m_image);
			int h = fz_pixmap_height(m_ctx, m_image);
			if (m_winw == w)
				m_panx = 0;
			if (m_winh == h)
				m_pany = 0;
			if (w > m_scrw * 90 / 100)
				w = m_scrw * 90 / 100;
			if (h > m_scrh * 90 / 100)
				h = m_scrh * 90 / 100;
			///if (w != app->winw || h != app->winh)
				///winresize(app, w, h);
		}

		//winrepaint(app);

		//wincursor(app, ARROW);
	}

	fz_flush_warnings(m_ctx);
}


void CPdfApp::Open(char *filename, bool reload, char *password)
{
	//char *password = "";

	fz_try(m_ctx)
	{
		m_doc = fz_open_document(m_ctx, filename);
		//m_pageno = 1;
		if (fz_needs_password(m_doc))
		{
			int okay = fz_authenticate_password(m_doc, password);
			if (!okay)
				ShowError("Invalid password");
		/*	while (!okay)
			{
				password = winpassword(app, filename);
				if (!password)
					pdfapp_error(app, "Needs a password.");
				okay = fz_authenticate_password(app->doc, password);
				if (!okay)
					pdfapp_warn(app, "Invalid password.");
			}*/
		}

		m_doctitle = filename;
		if (strrchr(m_doctitle, '\\'))
			m_doctitle = strrchr(m_doctitle, '\\') + 1;
		if (strrchr(m_doctitle, '/'))
			m_doctitle = strrchr(m_doctitle, '/') + 1;
		m_doctitle = fz_strdup(m_ctx, m_doctitle);

		m_pagecount = fz_count_pages(m_doc);
		m_outline = fz_load_outline(m_doc);

		// Read info
		_version = (double)((pdf_document *)m_doc)->version / 10.0;
		char buff[256];
		fz_meta(m_doc, FZ_META_CRYPT_INFO, buff, 256);
		_is_encrypted = !lstrcmp(buff,"None") ? false : true;

		memset(_keyword,0,256);
		*(char **)_keyword = "Keywords";
		fz_meta(m_doc, FZ_META_INFO, _keyword, 256);

		memset(_author,0,256);
		*(char **)_author = "Author";
		fz_meta(m_doc, FZ_META_INFO, _author, 256);		

		memset(_title,0,256);
		*(char **)_title = "Title";
		fz_meta(m_doc, FZ_META_INFO, _title, 256);	

		memset(_subject,0,256);
		*(char **)_subject = "Subject";
		fz_meta(m_doc, FZ_META_INFO, _subject, 256);	

		memset(_producer,0,256);
		*(char **)_producer = "Producer";
		fz_meta(m_doc, FZ_META_INFO, _producer, 256);	

		memset(_creation_date,0,256);
		*(char **)_creation_date = "CreationDate";
		fz_meta(m_doc, FZ_META_INFO, _creation_date, 256);	

		memset(_modify_date,0,256);
		*(char **)_modify_date = "ModDate";
		fz_meta(m_doc, FZ_META_INFO, _modify_date, 256);				
	}
	fz_catch(m_ctx)
	{
		ShowError("cannot open document");
	}

	if (m_pageno < 1)
		m_pageno = 1;
	if (m_pageno > m_pagecount)
		m_pageno = m_pagecount;
	if (m_resolution < MINRES)
		m_resolution = MINRES;
	if (m_resolution > MAXRES)
		m_resolution = MAXRES;

	m_orig_resolut = m_resolution;

	if (!reload)
	{
		m_shrinkwrap = 1;
		m_rotate = 0;
		m_panx = 0;
		m_pany = 0;
	}

	ShowPage(1, 1, 1);
	m_opened = true;
}

char* CPdfApp::GetKeyword()
{
	return _keyword;
}

char* CPdfApp::GetAuthor()
{
	return _author;
}

char* CPdfApp::GetTitle()
{
	return _title;
}

char* CPdfApp::GetSubject()
{
	return _subject;
}

char* CPdfApp::GetProducer()
{
	return _producer;
}

char* CPdfApp::GetCreationDate()
{
	return _creation_date;
}

char* CPdfApp::GetModifyDate()
{
	return _modify_date;
}

bool CPdfApp::IsEncrypted()
{
	return _is_encrypted;
}

double CPdfApp::GetVersionNo()
{
	return _version;
}

bool CPdfApp::IsOpened()
{
	return m_opened;
}

inline fz_bbox CPdfApp::Bboxcharat(fz_text_page *page, int idx)
{
	return fz_bbox_covering_rect(Textcharat(page, idx).bbox);
}


void CPdfApp::Invert(fz_bbox rect)
{
	fz_invert_pixmap_rect(m_image, rect);

}
void CPdfApp::InvertHit()
{
	fz_bbox hitbox, bbox;
	fz_matrix ctm;
	int i;

	if (m_hit < 0)
		return;

	hitbox = fz_empty_bbox;
	ctm = ViewCtm();

	for (i = m_hit; i < m_hit + m_hitlen; i++)
	{
		bbox = Bboxcharat(m_page_text, i);
		if (fz_is_empty_rect(bbox))
		{
			if (!fz_is_empty_rect(hitbox))
			{
				fz_bbox tbox = hitbox;
				/*tbox.x0 += m_panx;
				tbox.x1 += m_panx;
				tbox.y0 += m_pany;
				tbox.y1 += m_pany;
				tbox = fz_transform_bbox(ctm, tbox);
				SetRect(&m_rectHit, tbox.x0, tbox.y0, tbox.x1, tbox.y1);*/		
				Invert(fz_transform_bbox(ctm, hitbox));
			}
			hitbox = fz_empty_bbox;
		}
		else
		{
			hitbox = fz_union_bbox(hitbox, bbox);
		}
	}

	if (!fz_is_empty_rect(hitbox))
	{

		fz_bbox tbox = hitbox;
		/*tbox.x0 += m_panx;
		tbox.x1 += m_panx;
		tbox.y0 += m_pany;
		tbox.y1 += m_pany;
		tbox = fz_transform_bbox(ctm, tbox);
		SetRect(&m_rectHit, tbox.x0, tbox.y0, tbox.x1, tbox.y1);*/		
		Invert(fz_transform_bbox(ctm, hitbox));
	}
}

int  CPdfApp::GetWidth()
{
	if (m_image)
	{
		return fz_pixmap_width(m_ctx, m_image);;
	}
	return -1;
}

int  CPdfApp::GetHeight()
{
	if (m_image)
	{
		return fz_pixmap_height(m_ctx, m_image);
	}
	return -1;
}

void CPdfApp::Blit(HDC hdc)
{
	if (m_image)
	{
		int image_w = fz_pixmap_width(m_ctx, m_image);
		int image_h = fz_pixmap_height(m_ctx, m_image);
		int image_n = fz_pixmap_components(context, m_image);
		unsigned char *samples = fz_pixmap_samples(context, m_image);
		int x0 = m_panx;
		int y0 = m_pany;
		int x1 = m_panx + image_w;
		int y1 = m_pany + image_h;
		RECT r;

		if (m_image)
		{

			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP bmp = CreateCompatibleBitmap(hdc,image_w,image_h);
			HBITMAP oldbmp = (HBITMAP)SelectObject(hdcMem,bmp);

			if (m_iscopying || m_justcopied)
			{
				//Invert(m_selr);
				m_justcopied = 1;
			}

			InvertHit();

			m_dibinf->bmiHeader.biWidth = image_w;
			m_dibinf->bmiHeader.biHeight = -image_h;
			m_dibinf->bmiHeader.biSizeImage = image_h * 4;

			if (image_n == 2)
			{
				int i = image_w * image_h;
				unsigned char *color = (unsigned char *)malloc(i*4);
				unsigned char *s = samples;
				unsigned char *d = color;
				for (; i > 0 ; i--)
				{
					d[2] = d[1] = d[0] = *s++;
					d[3] = *s++;
					d += 4;
				}
				SetDIBitsToDevice(hdc,
					m_panx, m_pany, image_w, image_h,
					0, 0, 0, image_h, color,
					m_dibinf, DIB_RGB_COLORS);
				free(color);
			}
			if (image_n == 4)
			{
				SetDIBitsToDevice(hdcMem,
					0, 0, image_w, image_h,
					//0, 0, image_w, image_h,
					//0, 0, 0, image_h, samples,
					0, 0, 0, image_h, samples,
					m_dibinf, DIB_RGB_COLORS);
			}

			InvertHit();

			if (m_iscopying || m_justcopied)
			{
				m_justcopied = 1;
			}
			


			// Grey background 
			r.top = 0; r.bottom = m_winh;
			r.left = 0; r.right = x0;
			FillRect(hdc, &r, m_bgbrush);
			r.left = x1; r.right = m_winw;
			FillRect(hdc, &r, m_bgbrush);
			r.left = 0; r.right = m_winw;
			r.top = 0; r.bottom = y0;
			FillRect(hdc, &r, m_bgbrush);
			r.top = y1; r.bottom = m_winh;
			FillRect(hdc, &r, m_bgbrush);

			// Drop shadow 
			r.left = x0 + 2;
			r.right = x1 + 2;
			r.top = y1;
			r.bottom = y1 + 2;
			FillRect(hdc, &r, m_shbrush);
			r.left = x1;
			r.right = x1 + 2;
			r.top = y0 + 2;
			r.bottom = y1;
			FillRect(hdc, &r, m_shbrush);

			BitBlt(hdc, m_panx, m_pany, image_w, image_h, hdcMem, 0, 0, SRCCOPY); 

			SelectObject(hdcMem, oldbmp);
			DeleteObject(oldbmp);
			DeleteObject(bmp);
			DeleteObject(hdcMem);

		}
	}
}

void CPdfApp::Close()
{
	if (m_page_list)
		fz_free_display_list(m_ctx, m_page_list);
	m_page_list = NULL;

	if (m_page_text)
		fz_free_text_page(m_ctx, m_page_text);
	m_page_text = NULL;

	if (m_page_sheet)
		fz_free_text_sheet(m_ctx, m_page_sheet);
	m_page_sheet = NULL;

	if (m_page_links)
		fz_drop_link(m_ctx, m_page_links);
	m_page_links = NULL;

	if (m_doctitle)
		fz_free(m_ctx, m_doctitle);
	m_doctitle = NULL;

	if (m_image)
		fz_drop_pixmap(m_ctx, m_image);
	m_image = NULL;

	if (m_outline)
		fz_free_outline(m_ctx, m_outline);
	m_outline = NULL;

	if (m_page)
		fz_free_page(m_doc, m_page);
	m_page = NULL;

	if (m_doc)
	{
		fz_close_document(m_doc);
		m_doc = NULL;
	}

	fz_flush_warnings(m_ctx);
}

void CPdfApp::Resize(int w, int h)
{
	if (m_winw != w || m_winh != h)
	{
		m_winw = w;
		m_winh = h;
		PanView(m_panx, m_pany);
	}
}

void CPdfApp::Prev()
{
	if (m_image)
	{
		m_pageno--;

		if (m_pageno < 1)
			m_pageno = 1;

		ShowPage(1, 1, 1);
	}
}

void CPdfApp::Next()
{
	if (m_image)
	{
		m_pageno++;

		if ( m_pageno > m_pagecount)
			m_pageno = m_pagecount ;
		ShowPage(1, 1, 1);
	}
}

int CPdfApp::GetPagesCount()
{
	return m_pagecount;
}

void CPdfApp::Go(int pageno)
{
	if (m_image)
	{
		m_pageno = pageno;

		if (m_pageno < 1)
			m_pageno = 1;

		if ( m_pageno > m_pagecount)
			m_pageno = m_pagecount;

		ShowPage(1, 1, 1);
	}
}

void CPdfApp::ZoomIn()
{
	CAutoLock lck(&m_lock);
	if (m_image)
	{
		m_resolution *= ZOOMSTEP;
		if (m_resolution > MAXRES)
			m_resolution = MAXRES;
		ShowPage(0, 1, 1);
	}
}

void CPdfApp::ZoomOut()
{
	CAutoLock lck(&m_lock);
	if (m_image)
	{
		m_resolution /= ZOOMSTEP;
		if (m_resolution < MINRES)
			m_resolution = MINRES;
		ShowPage(0, 1, 1);
	}
}


void CPdfApp::OriginalSize()
{
	CAutoLock lck(&m_lock);
	if (m_image)
	{
		m_resolution = m_orig_resolut;
		ShowPage(0, 1, 1);
	}
}

void CPdfApp::StartGrab()
{
	m_org_page = m_pageno;
}
void CPdfApp::StopGrab()
{
	m_pageno = m_org_page;
	ShowPage(1,1,0);
}

HBITMAP CPdfApp::GetPageBitmap(int pageno)
{
	m_pageno = pageno;
	ShowPage(1,1,0);
	
	HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC hdcMem = CreateCompatibleDC(hdc);

	int image_w = fz_pixmap_width(m_ctx, m_image);
	int image_h = fz_pixmap_height(m_ctx, m_image);
	int image_n = fz_pixmap_components(context, m_image);
	unsigned char *samples = fz_pixmap_samples(context, m_image);
	int x0 = m_panx;
	int y0 = m_pany;
	int x1 = m_panx + image_w;
	int y1 = m_pany + image_h;

	HBITMAP bmp = CreateCompatibleBitmap(hdc,image_w,image_h);
	HBITMAP oldbmp = (HBITMAP)SelectObject(hdcMem,bmp);

	if (m_image)
	{
		m_dibinf->bmiHeader.biWidth = image_w;
		m_dibinf->bmiHeader.biHeight = -image_h;
		m_dibinf->bmiHeader.biSizeImage = image_h * 4;

		if (image_n == 2)
		{
			int i = image_w * image_h;
			unsigned char *color = (unsigned char *)malloc(i*4);
			unsigned char *s = samples;
			unsigned char *d = color;
			for (; i > 0 ; i--)
			{
				d[2] = d[1] = d[0] = *s++;
				d[3] = *s++;
				d += 4;
			}
			SetDIBitsToDevice(hdcMem,
				0, 0, image_w, image_h,
				0, 0, 0, image_h, color,
				m_dibinf, DIB_RGB_COLORS);
			free(color);
		}

		if (image_n == 4)
		{
			SetDIBitsToDevice(hdcMem,
				0, 0, image_w, image_h,
				0, 0, 0, image_h, samples,
				m_dibinf, DIB_RGB_COLORS);
		}
	}
	SelectObject(hdcMem, oldbmp);
	DeleteDC(hdcMem);
	DeleteDC(hdc);
	return bmp;	
}

fz_text_char CPdfApp::Textcharat(fz_text_page *page, int idx)
{
	static fz_text_char emptychar = { {0,0,0,0}, ' ' };
	fz_text_block *block;
	fz_text_line *line;
	fz_text_span *span;
	int ofs = 0;
	for (block = page->blocks; block < page->blocks + page->len; block++)
	{
		for (line = block->lines; line < block->lines + block->len; line++)
		{
			for (span = line->spans; span < line->spans + line->len; span++)
			{
				if (idx < ofs + span->len)
					return span->text[idx - ofs];
				/* pseudo-newline */
				if (span + 1 == line->spans + line->len)
				{
					if (idx == ofs + span->len)
						return emptychar;
					ofs++;
				}
				ofs += span->len;
			}
		}
	}
	return emptychar;
}



inline int CPdfApp::Charat(fz_text_page *page, int idx)
{
	return Textcharat(page, idx).c;
}

int CPdfApp::Match(char *s, fz_text_page *page, int n)
{
	int orig = n;
	int c;
	while ((c = *s++))
	{
		if (c == ' ' && Charat(page, n) == ' ')
		{
			while (Charat(page, n) == ' ')
				n++;
		}
		else
		{
			if (tolower(c) != tolower(Charat(page, n)))
				return 0;
			n++;
		}
	}
	return n - orig;
}


int CPdfApp::Textlen(fz_text_page *page)
{
	fz_text_block *block;
	fz_text_line *line;
	fz_text_span *span;
	int len = 0;
	for (block = page->blocks; block < page->blocks + page->len; block++)
	{
		for (line = block->lines; line < block->lines + block->len; line++)
		{
			for (span = line->spans; span < line->spans + line->len; span++)
				len += span->len;
			len++; /* pseudo-newline */
		}
	}
	return len;
}

bool CPdfApp::SearchForward(enum panning *panto)
{
	int matchlen;
	int test;
	int len;
	int startpage;

	startpage = m_pageno;

	do
	{
		len = Textlen(m_page_text);

		if (m_hit >= 0)
			test = m_hit + strlen(m_search);
		else
			test = 0;

		while (test < len)
		{
			matchlen = Match(m_search, m_page_text, test);
			if (matchlen)
			{
				m_hit = test;
				m_hitlen = matchlen;
				return true;
			}
			test++;
		}

		m_pageno++;
		if (m_pageno > m_pagecount)
			m_pageno = 1;

		ShowPage(1,0,0);
		*panto = PAN_TO_TOP;

	} while (m_pageno != startpage);

	if (m_pageno == startpage)
	{
		return false;
	//	pdfapp_warn(app, "String '%s' not found.", app->search);
	}
	return true;
}

bool CPdfApp::SearchBackward(enum panning *panto)
{
	int matchlen;
	int test;
	int len;
	int startpage;
	startpage = m_pageno;

	do
	{
		len = Textlen(m_page_text);

		if (m_hit >= 0)
			test = m_hit - 1;
		else
			test = len;

		while (test >= 0)
		{
			matchlen = Match(m_search, m_page_text, test);
			if (matchlen)
			{
				m_hit = test;
				m_hitlen = matchlen;
				return true;
			}
			test--;
		}

		m_pageno--;
		if (m_pageno < 1)
			m_pageno = m_pagecount;

		ShowPage(1, 0, 0);
		*panto = PAN_TO_BOTTOM;

	} while (m_pageno != startpage);

	if (m_pageno == startpage)
		return false;
		//pdfapp_warn(app, "String '%s' not found.", m_search);
	return true;

}

int CPdfApp::Search(char *text)
{
	CAutoLock lck(&m_lock);
	m_isediting = 1;
	m_searchdir = 1;
	m_search[0] = 0;
	lstrcpy(m_search,text);
	m_hit = -1;
	m_hitlen = 0;

	int oldpage = m_pageno;

	panning panto;
    if(!SearchForward(&panto))
		return -1;

	if (m_pageno != oldpage)
	{
		ShowPage(0, 1, 1);
	}
	return m_pageno;
}

int CPdfApp::SearchNext()
{
	CAutoLock lck(&m_lock);
	int oldpage = m_pageno;

	panning panto;
    if(!SearchForward(&panto))
		return -1;

	if (m_pageno != oldpage)
	{
		ShowPage(0, 1, 1);
	}
	return m_pageno;
}

int CPdfApp::SearchPrev()
{
	CAutoLock lck(&m_lock);
	int oldpage = m_pageno;

	panning panto;
    if(!SearchBackward(&panto))
		return -1;

	if (m_pageno != oldpage)
	{
		ShowPage(0, 1, 1);
	}
	return m_pageno;
}

RECT CPdfApp::GetHitRect()
{
	RECT hitRect;
	SetRect(&hitRect,0,0,0,0);
	fz_bbox hitbox, bbox;
	fz_matrix ctm;
	int i;

	if (m_hit < 0)
		return hitRect;

	hitbox = fz_empty_bbox;
	ctm = ViewCtm();

	for (i = m_hit; i < m_hit + m_hitlen; i++)
	{
		bbox = Bboxcharat(m_page_text, i);
		if (fz_is_empty_rect(bbox))
		{
			if (!fz_is_empty_rect(hitbox))
			{
				fz_bbox tbox = hitbox;
				/*tbox.x0 += m_panx;
				tbox.x1 += m_panx;
				tbox.y0 += m_pany;
				tbox.y1 += m_pany;*/
				tbox = fz_transform_bbox(ctm, tbox);
				SetRect(&hitRect, tbox.x0, tbox.y0, tbox.x1, tbox.y1);
						//Invert(fz_transform_bbox(ctm, hitbox));
			}
			hitbox = fz_empty_bbox;
		}
		else
		{
			hitbox = fz_union_bbox(hitbox, bbox);
		}
	}

	if (!fz_is_empty_rect(hitbox))
	{

		fz_bbox tbox = hitbox;
		/*tbox.x0 += m_panx;
		tbox.x1 += m_panx;
		tbox.y0 += m_pany;
		tbox.y1 += m_pany;*/
		tbox = fz_transform_bbox(ctm, tbox);
		SetRect(&hitRect, tbox.x0, tbox.y0, tbox.x1, tbox.y1);
		//SetRect(&hitRect,0,0,0,0);
		//Invert(fz_transform_bbox(ctm, hitbox));
	}
	return hitRect;
}

void CPdfApp::BeginPrinting(HDC hdc)
{
	m_printw = GetDeviceCaps(hdc,HORZRES);
	m_printh = GetDeviceCaps(hdc,VERTRES); 

	m_org_page = m_pageno;
	m_orig_resolut = m_resolution;

	m_resolution *= 5*ZOOMSTEP;
}

void CPdfApp::EndPrinting(HDC hdc)
{
	m_resolution = m_orig_resolut;
	m_pageno = m_org_page;
	ShowPage(1,1,1);
}

void CPdfApp::PrintPage(HDC hdc, int pageno)
{
	int mapmode = GetMapMode(hdc);
	m_pageno = pageno;
	ShowPage(1,1,0);
	
	HDC hdcMem = CreateCompatibleDC(hdc);

	SetMapMode(hdcMem, mapmode);
	SetStretchBltMode(hdc,HALFTONE);
	SetStretchBltMode(hdcMem,HALFTONE);

	int image_w = fz_pixmap_width(m_ctx, m_image);
	int image_h = fz_pixmap_height(m_ctx, m_image);
	int image_n = fz_pixmap_components(context, m_image);
	unsigned char *samples = fz_pixmap_samples(context, m_image);
	int x0 = m_panx;
	int y0 = m_pany;
	int x1 = m_panx + image_w;
	int y1 = m_pany + image_h;

	HBITMAP bmp = CreateCompatibleBitmap(hdc,image_w,image_h);
	HBITMAP oldbmp = (HBITMAP)SelectObject(hdcMem,bmp);

	if (m_image)
	{
		m_dibinf->bmiHeader.biWidth = image_w;
		m_dibinf->bmiHeader.biHeight = -image_h;
		m_dibinf->bmiHeader.biSizeImage = image_h * 4;

		if (image_n == 2)
		{
			int i = image_w * image_h;
			unsigned char *color = (unsigned char *)malloc(i*4);
			unsigned char *s = samples;
			unsigned char *d = color;
			for (; i > 0 ; i--)
			{
				d[2] = d[1] = d[0] = *s++;
				d[3] = *s++;
				d += 4;
			}
			SetDIBitsToDevice(hdcMem,
				0, 0, image_w, image_h,
				0, 0, 0, image_h, color,
				m_dibinf, DIB_RGB_COLORS);
			free(color);
		}

		if (image_n == 4)
		{
			SetDIBitsToDevice(hdcMem,
				0, 0, image_w, image_h,
				0, 0, 0, image_h, samples,
				m_dibinf, DIB_RGB_COLORS);
		}
	}

	// Make some aspect ratio correction
	int ypos = 0;
	double r = (double)image_w/(double)image_h;
	int ph  = (double)m_printw / r;
	int diff = m_printh - ph;
	if (diff > 0)
	{
		ypos += diff/2;		
	}	

	StretchBlt(hdc, 0, ypos, m_printw, ph, hdcMem, 0, 0, image_w, image_h, SRCCOPY); 
	SelectObject(hdcMem, oldbmp);
	DeleteDC(hdcMem);
}