#pragma once
#include "resource.h"
#include "afxwin.h"

// CDlgGo dialog

class CDlgGo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGo)

public:
	CDlgGo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgGo();

// Dialog Data
	enum { IDD = IDD_GOPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtngo();
	CEdit m_edtPage;
	int GetPageNo();
private:
	int m_pageno;
};
