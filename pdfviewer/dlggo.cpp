// dlggo.cpp : implementation file
//

#include "stdafx.h"
#include "pdfviewer.h"
#include "dlggo.h"
#include "afxdialogex.h"


// CDlgGo dialog

IMPLEMENT_DYNAMIC(CDlgGo, CDialogEx)

CDlgGo::CDlgGo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgGo::IDD, pParent)
{
	m_pageno = 0;
}

CDlgGo::~CDlgGo()
{
}

void CDlgGo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDTGO, m_edtPage);
}


BEGIN_MESSAGE_MAP(CDlgGo, CDialogEx)
	ON_BN_CLICKED(IDC_BTNGO, &CDlgGo::OnBnClickedBtngo)
END_MESSAGE_MAP()


// CDlgGo message handlers


void CDlgGo::OnBnClickedBtngo()
{
	CString str;
	m_edtPage.GetWindowText(str);
	m_pageno = StrToInt(str);
	EndDialog(1);
}

int CDlgGo::GetPageNo()
{
	return m_pageno;
}