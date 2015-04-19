#pragma once
#include "resource.h"
#include "afxwin.h"

// CDlgSearch dialog

class CDlgSearch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSearch)

public:
	CDlgSearch(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSearch();

// Dialog Data
	enum { IDD = IDD_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnsearch();
	CEdit m_edtSearch;

	CString &GetSearchStr();
private:
	CString m_search_str;

};
