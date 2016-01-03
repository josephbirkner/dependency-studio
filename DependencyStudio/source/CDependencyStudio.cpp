
#include "StdAfx.h"

#pragma hdrstop

#include "../include/CDependencyStudio.hpp"

using namespace Layout;

/**
 * CDependencyStudio-Dialogfeld
 */
IMPLEMENT_DYNAMIC(CDependencyStudio, CDialog)

BEGIN_MESSAGE_MAP(CDependencyStudio, CDialog)
	ON_COMMAND(ID_LOAD_DATABASE, OnLoadDatabase)
	ON_COMMAND(ID_CLOSE_DATABASE, OnCloseDatabase)
	ON_COMMAND(ID_MODUL_PATCHTESTEN, OnTestPatch)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_USERHELP, OnUserHelp)
	ON_COMMAND(ID_RESULT_EXPORT, OnExportResult)
	
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_SUBJECTS, &CDependencyStudio::OnDoubleclickSelTree)
	ON_NOTIFY(NM_CLICK, IDC_TREE_SUBJECTS, &CDependencyStudio::OnClickSelTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SUBJECTS, &CDependencyStudio::OnSelChangeSelTree)
	
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CDependencyStudio::OnClickSearch)
	
	ON_EN_CHANGE(IDC_EDIT_QUERY, &CDependencyStudio::OnEnChangeSearch)
END_MESSAGE_MAP()

//////////////////////////////////////////
// 
//   Global Dialog Functionality
//

/**
 * CDependencyStudio::CDependencyStudio
 */
CDependencyStudio::CDependencyStudio( HWND pParent, System* pSystem, std::string sPath, HICON icon ) :
	CDialog( CDependencyStudio::IDD, CWnd::FromHandle(pParent) ),
	Layout::Owner("DependencyStudio", Layout::ProfileGlobal, CSize(0, 0), CSize(2000, 2000)),
	m_bIsDirty(false),
	m_pSystem(pSystem),
	m_sCurrentPath(sPath),
	m_pCurrentQuery(NULL),
	m_pSelQuerySelectionNode(NULL),
	m_nRecentListLength(0),
	m_icon(icon)
{
	m_nRecentListLength = Config().getRecentListMaxLength();
}
	
/**
 * CDependencyStudio::~CDependencyStudio
 */
CDependencyStudio::~CDependencyStudio()
{
}

void CDependencyStudio::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_SUBJECTS, m_treeQuerySelection);
	DDX_Control(pDX, IDC_TREE_RESULTS, m_treeResults);
	DDX_Control(pDX, IDC_EDIT_QUERY, m_editQuery);
	DDX_Control(pDX, IDC_CHECK_CASESENSITIVE, m_checkCaseSensitive);
	DDX_Control(pDX, IDC_CHECK_ONLYWORDS, m_checkOnlyWords);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_btQuery);
	DDX_Control(pDX, IDC_GROUP_SEARCH_PARAMS, m_groupSearchParams);
}

/**
 * CDependencyStudio::OnInitDialog
 */
BOOL CDependencyStudio::OnInitDialog()
{
	CDialog::OnInitDialog();
	initLayout(GetSafeHwnd());
	deactivateControls();
	updateRecentMenu();
	SetIcon(m_icon, TRUE);
	
	return TRUE;
}

/**
 * CDependencyStudio::OnCmdMsg
 */
BOOL CDependencyStudio::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
	if( !pHandlerInfo && nID <= ID_ALLOCDYNAMICMAX && nID > (ID_ALLOCDYNAMICMAX - m_nRecentListLength) && nCode == CN_COMMAND )
	{
		int nDiff = ID_ALLOCDYNAMICMAX - nID;
		std::string sPath = Config().getRecentDatabases()[nDiff];
		setSystem(NULL, sPath);
		return TRUE;
	}
	else
		return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/**
 * CDependencyStudio::doLayoutDataExchange
 */
