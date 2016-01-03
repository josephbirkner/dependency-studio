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
//- Filename:          KommerzMFCUtil.h
//- Author:            $Author: BKI $
//- Version:           $Revision: 1.2 $
//- Release:           $Name: 17-INT 17-0 16-INT 16-0 15-INT 15-0 14-INT 14-0 13-INT 13-19 OPS732623 $
//- CHANGE LOG (Letzte Aenderung $Date: 2009/07/10 10:23:25Z $):
//-  Ver.    Datum     Kommentar
//-  ----    --------  ---------
//----------------------------------------------------------------
//- Description:
//----------------------------------------------------------------
//----------------------------------------------------------------
//- Klasse: CKommerzMFCUtilApp
//----------------------------------------------------------------


#ifndef _DEPENDENCYLOGIC_
#define _DEPENDENCYLOGIC_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

//#include <vld.h>

/**
 * CWinApp Specialization that defines this DLLs DLLMain() function.
 */
class CDependencyLogicApp : public CWinApp
{
public:
	CDependencyLogicApp();
	
	/** The one and only CDependencyLogicApp instance.*/
	static CDependencyLogicApp theApp;
	
	DECLARE_MESSAGE_MAP()
};


#endif // _DEPENDENCYLOGIC_
