//----------------------------------------------------------------
//- Project:           MPO-Verlagsloesung
//-                    DependencyLogic
//----------------------------------------------------------------
//- Copyright:         Copyright (C) 1999, 2000, 2001 MPO Leipzig
//-                    Alle Rechte vorbehalten.
//-
//-                    Media Print & Online Systeme GmbH
//-                    Atriumstrasse 1
//-                    D-04315 Leipzig
//-                    Tel. +49 (0)341 - 92223 - 0
//----------------------------------------------------------------
//- Filename:          StdAfx.h
//- Author:            $Author: Integrator $
//- Version:           $Revision: 1.1 $
//- Release:           $Name: 17-INT 17-0 16-INT 16-0 15-INT 15-0 14-INT 14-0 13-INT 13-0 12-INT 12-0 11-INT 11-0 10-INT 10-0 8-INT 8-0 7-INT 7-0 5-INT 5-0 4-INT 4-0 0-0 $
//- CHANGE LOG (Letzte Aenderung $Date: 2008/09/01 17:32:41Z $):
//-  Ver.    Datum     Kommentar
//-  ----    --------  ---------
//----------------------------------------------------------------
//- Description:
//----------------------------------------------------------------
//----------------------------------------------------------------
//- Klasse: .
//----------------------------------------------------------------

#if !defined(AFX_STDAFX_H__A1005535_6FDC_4cd6_9184_541BEF0D8C29__INCLUDED_)
#define AFX_STDAFX_H__A1005535_6FDC_4cd6_9184_541BEF0D8C29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

#include "../include/resource.h"

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <shlwapi.h>
#include <dbghelp.h>

#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <locale>
#include <string>
#include <memory>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE-Klassen
#include <afxodlgs.h>       // MFC OLE-Dialogfeldklassen
#include <afxdisp.h>        // MFC Automatisierungsklassen
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC-Datenbankklassen
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO-Datenbankklassen
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für gängige Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#include <set>
#include <string>
#include <vector>
#include <list>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

using namespace boost;

#include "../GlobExport/msg.hpp"
#include "../GlobExport/progressdlg.hpp"
#include "../GlobExport/subject.h"

#endif // !defined(AFX_STDAFX_H__A1005535_6FDC_4cd6_9184_541BEF0D8C29__INCLUDED_)
