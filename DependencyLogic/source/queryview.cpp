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

#include "StdAfx.h"
#include "../include/resource.h"

#pragma hdrstop

#include "../GlobExport/queryview.h"
#include "../GlobExport/querynode.h"
#include "../GlobExport/query.h"
#include "../GlobExport/config.h"
#include "../include/treeiconmanager.h"

namespace DependencyLogic
{
	IMPLEMENT_DYNAMIC(QueryView, CColumnTreeCtrl)

	BEGIN_MESSAGE_MAP(QueryView, CColumnTreeCtrl)
		//{{AFX_MSG_MAP(CDependencyStudio)
		ON_WM_CREATE()
		ON_NOTIFY(HDN_ITEMCHANGED, HeaderID, OnHeaderItemChanged)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	
	QueryView::QueryView() : m_pQuery(NULL), m_pIcons(NULL)
	{
		
	}

	QueryView::~QueryView()
	{
		if(m_pIcons) delete m_pIcons;
	}

	void QueryView::setQuery( const Query* pQuery )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		if(pQuery != m_pQuery)
		{
			if(m_pQuery)
				delete m_pQuery;

			if(m_pQuery = pQuery)
				pQuery->display(*this);
		}
		EnableWindow(m_pQuery != NULL);
	}

	void QueryView::PreSubclassWindow()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CColumnTreeCtrl::PreSubclassWindow();
		new ManagedTreeIcons(&this->GetTreeCtrl(), m_pIcons);
		
		m_bmpHupLogo.LoadBitmap(IDB_HUP_LOGO);
		GetTreeCtrl().ModifyStyle(TVS_CHECKBOXES|TVS_FULLROWSELECT, TVS_CHECKBOXES|TVS_FULLROWSELECT);
		InsertColumn(0, _T("Name"), LVCFMT_LEFT, Config().getUserColWidth(0, 500));
	
		LVBKIMAGE bk;
		//bk.xOffsetPercent = bk.yOffsetPercent = 60;
		bk.hbm = m_bmpHupLogo;
		bk.ulFlags |= LVBKIF_STYLE_TILE;
		GetTreeCtrl().SetBkImage(&bk);
	}
	
	inline void QueryView::setItemBitmap( HTREEITEM hItem, INT nResID, bool bGrey )
	{
		AFXASSUME(m_pIcons);
		m_pIcons->setItemBitmap(hItem, nResID, bGrey);
	}

	void QueryView::OnHeaderItemChanged( NMHDR* pNMHDR, LRESULT* pResult )
	{
		CColumnTreeCtrl::OnHeaderItemChanged(pNMHDR, pResult);
		LPNMHEADER pnm = (LPNMHEADER)pNMHDR;
		
		if(pnm->pitem >= 0)
			Config().setUserColWidth(pnm->iItem, pnm->pitem->cxy);
	}
}

