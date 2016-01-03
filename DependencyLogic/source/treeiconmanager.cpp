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

#pragma hdrstop

#include "../include/treeiconmanager.h"

namespace DependencyLogic
{
	ManagedTreeIcons::ManagedTreeIcons( CTreeCtrl* pCtrl, ManagedTreeIcons*& var ) : m_lRefs(0), m_refVar(var)
	{
		m_ilBitmaps.Create(16, 16, ILC_COLOR32, 0, 2);
		m_pTreeCtrl = pCtrl;
		m_refVar = this;
		pCtrl->SetImageList(&m_ilBitmaps, TVSIL_NORMAL);
	}

	ManagedTreeIcons::~ManagedTreeIcons()
	{
		m_refVar = NULL;
		m_pTreeCtrl->SetImageList(NULL, TVSIL_NORMAL);
	}

	void ManagedTreeIcons::setItemBitmap(HTREEITEM hItem, INT nResID, bool bGrey)
	{
		int iIndex = 0;

		if( m_mapResIDImageListIndex[nResID].first == false )
		{
			CBitmap hBmp, hGreyBmp;
			hBmp.LoadBitmap(nResID);
			m_ilBitmaps.Add(&hBmp, (COLORREF) 0x00000000);
			m_mapResIDImageListIndex[nResID].first = true;
			m_mapResIDImageListIndex[nResID].second = iIndex = m_ilBitmaps.GetImageCount() - 1;
			AfxGetGrayBitmap(hBmp, &hGreyBmp, (COLORREF) 0x00000000);
			m_ilBitmaps.Add(&hGreyBmp, (COLORREF) 0x00000000);
			iIndex += (int) bGrey;
		}
		else
			iIndex = m_mapResIDImageListIndex[nResID].second + (int) bGrey;
		
		m_pTreeCtrl->SetItemImage(hItem, iIndex, iIndex);
	}

	void ManagedTreeIcons::addRef( void* p )
	{
		m_lRefs += m_mapPointerRefStatus[p] == false;
		m_mapPointerRefStatus[p] = true;
	}

	void ManagedTreeIcons::removeRef( void* p )
	{
		m_lRefs -= m_mapPointerRefStatus[p] == true;
		if(m_lRefs <= 0)
		{
			delete this;
		}
		else
			m_mapPointerRefStatus[p] = false;
	}

}

