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

#include "../GlobExport/query.h"
#include "../GlobExport/querynode.h"
#include "../GlobExport/queryselectionnode.h"

namespace DependencyLogic
{
	Query::Query( CTreeCtrl* pSelTree, System* pSystem ) : m_pSystem(pSystem), m_pQueryView(NULL), m_pSelTreeCtrl(pSelTree)
	{
		m_lsSelection.push_back(new QuerySelectionNode(IDNODE_SYSTEM, this, NULL, pSelTree));
		m_lsSelection.front()->expand();
	}

	Query::Query( Query& refQuery ) : m_pQueryView(NULL), m_pSystem(refQuery.m_pSystem), m_pSelTreeCtrl(NULL)
	{
		for each(QuerySelectionNode* pSelNode in refQuery.m_lsSelection)
			m_lsSelection.push_back(new QuerySelectionNode(*pSelNode, this, NULL));
			
		setSelTree(refQuery.m_pSelTreeCtrl);
	}

	Query::~Query()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		
		for each(QuerySelectionNode* pNode in m_lsSelection)
			delete pNode;
			
		if(m_pQueryView)
			m_pQueryView->GetTreeCtrl().DeleteAllItems();
			
		for each(QueryNode* pQueryNode in m_lsResult)
			delete pQueryNode;
	}

	void Query::invalidate( const Observable* pModel ) const
	{
		for each(QueryNode* pNode in m_lsResult)
			if(pNode->getModel() == pModel)
				pNode->invalidate();
	}

	void Query::execute()
	{
		if(m_lsResult.empty())
			for each(QuerySelectionNode* pNode in m_lsSelection)
				if(pNode->getSelected())
					m_lsResult.push_back(pNode->select(m_pSystem));
	}
	
	void Query::display( QueryView& treeView ) const
	{
		if(!m_lsResult.empty())
			for each(QueryNode* pNode in m_lsResult)
				pNode->display(treeView, NULL);
				
		m_pQueryView = &treeView;
	}

	void Query::exportResults( std::string sPath ) const
	{
		FILE* pFile = fopen(sPath.c_str(), "a");
		fputs("----------------------------------------------------------------------\r\n", pFile);
		for each(QueryNode* pParent in m_lsResult)
		{
			pParent->writeToFile(pFile, 0);
		}
		fclose(pFile);
	}

	void Query::setSelTree( CTreeCtrl* pSelTreeCtrl ) const
	{
		if(pSelTreeCtrl != m_pSelTreeCtrl)
		{
			for each(QuerySelectionNode* pSelNode in m_lsSelection)
				pSelNode->setTreeCtrl(pSelTreeCtrl);
				
			m_pSelTreeCtrl = pSelTreeCtrl;
		}
	}
}

