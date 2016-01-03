/*     __                            __                               __          __
  ____/ /___  ____  ____  ____  ____/ /___  ____  _______  __  ______/ /_  ______/ /_____
 / __, / __ \/ __ \/ __ \/ ,_ \/ __, / __ \/ ,_ \/ ___/ / / / / ___\, / / / / __, /_/ __ \
/ /_/ / /_/ / /_/ / /_/ / / / / /_/ / /_/ / / / / /__/ /_/ / _\__, / / /_/ / /_/ / / /_/ /
\____/ ,___/ ,___/ ,___/\/ /_/\____/ ,___/\/ /_/\___/\__, /  \____/\/\___,_\____/\/\____/
     \____/\/    \____/            \____/               \/
    __  ____  ______    ____  ____    ____  ____________              __    __
   / /_/ / / / / __ \  / __ \/ ,__\  /__, \/ __ \, /__, \          __/ /   /_/
  / ,_, / / / / /_/ / / /_/ / / __  ____/ / / / / /___/ /         /_/ /_  / /
 / / / / /_/ / ,___/ / ,_, / /_/ / / ,___/ /_/ / / ,___/         / / ,_ \/ /
 \/  \/\____/\/      \/  \/\____/  \____/\____/_/\____/ /\   ___/ / /_/ /_/
                                                        '/   \___/\____/                 */
                                                        
#ifndef __DEPENDENCYSTUDIO_DIALOG_HPP__
#define __DEPENDENCYSTUDIO_DIALOG_HPP__


#include "StdAfx.h"

#include "resource.h"

#include "../../DependencyLogic/GlobExport/queryview.h"
#include "../../DependencyLogic/GlobExport/system.h"

#include "../../DynLayout/GlobExport/owner.h"

class CDependencyStudio : public CDialog, public Layout::Owner
{
	DECLARE_DYNAMIC(CDependencyStudio);

public:
	CDependencyStudio( HWND pParent, System* pSystem, std::string sXmlPath, HICON icon );
	virtual ~CDependencyStudio();
	
	enum { IDD = IDD_DEPENDENCYSTUDIO_DIALOG };
	
	System* getSystem() {return m_pSystem;}
	void setSystem(System* pSystem, std::string sXmlPath);
	bool isDirty() {return m_bIsDirty;}
	
protected:
	virtual void doLayoutDataExchange();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnInitDialog();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	DECLARE_MESSAGE_MAP()
	
	void OnLoadDatabase();
	void OnCloseDatabase();
	
	void OnTestPatch();
	void OnAbout();
	void OnUserHelp();
	void OnExportResult();
	
	void OnDoubleclickSelTree(NMHDR *pNMHDR, LRESULT *pResult);
	void OnSelChangeSelTree( NMHDR *pNMHDR, LRESULT *pResult );
	void OnClickSelTree( NMHDR *pNMHDR, LRESULT *pResult );
	void OnClickSearch();
	
	void OnEnChangeSearch();

private:
	CTreeCtrl m_treeQuerySelection;
	CEdit m_editQuery;
	CButton m_checkCaseSensitive;
	CButton m_checkOnlyWords;
	CButton m_btQuery;
	QueryView m_treeResults;
	CBitmap m_bmpCancelFilter;
	CStatic m_groupSearchParams;
	
	System* m_pSystem;
	std::string m_sCurrentPath;
	bool m_bIsDirty;
	Query* m_pCurrentQuery;
	QuerySelectionNode* m_pSelQuerySelectionNode;
	unsigned int m_nRecentListLength;
	HICON m_icon;
	
	void deactivateControls();
	void resetQueryProperties();
	void updateRecentMenu();
};

#endif