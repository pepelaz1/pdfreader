// dlgpassword.cpp : implementation file
//

#include "stdafx.h"
#include "pdfviewer.h"
#include "dlgpassword.h"
#include "afxdialogex.h"


// CDlgPassword dialog

IMPLEMENT_DYNAMIC(CDlgPassword, CDialogEx)

CDlgPassword::CDlgPassword(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPassword::IDD, pParent)
{

}

CDlgPassword::~CDlgPassword()
{
}

void CDlgPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PASSWORD, m_edtPassword);
}


BEGIN_MESSAGE_MAP(CDlgPassword, CDialogEx)
	ON_BN_CLICKED(IDC_OK, &CDlgPassword::OnBnClickedOk)
END_MESSAGE_MAP()


CString &CDlgPassword::GetPasswordStr()
{
	return m_password_str;
}

void CDlgPassword::OnBnClickedOk()
{
	m_edtPassword.GetWindowTextA(m_password_str);
	EndDialog(!m_password_str.IsEmpty());
}
