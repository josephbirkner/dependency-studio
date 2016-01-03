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

#include "../GlobExport/querynode.h"
#include "../GlobExport/system.h"

namespace DependencyLogic
{
	QueryNode::QueryNode( const QuerySelectionNode* pNode ) : m_pSelectionNode(pNode)
	{

	}

	QueryNode::~QueryNode()
	{
		for each( QueryNode* pNode in m_lsChildren )
			delete pNode;
	}

	void QueryNode::display( QueryView& treeView, QueryNode* pParent )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		m_hTreeItem = treeView.GetTreeCtrl().InsertItem(getTitle().c_str(), pParent ? pParent->getTreeItem() : TVI_ROOT);
		for each(QueryNode* pNode in m_lsChildren)
			pNode->display(treeView, this);
		treeView.GetTreeCtrl().Expand(m_hTreeItem, TVE_EXPAND);
		treeView.setItemBitmap(m_hTreeItem, m_pSelectionNode->getItemImage(), false);
	}

	void QueryNode::invalidate()
	{

	}

	bool QueryNode::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->select(*this, bApplyQuery);
	}

	std::string QueryNode::getTitle() const
	{
		return m_pSelectionNode->updateTitle();
	}
	
	void QueryNode::unload( QueryView& treeView )
	{
		for each( QueryNode* pNode in m_lsChildren )
			pNode->unload(treeView);
			
		treeView.GetTreeCtrl().DeleteItem(m_hTreeItem);
	}

	void QueryNode::addChild( QueryNode* pChild )
	{
		m_lsChildren.push_back(pChild);
	}

	void QueryNode::writeToFile( FILE* pFile, int iDepth )
	{
		for(int i = 0; i < iDepth; i++)
			fputs("\t", pFile);
			
		fputs(getTitle().c_str(), pFile);
		fputs("\r\n", pFile);
		
		iDepth++;
		
		for each(QueryNode* pNode in m_lsChildren)
			pNode->writeToFile(pFile, iDepth);
	}

	bool QueryNodeSystem::expand( QueryNode* pNode, bool bApplyQuery )
	{
		bool bResult = false;
	
		bResult |= m_pSelectionNode->getSubnode(IDNODE_MODULES)->select< const std::pair<std::string, Module*>, const std::map<std::string, Module*> >(
			getModel()->getModules(), *this, &m_fctModuleMapElem, bApplyQuery
		);
		
		bResult |= m_pSelectionNode->getSubnode(IDNODE_DEPENDENCIES)->selectSequence<const Dependency*, const std::list<Dependency*>>(
			getModel()->getDependencies(), *this, bApplyQuery
		);
		
		for each(const std::pair<std::string, Module*>& pairModuleName in getModel()->getModules())
			if( pairModuleName.second )
				bResult |= m_pSelectionNode->getSubnode(IDNODE_EXPORTS)->select< const std::pair<std::string, Export*>, const std::map<std::string, Export*> >(
					pairModuleName.second->getExports(), *this, &m_fctExportMapElem, bApplyQuery
				);
				
		if(m_pSelectionNode->getSubnode(IDNODE_CYCLIC_CHAIN_ERRORS)->getSelected())
			getModel()->updateCyclicRefs();
				
		bResult |= m_pSelectionNode->getSubnode(IDNODE_CYCLIC_CHAIN_ERRORS)->selectSequence<const CyclicReference*, const std::list<CyclicReference*>>(
			getModel()->getCyclicReferences(), *this, bApplyQuery
		);
				
		return bResult;
	}
	
	bool QueryNodeModuleIncomingDependencies::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->getSubnode(IDNODE_DEPENDENCIES)->selectSequence<const Dependency*, const std::list<const Dependency*>>(
			getModel()->getIncomingDependencies(), *this, bApplyQuery
		);
	}

	bool QueryNodeModuleOutgoingDependencies::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->getSubnode(IDNODE_DEPENDENCIES)->selectSequence<const Dependency*, const std::list<const Dependency*>>(
			getModel()->getOutgoingDependencies(), *this, bApplyQuery
		);
	}

	bool QueryNodeModuleExports::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->getSubnode(IDNODE_EXPORTS)->select< const std::pair<std::string, Export*>, const std::map<std::string, Export*> >(
			getModel()->getExports(), *this, &m_fctExportMapElem, bApplyQuery
		);
	}

	bool QueryNodeCyclicChainErrorDependencies::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->getSubnode(IDNODE_DEPENDENCIES)->selectSequence<const Dependency*, const std::list<const Dependency*>>(
			getModel()->getDependencies(), *this, bApplyQuery
		);
	}

	bool QueryNodeExportSourceModule::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->getSubnode(IDNODE_MODULES)->select(const_cast<Module*>(getModel()->getModule()), this, bApplyQuery);
	}

	bool QueryNodeExportDependencies::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->getSubnode(IDNODE_DEPENDENCIES)->selectSequence<const Dependency*, const std::list<const Dependency*>>(
			getModel()->getDependencies(), *this, bApplyQuery
		);
	}

	bool QueryNodeDependencyTargetModule::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->getSubnode(IDNODE_MODULES)->select(const_cast<Module*>(getModel()->getTargetModule()), this, bApplyQuery);
	}

	bool QueryNodeDependencySourceModule::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->getSubnode(IDNODE_MODULES)->select(const_cast<Module*>(getModel()->getSourceModule()), this, bApplyQuery);
	}

	bool QueryNodeDependencyExports::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->getSubnode(IDNODE_EXPORTS)->selectSequence<const ExportRef, const std::list<ExportRef>>(
			getModel()->getExports(), *this, bApplyQuery
		);
	}

	bool QueryNodeDependencyMissingExportErrors::expand( QueryNode* pNode, bool bApplyQuery )
	{
		return m_pSelectionNode->getSubnode(IDNODE_MISSING_EXPORT_ERROR)->selectSequence<const ExportRef, const std::list<ExportRef>>(
			getModel()->getExports(), *this, bApplyQuery, &m_fctIncompleteExportRefListElem
		);
	}
}

