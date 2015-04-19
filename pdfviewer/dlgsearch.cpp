// dlgsearch.cpp : implementation file
//

#include "stdafx.h"
#include "pdfviewer.h"
#include "dlgsearch.h"
#include "afxdialogex.h"


// CDlgSearch dialog

IMPLEMENT_DYNAMIC(CDlgSearch, CDialogEx)

CDlgSearch::CDlgSearch(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSearch::IDD, pParent)
{

}

CDlgSearch::~CDlgSearch()
{
}

void CDlgSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDTSEARCH, m_edtSearch);
}


BEGIN_MESSAGE_MAP(CDlgSearch, CDialogEx)
	ON_BN_CLICKED(IDC_BTNSEARCH, &CDlgSearch::OnBnClickedBtnsearch)
END_MESSAGE_MAP()


// CDlgSearch message handlers
CString &CDlgSearch::GetSearchStr()
{
	return m_search_str;
}

void CDlgSearch::OnBnClickedBtnsearch()
{
	m_edtSearch.GetWindowTextA(m_search_str);
	EndDialog(!m_search_str.IsEmpty());
}
