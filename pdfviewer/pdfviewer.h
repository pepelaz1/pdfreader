
// pdfviewer.h : main header file for the pdfviewer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPdfViewerApp:
// See pdfviewer.cpp for the implementation of this class
//

class CPdfViewerApp : public CWinApp
{
public:
	CPdfViewerApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPdfViewerApp theApp;
