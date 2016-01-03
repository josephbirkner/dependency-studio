// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__8BFE31E8_0149_11D3_AAE8_00008329C597__INCLUDED_)
#define AFX_STDAFX_H__8BFE31E8_0149_11D3_AAE8_00008329C597__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include "resource.h"

#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <locale>
#include <set>

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <shlwapi.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "../../DependencyLogic/GlobExport/msg.hpp"
#include "../../DependencyLogic/GlobExport/progressdlg.hpp"

#include "../../DependencyLogic/GlobExport/subject.h"

#include "../../DependencyLogic/GlobExport/subject.h"
#include "../../DependencyLogic/GlobExport/config.h"
#include "../../DependencyLogic/GlobExport/dependency.h"
#include "../../DependencyLogic/GlobExport/system.h"
#include "../../DependencyLogic/GlobExport/module.h"
#include "../../DependencyLogic/GlobExport/export.h"
#include "../../DependencyLogic/GlobExport/reader.h"
#include "../../DependencyLogic/GlobExport/query.h"
#include "../../DependencyLogic/GlobExport/queryselectionnode.h"
#include "../../DependencyLogic/GlobExport/patch.h"

using namespace DependencyLogic;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__8BFE31E8_0149_11D3_AAE8_00008329C597__INCLUDED_)
