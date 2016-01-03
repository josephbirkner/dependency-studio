// ProgressDlg.cpp: Implementierung der Klasse CProgressDlg.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "../GlobExport/progressdlg.hpp"

#include <shlobj.h>
#include <shlguid.h>
#include <stddef.h>
#include <crtdbg.h>
#include <atlconv.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CProgressDlg::CProgressDlg()
: m_bValid ( false )
, m_bDlgVisible ( false )
, m_dwDlgFlags ( PROGDLG_NORMAL )
, m_dwLastMaxProgress (0)
, m_u64LastMaxProgress (0)
, m_pIDlg ( NULL )
{
	HRESULT hr;

    hr = CoCreateInstance ( CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER,
                            IID_IProgressDialog, (void**) &m_pIDlg );

    if ( SUCCEEDED(hr) )
        m_bValid = true;
}

CProgressDlg::~CProgressDlg()
{
    if ( m_bValid )
    {
		if ( m_bDlgVisible )
		  m_pIDlg->StopProgressDialog();

		m_pIDlg->Release();
    }
}

void CProgressDlg::SetTitle ( LPCTSTR szTitle )
{
    USES_CONVERSION;

    _ASSERT (  szTitle );
    _ASSERT ( m_bValid );

    m_pIDlg->SetTitle ( T2COLE(szTitle) );
}

void CProgressDlg::SetAnimation ( HINSTANCE hinst, UINT uRsrcID )
{
    _ASSERT ( m_bValid );

    m_pIDlg->SetAnimation ( hinst, uRsrcID );
}

void CProgressDlg::SetCancelMessage ( LPCTSTR szMessage )
{
    USES_CONVERSION;

    _ASSERT ( m_bValid );
    _ASSERT (  szMessage );

    m_pIDlg->SetCancelMsg ( T2COLE(szMessage), NULL );
}

void CProgressDlg::SetCalculateTime ( bool bCalculate /*=true*/ )
{
    m_dwDlgFlags &= ~(PROGDLG_NOTIME | PROGDLG_AUTOTIME);

    if ( bCalculate )
        m_dwDlgFlags |= PROGDLG_AUTOTIME;
    else
        m_dwDlgFlags |= PROGDLG_NOTIME;
}

void CProgressDlg::SetAllowMinimize ( bool bAllow /*=true*/ )
{
    if ( bAllow )
        m_dwDlgFlags &= ~PROGDLG_NOMINIMIZE;
    else
        m_dwDlgFlags |= PROGDLG_NOMINIMIZE;
}

void CProgressDlg::SetShowProgressBar ( bool bShow /*=true*/ )
{
    if ( bShow )
        m_dwDlgFlags &= ~PROGDLG_NOPROGRESSBAR;
    else
        m_dwDlgFlags |= PROGDLG_NOPROGRESSBAR;
}

HRESULT CProgressDlg::ShowModal( HWND hWndParent )
{
	HRESULT hr;

    _ASSERT ( m_bValid );
    hr = m_pIDlg->StartProgressDialog ( hWndParent,
                                        NULL,
                                        m_dwDlgFlags | PROGDLG_MODAL,
                                        NULL );

    if ( SUCCEEDED(hr) )
        {
        m_bDlgVisible = true;
        }

    return hr;
}

HRESULT CProgressDlg::ShowModeless( HWND hWndParent )
{
	HRESULT hr;

    _ASSERT ( m_bValid );

    hr = m_pIDlg->StartProgressDialog ( hWndParent,
                                        NULL, m_dwDlgFlags, NULL );

    if ( SUCCEEDED(hr) )
        {
        m_bDlgVisible = true;
        }

    return hr;
}

void CProgressDlg::SetLineText ( DWORD   dwLine, 
                                   LPCTSTR szText,
                                   bool    bCompactPath /*=false*/ )
{
    USES_CONVERSION;

    _ASSERT (  szText );
    _ASSERT ( dwLine >= 1  &&  dwLine <= 3 );
    _ASSERT ( m_bValid );

    m_pIDlg->SetLine ( dwLine, T2COLE(szText), bCompactPath, NULL );
}

void CProgressDlg::UpdateProgress ( DWORD dwProgress, DWORD dwMax )
{
    _ASSERT ( m_bValid );
    _ASSERT ( 0 != dwMax );

    m_dwLastMaxProgress = dwMax;
    m_u64LastMaxProgress = 0;

    m_pIDlg->SetProgress ( dwProgress, dwMax );
}

void CProgressDlg::UpdateProgress ( DWORD dwProgress )
{
    _ASSERT ( m_bValid );
    _ASSERT ( 0 != m_dwLastMaxProgress );

    m_pIDlg->SetProgress ( dwProgress, m_dwLastMaxProgress );
}

void CProgressDlg::UpdateProgress ( ULONGLONG u64Progress,
                                      ULONGLONG u64ProgressMax )
{
    _ASSERT ( m_bValid );
    _ASSERT ( 0 != u64ProgressMax );

    m_dwLastMaxProgress = 0;
    m_u64LastMaxProgress = u64ProgressMax;

    m_pIDlg->SetProgress64 ( u64Progress, u64ProgressMax );
}

void CProgressDlg::UpdateProgress ( ULONGLONG u64Progress )
{
    _ASSERT ( m_bValid );
    _ASSERT ( 0 != m_u64LastMaxProgress );
    
    m_pIDlg->SetProgress64 ( u64Progress, m_u64LastMaxProgress );
}

bool CProgressDlg::HasUserCancelled()
{
	return 0 != m_pIDlg->HasUserCancelled();
}

void CProgressDlg::EndDialog()
{
	_ASSERT ( m_bValid );

	if ( m_bDlgVisible )
	{
//		HRESULT hRes = m_pIDlg->StopProgressDialog();
//++
		HWND hDlgWnd = NULL;
		//Sometimes the progress dialog sticks around after stopping it,
		//until the mouse pointer is moved over it or some other triggers.
		//This process finds the hwnd of the progress dialog and hides it
		//immediately.
		IOleWindow* pOleWindow;
		HRESULT hr = m_pIDlg->QueryInterface(IID_IOleWindow,(LPVOID*)&pOleWindow);
		if( SUCCEEDED(hr) )
        {
            hr = pOleWindow->GetWindow(&hDlgWnd);
            if( FAILED(hr) )
            {
                hDlgWnd = NULL;
            }
            pOleWindow->Release();
        }
		m_pIDlg->StopProgressDialog();
        if( hDlgWnd )
            ShowWindow(hDlgWnd, SW_HIDE);
//++
		m_bDlgVisible = false;
	}
}