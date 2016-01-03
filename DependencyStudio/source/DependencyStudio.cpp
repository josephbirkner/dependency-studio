// DependencyStudio.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "resource.h"
#pragma hdrstop

#include "DependencyStudio.h"
#include "CDependencyStudio.hpp"

#include <xercesc/util/PlatformUtils.hpp>

using namespace xercesc;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

CProgressDlg* CDependencyStudioApp::s_pProgressDlg = NULL;

bool SystemCreateProgressCallback(unsigned int iDone, unsigned int iCount, std::string sCurrentModule)
{
	CDependencyStudioApp::s_pProgressDlg->SetLineText( 1, CFMsg(
		IDS_CREATEDB_DONECOUNT,
		iDone + 1, iCount ),
		false
	);
	CDependencyStudioApp::s_pProgressDlg->SetLineText( 2, CFMsg(
		IDS_CREATEDB_PROCESSINGMODULE,
		boost::filesystem::path(sCurrentModule).filename().string().c_str()),
		false
	);
	CDependencyStudioApp::s_pProgressDlg->UpdateProgress((DWORD) iDone, (DWORD) iCount);
	return !CDependencyStudioApp::s_pProgressDlg->HasUserCancelled();
}

/////////////////////////////////////////////////////////////////////////////
// CDependencyStudioApp

BEGIN_MESSAGE_MAP(CDependencyStudioApp, CWinApp)
	//{{AFX_MSG_MAP(CDependencyStudioApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDependencyStudioApp construction

CDependencyStudioApp::CDependencyStudioApp() : m_nExitCode(0)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDependencyStudioApp object

CDependencyStudioApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDependencyStudioApp initialization

#define SWITCH(_type, _caseclass) {_type __caseclass=_caseclass; if(0){
#define CASE(__case) } else if(__caseclass == __case) {
#define DEFAULT } else {
#define END_SWITCH }}

BOOL CDependencyStudioApp::InitInstance()
{
	AfxEnableControlContainer();
	AfxInitRichEdit2();
	CoInitialize(NULL);
	XMLPlatformUtils::Initialize();
	
	CDependencyStudioCmdLineInfo cmdLineInfo;
	ParseCommandLine(cmdLineInfo);
	
	if( cmdLineInfo.getXmlPath() == "?" )
	{
		OPENFILENAME ofn;
		ofn.hInstance = AfxGetInstanceHandle();
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.Flags = OFN_EXPLORER;
		ofn.lpstrFilter = "XML Database\0*.xml\0\0";
		
		if(GetOpenFileName(&ofn))
			cmdLineInfo.getXmlPath() = ofn.lpstrFile;
		else
		{
			DebugBreak();
			cmdLineInfo.getXmlPath() = "";
		}
	}
	
	do
	{
		System* pSystem = NULL;
		bool bExists = PathFileExists(cmdLineInfo.getXmlPath().c_str()) != FALSE;
		
		if(cmdLineInfo.getCommand() == "create")
		{
			if(bExists && MessageBox( NULL, CFMsg(IDS_CREATEDB_OVERWRITE, cmdLineInfo.getXmlPath().c_str()), CMsg(IDS_CREATEDB), MB_OKCANCEL) == IDCANCEL)
				break;
			
			std::string sName = cmdLineInfo.getArgs().front();
			cmdLineInfo.getArgs().pop_front();
			
			s_pProgressDlg = new CProgressDlg();
			s_pProgressDlg->ShowModal(NULL);
			s_pProgressDlg->SetTitle(CFMsg(IDS_CREATEDB_PROGRESSDLGTITLE, cmdLineInfo.getXmlPath().c_str()));
			
			pSystem = new System(
				sName,
				std::vector<std::string>(cmdLineInfo.getArgs().begin(), cmdLineInfo.getArgs().end()),
				"",
				SystemCreateProgressCallback
			);
			
			pSystem->archive(cmdLineInfo.getXmlPath());
			break;
		}
		else if(bExists)
			pSystem = Reader(cmdLineInfo.getXmlPath());
		
		if(cmdLineInfo.getCommand().empty())
		{
			HICON icon = LoadIcon(IDI_LOGO_SMALL);
			CDependencyStudio dlg(NULL, pSystem, cmdLineInfo.getXmlPath(), icon);
			m_pMainWnd = &dlg;
			dlg.DoModal();
		}
		else if(pSystem)
		{
			int nPatchMode = 0;
			bool bOnlyStoreIfSuccessfull = false;
			
			SWITCH(std::string, cmdLineInfo.getCommand())
				CASE("update")
					nPatchMode = PatchTest;
				CASE("updatesilent")
					nPatchMode = PatchTestSilent;
				CASE("updatesilent_ifsuccessfull")
					nPatchMode = PatchTestSilent;
					bOnlyStoreIfSuccessfull = true;
				CASE("resolve_with_other_system")
					nPatchMode = TestWithOtherSystem;
			END_SWITCH
			
			if(nPatchMode > 0)
			{
				std::string sLog = cmdLineInfo.getArgs().front();
				cmdLineInfo.getArgs().pop_front();
				Patch patch((PatchMode) nPatchMode, std::vector<std::string>(cmdLineInfo.getArgs().begin(), cmdLineInfo.getArgs().end()), pSystem, sLog);
				m_nExitCode = patch.execute();
				if(!(bOnlyStoreIfSuccessfull && m_nExitCode > 0) && nPatchMode != TestWithOtherSystem)
					pSystem->archive(cmdLineInfo.getXmlPath());
			}
		}
	}
	while(false);
	
	if(!cmdLineInfo.getCommand().empty())
		return FALSE;
		
	return TRUE;
}


int CDependencyStudioApp::ExitInstance()
{
	if(s_pProgressDlg)
		delete s_pProgressDlg;
	
	CoUninitialize();
	XMLPlatformUtils::Terminate();

	// Speicherlochwarnungen auszublenden
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	
	CWinApp::ExitInstance();
	return m_nExitCode;
}
