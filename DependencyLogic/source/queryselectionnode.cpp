
#include "StdAfx.h"

#pragma hdrstop

#include "../GlobExport/query.h"
#include "../GlobExport/queryselectionnode.h"
#include "../GlobExport/observable.h"
#include "../GlobExport/querynode.h"
#include "../include/treeiconmanager.h"
#include "../include/nodefactory.h"

namespace DependencyLogic
{
	std::map<INT, std::list<INT>> QuerySelectionNode::s_mapNodeIDSubnodeIDs;
	ManagedTreeIcons* QuerySelectionNode::s_pSelTreeIcons(NULL);

	QuerySelectionNode::QuerySelectionNode( INT nID, const Query* pQuery, const QuerySelectionNode* pParent, CTreeCtrl* pTree ) :
		m_nID(nID),
		m_pQuery(pQuery),
		m_pTreeCtrl(NULL),
		m_nSubnodesCreated(false),
		m_nState(0),
		m_hTreeItem(NULL),
		m_pParent(pParent),
		m_bSelected(false),
		m_nSearchFlags(0)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState())
		setTreeCtrl(pTree);
	}

	QuerySelectionNode::QuerySelectionNode( QuerySelectionNode& refNode, Query* pQuery, QuerySelectionNode* pParent ) :
		m_nID(refNode.m_nID),
		m_pQuery(pQuery),
		m_pTreeCtrl(NULL),
		m_nSubnodesCreated(refNode.m_nSubnodesCreated),
		m_nState(refNode.m_nState),
		m_hTreeItem(NULL),
		m_pParent(pParent),
		m_bSelected(false),
		m_nSearchFlags(refNode.m_nSearchFlags),
		m_sSearchTerm(refNode.m_sSearchTerm)
	{
		setSelected(refNode.getSelected(), true, false);
		
		// copy child nodes
		for each(const std::pair<INT, QuerySelectionNode*>& pairNodeID in refNode.m_mapChildNodeIDObject)
			m_mapChildNodeIDObject[pairNodeID.first] = new QuerySelectionNode(*(pairNodeID.second), pQuery, this);
	}


	INT QuerySelectionNode::getItemImage() const
	{
		switch(m_nID)
		{
			#pragma push_macro("NODE_IMAGE")
			#define NODE_IMAGE(node, image) case node: return image;
		
			NODE_IMAGE(IDNODE_EXPORTS, IDB_EXPORT)
			NODE_IMAGE(IDNODE_EXPORT_SOURCE_MODULE, IDB_MODULE_INCOMING)
			NODE_IMAGE(IDNODE_EXPORT_REFERENCING_DEPENDENCIES, IDB_EXPORT_REFERENCES)
			NODE_IMAGE(IDNODE_MODULES, IDB_MODULE)
			NODE_IMAGE(IDNODE_MODULE_EXPORTS, IDB_MODULE_EXPORTS)
			NODE_IMAGE(IDNODE_MODULE_OUTGOING_DEPENDENCIES, IDB_MODULE_OUTGOING)
			NODE_IMAGE(IDNODE_MODULE_INCOMING_DEPENDENCIES, IDB_MODULE_INCOMING)
			NODE_IMAGE(IDNODE_DEPENDENCIES, IDB_DEPENDENCY)
			NODE_IMAGE(IDNODE_DEPENDENCY_EXPORTS, IDB_MODULE_EXPORTS)
			NODE_IMAGE(IDNODE_DEPENDENCY_MISSING_EXPORTS, IDB_MISSING_TARGET)
			NODE_IMAGE(IDNODE_DEPENDENCY_SOURCE_MODULE, IDB_MODULE_OUTGOING)
			NODE_IMAGE(IDNODE_DEPENDENCY_TARGET_MODULE, IDB_MODULE_INCOMING)
			NODE_IMAGE(IDNODE_CYCLIC_CHAIN_ERRORS, IDB_CYCLIC_REFERENCE)
			NODE_IMAGE(IDNODE_CYCLIC_CHAIN_ERROR_DEPENDENCIES, IDB_MODULE)
			NODE_IMAGE(IDNODE_MISSING_EXPORT_ERROR, IDB_EXPORT_MISSING)
			NODE_IMAGE(IDNODE_SYSTEM, IDB_MODULE_EXPORTS)
			
			#pragma pop_macro("NODE_IMAGE")
		}
		
		DebugBreak();
		return 0;
	}

	QuerySelectionNode::~QuerySelectionNode()
	{
		setTreeCtrl(NULL);
	}
	
	QueryNode* QuerySelectionNode::select(
		__in Observable* pModel,
		__in_opt QueryNode* pParent,
		__in_opt bool bApplyQuery
	) const
	{
		if(!pModel || !getSelected()) return NULL;
		
		if(!bApplyQuery || ( pModel->isApplicable(this) && getSelected() ) )
		{
			QueryNode* pNode = QueryNodeFactory(pModel, this);
			
			if( bApplyQuery && hasSelectedChildnodes() && !pNode->expand(NULL, true) && m_pParent )
			{
				delete pNode;
				pNode = NULL;
			}
			
			if(pParent && pNode)
				pParent->addChild(pNode);
				
			return pNode;
		}
		
		return NULL;
	}

	bool QuerySelectionNode::select( __out QueryNode& constParentNode, __in_opt bool bApplyQuery /*= true */ ) const
	{
		bool bRet = false;
		
		for each( const std::pair<INT, QuerySelectionNode*>& pairNodeID in m_mapChildNodeIDObject)
			if( pairNodeID.second->select(constParentNode.getModel(), &constParentNode, bApplyQuery) )
				bRet = true;
				
		return bRet;
	}

	void QuerySelectionNode::createSubnodes()
	{
		if(m_nSubnodesCreated == 0)
		{
			if(s_mapNodeIDSubnodeIDs.empty())
				buildSubnodeDict();

			for each(INT nSubNode in getSubnodeIDsForID(m_nID))
			{
				m_nSubnodesCreated++;
				m_mapChildNodeIDObject[nSubNode] = new QuerySelectionNode(nSubNode, m_pQuery, this, m_pTreeCtrl);
			}
		}
	}

	void QuerySelectionNode::expand()
	{
		if(!m_nSubnodesCreated)
			createSubnodes();
		
		if(m_pTreeCtrl)
			m_pTreeCtrl->Expand(m_hTreeItem, TVE_EXPAND);
	}
	
	void QuerySelectionNode::collapse()
	{
		if(m_pTreeCtrl)
			m_pTreeCtrl->Expand(m_hTreeItem, TVE_COLLAPSE);
	}
	
	void QuerySelectionNode::buildSubnodeDict()
	{
		s_mapNodeIDSubnodeIDs[IDNODE_SYSTEM].push_back(IDNODE_MODULES);
		s_mapNodeIDSubnodeIDs[IDNODE_SYSTEM].push_back(IDNODE_DEPENDENCIES);
		s_mapNodeIDSubnodeIDs[IDNODE_SYSTEM].push_back(IDNODE_EXPORTS);
		s_mapNodeIDSubnodeIDs[IDNODE_SYSTEM].push_back(IDNODE_CYCLIC_CHAIN_ERRORS);
		s_mapNodeIDSubnodeIDs[IDNODE_EXPORTS].push_back(IDNODE_EXPORT_SOURCE_MODULE);
		s_mapNodeIDSubnodeIDs[IDNODE_EXPORTS].push_back(IDNODE_EXPORT_REFERENCING_DEPENDENCIES);
		s_mapNodeIDSubnodeIDs[IDNODE_EXPORT_REFERENCING_DEPENDENCIES].push_back(IDNODE_DEPENDENCIES);
		s_mapNodeIDSubnodeIDs[IDNODE_EXPORT_SOURCE_MODULE].push_back(IDNODE_MODULES);
		s_mapNodeIDSubnodeIDs[IDNODE_MODULES].push_back(IDNODE_MODULE_EXPORTS);
		s_mapNodeIDSubnodeIDs[IDNODE_MODULES].push_back(IDNODE_MODULE_OUTGOING_DEPENDENCIES);
		s_mapNodeIDSubnodeIDs[IDNODE_MODULES].push_back(IDNODE_MODULE_INCOMING_DEPENDENCIES);
		s_mapNodeIDSubnodeIDs[IDNODE_MODULE_EXPORTS].push_back(IDNODE_EXPORTS);
		s_mapNodeIDSubnodeIDs[IDNODE_MODULE_OUTGOING_DEPENDENCIES].push_back(IDNODE_DEPENDENCIES);
		s_mapNodeIDSubnodeIDs[IDNODE_MODULE_INCOMING_DEPENDENCIES].push_back(IDNODE_DEPENDENCIES);
		s_mapNodeIDSubnodeIDs[IDNODE_DEPENDENCIES].push_back(IDNODE_DEPENDENCY_EXPORTS);
		s_mapNodeIDSubnodeIDs[IDNODE_DEPENDENCIES].push_back(IDNODE_DEPENDENCY_MISSING_EXPORTS);
		s_mapNodeIDSubnodeIDs[IDNODE_DEPENDENCIES].push_back(IDNODE_DEPENDENCY_SOURCE_MODULE);
		s_mapNodeIDSubnodeIDs[IDNODE_DEPENDENCIES].push_back(IDNODE_DEPENDENCY_TARGET_MODULE);
		s_mapNodeIDSubnodeIDs[IDNODE_DEPENDENCY_EXPORTS].push_back(IDNODE_EXPORTS);
		s_mapNodeIDSubnodeIDs[IDNODE_DEPENDENCY_MISSING_EXPORTS].push_back(IDNODE_MISSING_EXPORT_ERROR);
		s_mapNodeIDSubnodeIDs[IDNODE_DEPENDENCY_SOURCE_MODULE].push_back(IDNODE_MODULES);
		s_mapNodeIDSubnodeIDs[IDNODE_DEPENDENCY_TARGET_MODULE].push_back(IDNODE_MODULES);
		s_mapNodeIDSubnodeIDs[IDNODE_CYCLIC_CHAIN_ERRORS].push_back(IDNODE_CYCLIC_CHAIN_ERROR_DEPENDENCIES);
		s_mapNodeIDSubnodeIDs[IDNODE_CYCLIC_CHAIN_ERROR_DEPENDENCIES].push_back(IDNODE_DEPENDENCIES);
	}

	void QuerySelectionNode::setSelected( bool bSelected, bool bCheckTreeItem, bool bAutoSelectChildren /*= true*/, bool bSelectParent /*= true*/ ) const
	{
		m_bSelected = bSelected;
			
		if(bSelected)
		{
			// show children
			const_cast<QuerySelectionNode*>(this)->expand();
			
			if(bAutoSelectChildren)
			{
				// select single child
				if(m_nSubnodesCreated == 1)
					for each(std::pair<INT, QuerySelectionNode*> pairChildNodeIDObject in m_mapChildNodeIDObject)
						pairChildNodeIDObject.second->setSelected(true, true, false, false);
			}
			
			if(m_pParent)
				m_pParent->singleSelectRootChild(this);
		
			// recursively select parent node
			if(m_pParent && bSelectParent)
				m_pParent->setSelected(true, true, false);
		}
		else if(m_nSubnodesCreated > 0)
		{
			// recursively deselect children
			for each(std::pair<INT, QuerySelectionNode*> pairChildNodeIDObject in m_mapChildNodeIDObject)
				pairChildNodeIDObject.second->setSelected(false, true);
				
			// collapse the treeitem
			const_cast<QuerySelectionNode*>(this)->collapse();
		}
		
		if(m_hTreeItem && bCheckTreeItem)
			m_pTreeCtrl->SetCheck(m_hTreeItem, bSelected);
	}
	
	bool QuerySelectionNode::hasSelectedChildnodes() const
	{
		for each(std::pair<INT, QuerySelectionNode*> pairChildNodeIDObject in m_mapChildNodeIDObject)
		{
			if(pairChildNodeIDObject.second->getSelected())
				return true;
		}
		return false;
	}

	std::string QuerySelectionNode::updateTitle() const
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		std::string sRet = getTitle() + (getSearchTerm().empty() ? std::string("") : std::string(" ['") + getSearchTerm() + "']");
		if(m_hTreeItem)
			m_pTreeCtrl->SetItemText(m_hTreeItem, sRet.c_str());
		return sRet;
	}

	QuerySelectionNode* QuerySelectionNode::getSubnode( INT nID ) const
	{
		const_cast<QuerySelectionNode*>(this)->createSubnodes();
		return m_mapChildNodeIDObject[nID];
	}

	void QuerySelectionNode::singleSelectRootChild( const QuerySelectionNode* pNode ) const
	{
		if(!m_pParent)
			for each(std::pair<INT, QuerySelectionNode*> pairChildNodeIDObject in m_mapChildNodeIDObject)
				if(pairChildNodeIDObject.second != pNode)
					pairChildNodeIDObject.second->setSelected(false, true);
	}

	void QuerySelectionNode::invalidate( const Observable* pModel ) const
	{
		
	}

	std::string QuerySelectionNode::getTitle() const
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		return (const char*) CMsg(m_nID);
	}

	void QuerySelectionNode::setTreeCtrl( CTreeCtrl* pTreeCtrl )
	{
		if(pTreeCtrl != m_pTreeCtrl)
		{
			if(m_pTreeCtrl && m_hTreeItem)
			{
				for each(std::pair<INT, QuerySelectionNode*> pairChildNodeIDObject in m_mapChildNodeIDObject)
					if(pairChildNodeIDObject.second)
						pairChildNodeIDObject.second->setTreeCtrl(NULL);
						
				m_pTreeCtrl->DeleteItem(m_hTreeItem);
				m_hTreeItem = NULL;
				s_pSelTreeIcons->removeRef(this);
			}
			
			m_pTreeCtrl = pTreeCtrl;
			
			if(pTreeCtrl)
			{
				m_hTreeItem = pTreeCtrl->InsertItem(updateTitle().c_str(), m_pParent ? m_pParent->getTreeItem() : TVI_ROOT);
				pTreeCtrl->SetItemData(m_hTreeItem, (DWORD_PTR) this);

				// make sure we have a treeicon manager
				if(!s_pSelTreeIcons) new ManagedTreeIcons(pTreeCtrl, s_pSelTreeIcons);

				s_pSelTreeIcons->addRef(this);
				s_pSelTreeIcons->setItemBitmap(m_hTreeItem, getItemImage(), false);
				
				for each(std::pair<INT, QuerySelectionNode*> pairChildNodeIDObject in m_mapChildNodeIDObject)
					if(pairChildNodeIDObject.second)
						pairChildNodeIDObject.second->setTreeCtrl(pTreeCtrl);
						
				setSelected(m_bSelected, true, false, false);
			}
		}
	}
}