void CDependencyStudio::doLayoutDataExchange()
{
	// Add all controls to the layout
	LAYOUT_CONTROL_VSHS(IDC_GROUP_QUERY);
	LAYOUT_CONTROL_VSHS(IDC_TREE_SUBJECTS);
	
	LAYOUT_CONTROL_VSHS(IDC_GROUP_SEARCH_PARAMS);
	LAYOUT_CONTROL_THS(IDC_EDIT_QUERY);
	LAYOUT_CONTROL_THS(IDC_CHECK_ONLYWORDS);
	LAYOUT_CONTROL_THS(IDC_CHECK_CASESENSITIVE);
	LAYOUT_CONTROL_BHS(IDC_BUTTON_SEARCH);
	
	LAYOUT_CONTROL_VSHS(IDC_TREE_RESULTS);
	
	// Initialize the layout area properties
	int nStyle = AreaStyleDrawBk|AreaStyleHover|AreaStyleDrawTitle|AreaStyleHoverTitle;
	AreaColorScheme aColor(
		RGB(227, 227, 227), // Bk
		RGB(137, 207, 254), // Hover
		RGB(140, 140, 140), // Title
		RGB(30, 30, 30) );  // Title Hover
	AreaProperties aParams(IDC_GROUP_SEARCH_PARAMS, nStyle|AreaStyleFoldable, aColor, "SUCHPARAMETER");
	AreaProperties aSubjects(IDC_GROUP_QUERY, nStyle, aColor, "SUBJEKTE");
	AreaProperties aResults(IDC_TREE_RESULTS, nStyle, aColor, "SUCHERGEBNIS");
	
	// Create splitters between the areas
	putSplitter(aSubjects, aResults, Splitter::Vertical, Splitter::AlignRelative);
	putSplitter(aSubjects, aParams, Splitter::Horizontal, Splitter::AlignLow);
}

void CDependencyStudio::OnLoadDatabase()
{
	CFileDialog fdlgXmlPath(true,NULL,NULL,FALSE,"",this);
	fdlgXmlPath.DoModal();

	if(!fdlgXmlPath.GetPathName().IsEmpty())
		setSystem(NULL, (const char*) fdlgXmlPath.GetPathName());
}

void CDependencyStudio::OnCloseDatabase()
{
	setSystem(NULL, "");
}

//////////////////////////////////////////
// 
//    Small-ish helper functions
//

void CDependencyStudio::setSystem(System* pSystem, std::string sXmlPath)
{
	if( m_pSystem )
	{
		if(boost::iequals(sXmlPath, m_sCurrentPath))
			if(MessageBox(CFMsg(IDS_RELOAD_DATABASE, m_sCurrentPath.c_str()), CMsg(IDS_WARNING), MB_YESNO|MB_ICONEXCLAMATION) == IDNO)
				return;
		
		if(Config().getAllowWrite() && isDirty())
		{
			switch(MessageBox(CMsg(IDS_UNSAVED_CHANGES), CMsg(IDS_SAVE_BEFORE_CLOSE), MB_YESNOCANCEL))
			{
			case IDCANCEL:
				if(pSystem)
				{
					delete pSystem;
					deactivateControls();
				}
				return;
			case IDOK:
				pSystem->archive(m_sCurrentPath);
			}
		}
		delete m_pCurrentQuery;
		delete m_pSystem;
		m_pCurrentQuery = NULL;
	}
	
	// Load database if not loaded yet
	if(!pSystem && !sXmlPath.empty())
	{
		Reader* pReader = new Reader(sXmlPath);
		pSystem = *pReader;
		delete pReader;
		this->updateRecentMenu();
	}

	m_pSystem = pSystem;
	m_sCurrentPath = sXmlPath;
	m_bIsDirty = false;

	if(m_pSystem)
		resetQueryProperties();
	else
		deactivateControls();
}

void CDependencyStudio::resetQueryProperties()
{
	AFXASSUME(m_pSystem != NULL);

	m_treeQuerySelection.EnableWindow(TRUE);
	m_editQuery.EnableWindow(TRUE);
	m_checkCaseSensitive.EnableWindow(TRUE);
	m_checkOnlyWords.EnableWindow(TRUE);
	m_checkCaseSensitive.EnableWindow(TRUE);
	m_btQuery.EnableWindow(TRUE);
	m_groupSearchParams.EnableWindow(TRUE);
	
	GetMenu()->EnableMenuItem(1, MF_ENABLED|MF_BYPOSITION);
	GetMenu()->EnableMenuItem(2, MF_ENABLED|MF_BYPOSITION);
	
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_SAVE_DATABASE, MF_ENABLED);
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_CLOSE_DATABASE, MF_ENABLED);
	
	SetWindowText(CFMsg(IDS_TITLE_LOADED, m_pSystem->getName().c_str(), m_sCurrentPath.c_str()));
	
	if(m_pCurrentQuery)
	{
		Query* pPrevQuery = m_pCurrentQuery;
		m_pCurrentQuery = new Query(*pPrevQuery);
		pPrevQuery->setSelTree(NULL);
	}
	else
		m_pCurrentQuery = new Query(&m_treeQuerySelection, m_pSystem);
	
	m_pSelQuerySelectionNode = NULL;
}

