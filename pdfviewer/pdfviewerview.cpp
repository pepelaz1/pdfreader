
// pdfviewerview.cpp : implementation of the CPdfViewerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "pdfviewer.h"
#endif

#include "pdfviewerdoc.h"
#include "pdfviewerview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPdfViewerView

IMPLEMENT_DYNCREATE(CPdfViewerView, CScrollView )

BEGIN_MESSAGE_MAP(CPdfViewerView, CScrollView )
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_PAGESCOUNT, &CPdfViewerView::OnPagesCount)
	ON_COMMAND(ID_PREVPAGE, &CPdfViewerView::OnPrevPage)
	ON_COMMAND(ID_NEXTPAGE, &CPdfViewerView::OnNextPage)
	ON_COMMAND(ID_GOPAGE, &CPdfViewerView::OnGotoPage)
	ON_COMMAND(ID_ZOOMIN, &CPdfViewerView::OnZoomIn)
	ON_COMMAND(ID_ZOOMOUT, &CPdfViewerView::OnZoomOut)
	ON_COMMAND(ID_ORIGINALSIZE, &CPdfViewerView::OnOriginalSize)
	ON_COMMAND(ID_SAVEBITMAPS, &CPdfViewerView::OnSaveBitmaps)
	ON_COMMAND(ID_SEARCH, &CPdfViewerView::OnSearch)
	ON_COMMAND(ID_SEARCHNEXT, &CPdfViewerView::OnSearchNext)
	ON_COMMAND(ID_SEARCHPREV, &CPdfViewerView::OnSearchPrev)
	ON_COMMAND(ID_INFO, &CPdfViewerView::OnInfo)
	ON_COMMAND(ID_FILE_OPENENCRYPTED,&CPdfViewerView::OnOpenEncrypted)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CPdfViewerView construction/destruction

CPdfViewerView::CPdfViewerView()
{
	m_printing = false;
}

CPdfViewerView::~CPdfViewerView()
{
}

BOOL CPdfViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CScrollView::PreCreateWindow(cs);
}

// CPdfViewerView drawing

void CPdfViewerView::OnDraw(CDC *pDC)
{
	CPdfViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (!m_printing)
		Blit(pDC->GetSafeHdc());
}


// CPdfViewerView printing

BOOL CPdfViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(TotalPages());
	return DoPreparePrinting(pInfo);
}

void CPdfViewerView::OnBeginPrinting(CDC* pDC, CPrintInfo *pInfo)
{
	BeginPrinting(pDC->GetSafeHdc());
	m_printing = true;
}

void CPdfViewerView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	m_printing = false;
	EndPrinting(pDC->GetSafeHdc());
}



void CPdfViewerView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	PrintPage(pDC->GetSafeHdc(), pInfo->m_nCurPage);
	CScrollView::OnPrint(pDC, pInfo);
}


// CPdfViewerView diagnostics

#ifdef _DEBUG
void CPdfViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CPdfViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPdfViewerDoc* CPdfViewerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPdfViewerDoc)));
	return (CPdfViewerDoc*)m_pDocument;
}
#endif //_DEBUG


BOOL CPdfViewerView::OnEraseBkgnd(CDC* pDC)
{
	if (!IsOpened())
	{
		// Get client area
		CRect rc;
		GetClientRect(&rc);

		// Create grey brush and insert it into DC
		CBrush brush(RGB(128,128,128));
		CBrush* pOldBrush = pDC->SelectObject(&brush);

		// Fill window with brush
		pDC->PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);

		// Restore 
		pDC->SelectObject(pOldBrush);
	}
	return TRUE;
}


void CPdfViewerView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	Resize(cx,cy);
}


void CPdfViewerView::OnPagesCount()
{
	CString str;
	str.Format("Document contains %d pages", TotalPages());
	MessageBox(str);
}

void CPdfViewerView::OnPrevPage()
{
	Previous();
	Invalidate();
}

void CPdfViewerView::OnNextPage()
{
	Next();
	Invalidate();
}

void CPdfViewerView::OnGotoPage()
{
	CDlgGo dlg;
	if (dlg.DoModal() == IDOK)
	{
		Go(dlg.GetPageNo());
		Invalidate();
	}
}

