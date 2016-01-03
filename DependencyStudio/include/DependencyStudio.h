// DependencyStudio.h : main header file for the DependencyStudio application
//

#if !defined(__DEPENDENCYSTUDIO_APP_H__)
#define __DEPENDENCYSTUDIO_APP_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "StdAfx.h"

#include "DependencyStudioCmdLineInfo.h"

//#include <vld.h>

/////////////////////////////////////////////////////////////////////////////
// CDependencyStudioApp:
// See DependencyStudio.cpp for the implementation of this class
//

class CDependencyStudioApp : public CWinApp
{
public:
	CDependencyStudioApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDependencyStudioApp)
public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDependencyStudioApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CDependencyStudioCmdLineInfo m_CmdLineInfo;
	int m_nExitCode;
	
	static CProgressDlg* s_pProgressDlg;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(__DEPENDENCYSTUDIO_APP_H__)