void CDependencyStudio::deactivateControls()
{
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_SAVE_DATABASE, MF_GRAYED);
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_CLOSE_DATABASE, MF_GRAYED);
	
	GetMenu()->GetSubMenu(1)->EnableMenuItem(0, MF_GRAYED|MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->EnableMenuItem(1, MF_GRAYED|MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->EnableMenuItem(2, MF_GRAYED|MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->EnableMenuItem(3, MF_GRAYED|MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->EnableMenuItem(4, MF_GRAYED|MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->EnableMenuItem(5, MF_GRAYED|MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->EnableMenuItem(6, MF_GRAYED|MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->EnableMenuItem(7, MF_GRAYED|MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->EnableMenuItem(8, MF_GRAYED|MF_BYPOSITION);
	GetMenu()->GetSubMenu(1)->EnableMenuItem(9, MF_GRAYED|MF_BYPOSITION);
	
	GetMenu()->GetSubMenu(2)->EnableMenuItem(ID_MODUL_PATCHEINSPIELEN, MF_GRAYED);
	GetMenu()->GetSubMenu(2)->EnableMenuItem(ID_MODUL_HINZUF32783, MF_GRAYED);
	GetMenu()->GetSubMenu(2)->EnableMenuItem(ID_MODUL_ENTFERNEN, MF_GRAYED);
	GetMenu()->GetSubMenu(2)->EnableMenuItem(ID_MODUL_AKTUALISIEREN, MF_GRAYED);
	
	GetMenu()->EnableMenuItem(1, MF_GRAYED|MF_BYPOSITION);
	GetMenu()->EnableMenuItem(2, MF_GRAYED|MF_BYPOSITION);
	
	m_treeQuerySelection.EnableWindow(FALSE);
	m_editQuery.EnableWindow(FALSE);
	m_checkCaseSensitive.EnableWindow(FALSE);
	m_checkOnlyWords.EnableWindow(FALSE);
	m_checkCaseSensitive.EnableWindow(FALSE);
	m_btQuery.EnableWindow(FALSE);
	m_treeResults.EnableWindow(FALSE);
	m_groupSearchParams.EnableWindow(FALSE);
	
	m_editQuery.SetWindowText("");
	m_checkOnlyWords.SetCheck(FALSE);
	m_checkCaseSensitive.SetCheck(FALSE);
}

void CDependencyStudio::updateRecentMenu()
{
	CMenu* pMenu = GetMenu()->GetSubMenu(0)->GetSubMenu(1);
	
	AFXASSUME(pMenu);
	
	while(pMenu->GetMenuItemCount() > 0)
		pMenu->RemoveMenu(0, MF_BYPOSITION);
		
	int nMenuID = 0;
	
	for each(std::string sRecent in Config().getRecentDatabases())
		pMenu->AppendMenu(MF_STRING, (UINT_PTR) (ID_ALLOCDYNAMICMAX - nMenuID++ ), sRecent.data());
}

//////////////////////////////////////////
// 
//    Selection Tree Notifications
//

void CDependencyStudio::OnDoubleclickSelTree( NMHDR *pNMHDR, LRESULT *pResult )
{
	NMTREEVIEW *pNM = (NMTREEVIEW *)pNMHDR;
	UINT nFlags;
	CPoint pt(::GetMessagePos());
	m_treeQuerySelection.ScreenToClient(&pt);
	
	HTREEITEM hItem = m_treeQuerySelection.HitTest(pt, &nFlags);

	if( hItem && !(nFlags & TVHT_ONITEMSTATEICON) )
		reinterpret_cast<QuerySelectionNode*>(m_treeQuerySelection.GetItemData(hItem))->createSubnodes();
}

void CDependencyStudio::OnSelChangeSelTree( NMHDR *pNMHDR, LRESULT *pResult )
{
	m_pSelQuerySelectionNode = (QuerySelectionNode*) m_treeQuerySelection.GetItemData(reinterpret_cast<LPNMTREEVIEW>(pNMHDR)->itemNew.hItem);
	
	if(m_pSelQuerySelectionNode && m_pSelQuerySelectionNode->getParent())
	{
		m_editQuery.EnableWindow();
		m_checkOnlyWords.EnableWindow();
		m_checkCaseSensitive.EnableWindow();
		
		m_editQuery.SetWindowText(m_pSelQuerySelectionNode->getSearchTerm().c_str());
		m_checkOnlyWords.SetCheck(m_pSelQuerySelectionNode->getSearchWord() ? TRUE : FALSE);
		m_checkCaseSensitive.SetCheck(m_pSelQuerySelectionNode->getConsiderCase() ? TRUE : FALSE);
		m_groupSearchParams.SetWindowText(CFMsg(IDS_QUERY_PARAM_GROUP_TITLE, m_pSelQuerySelectionNode->getTitle().c_str()));
	}
	else
	{
		m_editQuery.SetWindowText("");
		m_checkOnlyWords.SetCheck(FALSE);
		m_checkCaseSensitive.SetCheck(FALSE);
		m_editQuery.EnableWindow(FALSE);
		m_checkOnlyWords.EnableWindow(FALSE);
		m_checkCaseSensitive.EnableWindow(FALSE);
	}
}

void CDependencyStudio::OnClickSelTree( NMHDR *pNMHDR, LRESULT *pResult )
{
	NMTREEVIEW *pNM = (NMTREEVIEW *)pNMHDR;
	UINT nFlags;
	CPoint pt(::GetMessagePos());
	m_treeQuerySelection.ScreenToClient(&pt);

	HTREEITEM hItem = m_treeQuerySelection.HitTest(pt, &nFlags);
	
	if( hItem )
		if( nFlags & TVHT_ONITEMSTATEICON )
			reinterpret_cast<QuerySelectionNode*>(m_treeQuerySelection.GetItemData(hItem))->setSelected(!(m_treeQuerySelection.GetCheck(hItem) == TRUE), false);
}

void CDependencyStudio::OnClickSearch()
{
	GetMenu()->GetSubMenu(1)->EnableMenuItem(0, MF_ENABLED|MF_BYPOSITION);
	
	if( m_pCurrentQuery != NULL )
	{
		if(m_pSelQuerySelectionNode)
		{
			m_pSelQuerySelectionNode->setSearchConsiderCase(m_checkCaseSensitive.GetCheck() == BST_CHECKED);
			m_pSelQuerySelectionNode->setSearchOnlyWords(m_checkOnlyWords.GetCheck() == BST_CHECKED);
		}
		m_pCurrentQuery->execute();
		m_treeResults.setQuery(m_pCurrentQuery);
		resetQueryProperties();
	}
}

void CDependencyStudio::OnEnChangeSearch()
{
	if( m_pSelQuerySelectionNode )
	{
		CString szText;
		m_editQuery.GetWindowText(szText);
		m_pSelQuerySelectionNode->setSearchTerm((const char*) szText);
		m_pSelQuerySelectionNode->setSearchConsiderCase( m_checkCaseSensitive.GetCheck() == TRUE );
		m_pSelQuerySelectionNode->setSearchOnlyWords( m_checkOnlyWords.GetCheck() == TRUE );
	}
}

void CDependencyStudio::OnTestPatch()
{
	TCHAR vchFileNameBuffer[32780] = {0}; // reasonable guess for 256 files with 128 characters per path

	CFileDialog fdlgPathes(true, "dll", NULL, OFN_ALLOWMULTISELECT, "DLL Dateien (*.dll)|*.dll|EXE Dateien (*.exe)|*.exe|Alle Dateien|*.*||", this);
	fdlgPathes.m_ofn.lpstrFile = vchFileNameBuffer;
	fdlgPathes.m_ofn.nMaxFile = 32780;
	fdlgPathes.DoModal();
	
	std::vector<std::string> vsPathes;
	POSITION p = fdlgPathes.GetStartPosition();
	
	while(p != NULL)
	{
		CString szPath = fdlgPathes.GetNextPathName(p);
		if(!szPath.IsEmpty())
			vsPathes.push_back((const char*) szPath);
	}
	
	if(!vsPathes.empty())
		Patch(PatchTest, vsPathes, m_pSystem, Config().getLogFilePath());
}

void CDependencyStudio::OnAbout()
{
	MessageBox(CFMsg(IDS_ABOUT_TEXT), CFMsg(IDS_ABOUT), MB_OK|MB_ICONINFORMATION);
}

void CDependencyStudio::OnUserHelp()
{
	STARTUPINFO si;
	::ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;
		
	::CreateProcess(
		NULL,
		"cmd.exe /c start .\\help.pdf",
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&si,
		&pi
	);
}

void CDependencyStudio::OnExportResult()
{
	CFileDialog fdlgPathes(false, "txt", NULL, FALSE, "Textdateien (*.txt)|*.txt|Alle Dateien|*.*||", this);
	fdlgPathes.DoModal();
	
	CString szPath = fdlgPathes.GetPathName();
	
	m_treeResults.getQuery()->exportResults((const char*) szPath);
}

BOOL CDependencyStudio::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE))
			pMsg->wParam = VK_TAB;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}