
// pdfviewerview.h : interface of the CPdfViewerView class
//

#pragma once


class CPdfViewerView : public CScrollView 
{
protected: // create from serialization only
	CPdfViewerView();
	DECLARE_DYNCREATE(CPdfViewerView)

// Attributes
public:
	CPdfViewerDoc* GetDocument() const;

private:
	bool m_printing;
	void UpdateScrollSizes();
	void UpdateScrollPos();
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPagesCount();
	virtual void OnPrevPage();
	virtual void OnNextPage();
	virtual void OnGotoPage();
	virtual void OnZoomIn();
	virtual void OnZoomOut();
	virtual void OnOriginalSize();
	virtual void OnSaveBitmaps();
	virtual void OnSearch();
	virtual void OnSearchNext();
	virtual void OnSearchPrev();
	virtual void OnInfo();
	virtual void OnOpenEncrypted();

// Implementation
public:
	virtual ~CPdfViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

//private:
	//void Blit(CPdfViewerDoc* doc, CDC *pdc);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // debug version in pdfviewerview.cpp
inline CPdfViewerDoc* CPdfViewerView::GetDocument() const
   { return reinterpret_cast<CPdfViewerDoc*>(m_pDocument); }
#endif

