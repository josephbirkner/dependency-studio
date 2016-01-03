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
//- Filename:          DependencyLogic.cpp
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


#include "stdafx.h"
#pragma hdrstop

#include "contrib\DependencyControl\DependencyLogic\GlobExport\dependencylogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Beachten Sie!
//
//		Wird diese DLL dynamisch an die MFC-DLLs gebunden,
//		muss bei allen von dieser DLL exportierten Funktionen,
//		die MFC-Aufrufe durchführen, das Makro AFX_MANAGE_STATE
//		direkt am Beginn der Funktion eingefügt sein.
//
//		Beispiel:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//		// Hier normaler Funktionsrumpf
//		}
//
//		Es ist sehr wichtig, dass dieses Makro in jeder Funktion
//		vor allen MFC-Aufrufen erscheint. Dies bedeutet, dass es
//		als erste Anweisung innerhalb der Funktion ausgeführt werden
//		muss, sogar vor jeglichen Deklarationen von Objektvariablen,
//		da ihre Konstruktoren Aufrufe in die MFC-DLL generieren
//		könnten.
//
//		Siehe MFC Technical Notes 33 und 58 für weitere
//		Details.
//

BEGIN_MESSAGE_MAP(CDependencyLogicApp, CWinApp)
END_MESSAGE_MAP()

CDependencyLogicApp::CDependencyLogicApp() : CWinApp("DependencyLogic")
{
}

CDependencyLogicApp CDependencyLogicApp::theApp;