void CPdfViewerView::UpdateScrollSizes()
{
	CSize sizeTotal;
	sizeTotal.cx = GetWidth();
	sizeTotal.cy = GetHeight();
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CPdfViewerView::UpdateScrollPos()
{
	int w = GetWidth();
	int h = GetHeight();
	RECT r = GetHitRect();
	RECT clr;
	GetClientRect(&clr);
	if (w > clr.bottom || h > clr.right)
	{
		POINT pt;	
		pt.x = 0;//(int)(r.right * (double)clr.right / (double)w);
		//int bottom =  (int)(r.bottom * (double)clr.bottom / (double)h);
		//int top = (int)(r.top * (double)clr.bottom / (double)h);
		pt.y = r.top;

		ScrollToPosition(pt);
	}
}

void CPdfViewerView::OnZoomIn()
{
	ZoomIn();
	UpdateScrollSizes();
	Invalidate();	
}

void CPdfViewerView::OnZoomOut()
{
	ZoomOut();
	UpdateScrollSizes();
	Invalidate();
}

void CPdfViewerView::OnOriginalSize()
{
	OriginalSize();
	UpdateScrollSizes();
	Invalidate();
}


void RecursiveDeleteDirectory(CString szPath)
{
	CFileFind ff;
	CString path = szPath;
	
	if(path.Right(1) != "\\")
		path += "\\";

	path += "*.*";

	BOOL res = ff.FindFile(path);

	while(res)
	{
		res = ff.FindNextFile();
		if (!ff.IsDots() && !ff.IsDirectory())
			DeleteFile(ff.GetFilePath());
		else if (!ff.IsDots() && ff.IsDirectory())
		{
			path = ff.GetFilePath();
			RecursiveDeleteDirectory(path);
			RemoveDirectory(path);
		}
	}
	RemoveDirectory(szPath);
}

void CPdfViewerView::OnSaveBitmaps()
{
	CWaitCursor wait;
	RecursiveDeleteDirectory("bitmaps");
	CreateDirectory("bitmaps", NULL);
	int tot = TotalPages();
	if (tot)
	{
		StartGrab();
		for ( int i = 1; i <= TotalPages(); i++)
		{
			CImage img;
			HBITMAP hbmp = GetHBITMAP(i);
			img.Attach(hbmp);
			CString bmpfile;		
			bmpfile.Format("bitmaps\\img%d.bmp", i);
			img.Save(bmpfile);		
			DeleteObject(hbmp);
		}
		StopGrab();
		MessageBox("Saving bitmaps completed");
	}
}

void CPdfViewerView::OnSearch()
{
	CDlgSearch dlg;
	if (dlg.DoModal() == IDOK)
	{
		CWaitCursor wait;
		int n = SearchText(dlg.GetSearchStr().GetBuffer()) ;
		Invalidate();
		UpdateScrollPos();
		if (n == -1)
			MessageBox("Text not found");
		/*else
		{
			CString msg;
			msg.Format("Text found on page %d", n);
			MessageBox(msg);
		}*/	
	}
}

void CPdfViewerView::OnSearchNext()
{
	CWaitCursor wait;
	if (SearchNext() == -1)
		MessageBox("Text not found");
	else
	{
		Invalidate();
		UpdateScrollPos();
	}
}

void CPdfViewerView::OnSearchPrev()
{
	CWaitCursor wait;
	if (SearchPrevious() == -1)
		MessageBox("Text not found");
	else
	{
		Invalidate();
		UpdateScrollPos();
	}
}

void CPdfViewerView::OnInfo()
{
	char info[MAX_PATH];
	sprintf(info, "Author = %s\nKeyword = %s\nTitle = %s\nSubject = %s\nProducer = %s\nCreation date = %s\nModify date = %s\nIs encrypted = %d\nVersion number = %.1f\n "
					, PDFGetAuthor(), PDFGetKeyword(), PDFGetTitle(), PDFGetSubject(), PDFGetProducer(), PDFGetCreationDate(), PDFGetModifyDate(), PDFIsEncrypted(), PDFGetVersionNo());

	MessageBox(info);
}


void CPdfViewerView::OnOpenEncrypted()
{
	CDlgPassword dlg;
	if (dlg.DoModal() == IDOK)
	{
		CPdfViewerDoc *doc = (CPdfViewerDoc *)GetDocument();
		doc->SetPassword(dlg.GetPasswordStr());
		if (AfxGetApp()->m_pDocManager)
			AfxGetApp()->m_pDocManager->OnFileOpen();
	}		
}

void CPdfViewerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	sizeTotal.cx = 0;
	sizeTotal.cy = 0;
	SetScrollSizes(MM_TEXT, sizeTotal);
}



void CPdfViewerView::OnTimer(UINT_PTR nIDEvent)
{
	CScrollView::OnTimer(nIDEvent);
}
