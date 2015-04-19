#pragma once
#include "afxwin.h"


// CDlgPassword dialog

class CDlgPassword : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPassword)

public:
	CDlgPassword(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPassword();

// Dialog Data
	enum { IDD = IDD_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtPassword;
	afx_msg void OnBnClickedOk();

	CString &GetPasswordStr();
private:
	CString m_password_str;
};
