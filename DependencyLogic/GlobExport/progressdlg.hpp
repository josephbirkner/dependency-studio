// ProgressDlg.h: Schnittstelle für die Klasse CProgressDlg.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__PROGRESSDLG_H_INCLUDED__)
#define __PROGRESSDLG_H_INCLUDED__

#include "StdAfx.h"

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

struct IProgressDialog;

class DEPENDENCYLOGIC_API CProgressDlg
{
public:
	CProgressDlg();
	virtual ~CProgressDlg();

	void SetTitle ( LPCTSTR szTitle );
    void SetAnimation ( HINSTANCE hinst, UINT uRsrcID );
    void SetCancelMessage ( LPCTSTR szMessage );

    void SetLineText ( DWORD dwLine, LPCTSTR szText, bool bCompactPath = false );
    
    void SetCalculateTime ( bool bCalculate = true );
    void SetAllowMinimize ( bool bAllow = true );
    void SetShowProgressBar ( bool bShow = true );
    
    void ResetTimer();

    HRESULT ShowModal ( HWND hWndParent );
    HRESULT ShowModeless ( HWND hWndParent );

    void EndDialog();

    void UpdateProgress ( DWORD dwProgress, DWORD dwMax );
    void UpdateProgress ( DWORD dwProgress );

    void UpdateProgress ( ULONGLONG u64Progress, ULONGLONG u64ProgressMax );
    void UpdateProgress ( ULONGLONG u64Progress );

    bool HasUserCancelled();	

    bool IsValid() const { return m_bValid; }

protected:
    IProgressDialog* m_pIDlg;
    bool      m_bValid;
    bool      m_bDlgVisible;
    DWORD     m_dwDlgFlags;
    DWORD     m_dwLastMaxProgress;
    ULONGLONG m_u64LastMaxProgress;
};

#endif // !defined(AFX_PROGRESSDLG_H__3158B4ED_9B4E_4577_8741_B138213A4157__INCLUDED_)
