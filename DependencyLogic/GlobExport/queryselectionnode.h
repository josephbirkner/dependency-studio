/*     __                            __                               __          __
  ____/ /___  ____  ____  ____  ____/ /___  ____  _______  __  ______/ /_  ______/ /_____
 / __, / __ \/ __ \/ __ \/ ,_ \/ __, / __ \/ ,_ \/ ___/ / / / / ___\, / / / / __, /_/ __ \
/ /_/ / /_/ / /_/ / /_/ / / / / /_/ / /_/ / / / / /__/ /_/ / _\__, / / /_/ / /_/ / / /_/ /
\____/ ,___/ ,___/ ,___/\/ /_/\____/ ,___/\/ /_/\___/\__, /  \____/\/\___,_\____/\/\____/
     \____/\/    \____/            \____/               \/
               __  ____  ______     ____  ____________              __    __
              / /_/ / / / / __ \   /__, \/ __ \, /__, \          __/ /   /_/
             / ,_, / / / / /_/ /  ____/ / / / / /___/ /         /_/ /_  / /
            / / / / /_/ / ,___/  / ,___/ /_/ / / ,___/         / / ,_ \/ /
            \/  \/\____/\/       \____/\____/_/\____/ /\   ___/ / /_/ /_/
                                                      '/   \___/\____/
*/

#ifndef _DEPENDENCYCONTROL_QUERYSELNODE_H_
#define _DEPENDENCYCONTROL_QUERYSELNODE_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

using namespace Archiving;

namespace DependencyLogic
{
	class Query;
	class Observable;
	class ManagedTreeIcons;
	class QueryNode;

	enum query_options {
		query_option_whole_word = 1,
		query_option_ci = 2
	};
	
	/** @brief Derivate of std::unary_function that holds a purely virtual operator ().*/
	template<class _Arg, class _Result>
	struct executable_unary : public std::unary_function<_Arg, _Result> {
		virtual _Result operator() (_Arg) = 0;
	};

	/**
	 * @about Represents an element in an abstract hierarchy of nodes depicting
	 *        which and in what hierarchy the persistent subjects should be searched.
	 *        
	 *        A hierarchy of QuerySelectionNode objects is put together in a Query,
	 *        before the call to Query::execute.
	 *        
	 *        The hierarchy might be displayed in a CTreeCtrl, that has to be issued in
	 *        the contructor.
	 *        
	 *        Which QueryNode class the ctor ultimately represents, and what kind of
	 *        children it can have, is ultimately determined by the nodes is. For example,
	 *        there is an ID that makes a QuerSelectionNode stand for one or many Modules,
	 *        or there is an ID that identifies a node as an Exports source module.
	 *        
	 *        Whether or not the element represented by a QuerySelectionNode will ultimately
	 *        appear in its Queries search result, will depend on whether that node is marked
	 *        as selected.
	 * @see   Query, QueryNode
	 */
	class DEPENDENCYLOGIC_API QuerySelectionNode : IQueryObserver
	{
		friend class QuerySelectionNode;
	
	public:
		/**
		 * @about  Contruct a QueryNode from an id, a query object, an optional parent selection node,
		 *         and an optional CTreeCtrl the node should be displayed in.
		 * @param  nID The ID identifying the type of element this node represents. Can be one of:
		 *         <ul>
		 *         <li>IDNODE_CYCLIC_CHAIN_ERRORS</li>
		 *         <li>IDNODE_MODULE_INCOMING_DEPENDENCIES</li>
		 *         <li>IDNODE_MODULE_OUTGOING_DEPENDENCIES</li>
		 *         <li>IDNODE_MODULE_EXPORTS</li>
		 *         <li>IDNODE_DEPENDENCY_TARGET_MODULE</li>
		 *         <li>IDNODE_DEPENDENCY_SOURCE_MODULE</li>
		 *         <li>IDNODE_DEPENDENCY_EXPORTS</li>
		 *         <li>IDNODE_DEPENDENCIES</li>
		 *         <li>IDNODE_DEPENDENCY_MISSING_EXPORTS</li>
		 *         <li>IDNODE_EXPORT_SOURCE_MODULE</li>
		 *         <li>IDNODE_EXPORT_REFERENCING_DEPENDENCIES</li>
		 *         <li>IDNODE_SYSTEM</li>
		 *         </ul>
		 * @param  pQuery The query this QuerySelectionNode belongs to.
		 * @param  pParent This nodes parent node, or Null, id=f y=this node is a root selection node.
		 * @param  pTree The tree control this QueryNode should be displayed in, or Null, if this node
		 *         should not be displayed.
		 */
		QuerySelectionNode(INT nID, const Query* pQuery, const QuerySelectionNode* pParent, CTreeCtrl* pTree);
		
		/**
		 * @about  Contruct a QueryNode from another QueryNode to copy it. The new QueryNodewill be identical,
		 *         to refNode, except it will belong to a different Query and have a different HTREEITEM.
		 * @param  refNode The node from which the new nodes id, child node, search params and selection attributes
		 *         shall be copied.
		 * @param  pQuery The query which the copied node should belong to.
		 * @param  pParent The parent node of the copy node.
		 */
		QuerySelectionNode(QuerySelectionNode& refNode, Query* pQuery, QuerySelectionNode* pParent);

		/**
		 * @about  Changes the selection tree ctrl this node's item is displayed in.
		 * @param  pTreeCtrl The tree ctrl the query selection node is to be displayed in,
		 *         or Null, if the selection node is just to be removed from its current tree.
		 * @return void
		 */
		 void setTreeCtrl(CTreeCtrl* pTreeCtrl);

		/**
		 * @about Destructor. Will remove the treeitem and delet the child nodes.
		 */
		~QuerySelectionNode();
	
	public:
		/**
		 * @about  Retreive all subnode IDs for a specific parent node id.
		 * @param  nID The parent id you want to retreive the subnode ids for.
		 * @return Const list of the child ids.
		 * @see    s_mapNodeIDSubnodeIDs, buildSubnodeDict()
		 */
		static const std::list<INT>& getSubnodeIDsForID(INT nID) {return s_mapNodeIDSubnodeIDs[nID];}
		
		/**
		 * @about  Create a QueryNode from a single Observable object.
		 * @param  pModel The Observable object that should be represented by the new QueryNode.
		 * @param  pParent The parent the new node chould be added to as a child. Can be Null.
		 * @param  bApplyQuery Tells whether this node's query's applicability to pModel should be checked.
		 * @return The new node, appended to the parent if given, or Null,
		 *         if either pModel was rejected by the new QueryNode (see QueryNode_T::setModel), or
		 *         bApplyQuery was true, and Observable::isApplicable() for this QueryNode's Query returned false.
		 * @see    QueryNode, Observable::isApplicable()
		 */
		QueryNode* select(
			__in Observable* pModel,
			__in_opt QueryNode* pParent = NULL,
			__in_opt bool bApplyQuery = true
		) const;
		
		/**
		 * @about  Create QueryNodes for each child node id (see getSubnodeIDsForID) issuing
		 *         constParentNode's represented Observable equally for all child nodes.
		 *         select(Observable*, QueryNode*, bool) will be called for each child QuerySelectionNode.
		 * @param  constParentNode The QueryNOde node whose standard subnodes are to be created.
		 * @param  bApplyQuery Tells whether this node's query's applicability to the Observable should be checked.
		 *         Will be issued as the third parameter for each select(Observable*, QueryNode*, bool) call.
		 * @return void
		 * @see    getSubnodeIDsForID, select(Observable*, QueryNode*, bool)
		 */
		bool select(
			__out QueryNode& constParentNode,
			__in_opt bool bApplyQuery = true
		) const;
		
		/**
		 * @about  Create QueryNodes for this QuerySelectionNode from a container of objects,
		 *         converted to Observable objects using a custom unary function.
		 *         select(Observable*, QueryNode*, bool) will be called on each Observable object,
		 *         and the new QueryNodes appended to parentNode a child.
		 * @param  T_ElementClass [Template Parameter] The class of elements contained in the collection
		 * @param  T_CollectionClass [Template Parameter] The class of the collection.
		 * @param  sourceContainer The container. Must be of type T_CollectionClass.
		 * @param  parentNode The parent node, which the QueryNodes created from the Observable objects in
		 *         the container should be appended to.
		 * @param  elem_func The functor that should be used to convert from T_ElementClass to Observable*.
		 *         Must be a subclass of executable_unary, take a T_ElementClass reference as an argument,
		 *         and return a pointer to an Observable object.
		 * @param  bApplyQuery Tells whether this node's query's applicability to the Observable should be checked.
		 * @return void
		 * @see    selectSequence
		 */
		template<class T_ElementClass, class T_CollectionClass>
		bool select(
			__in T_CollectionClass& sourceContainer,
			__out QueryNode& parentNode,
			__in executable_unary<T_ElementClass&, Observable*>* elem_func,
			__in_opt bool bApplyQuery = true
		) const
		{
			AFXASSUME(elem_func != NULL);
			if(!getSelected())
				return false;
		
			bool bResult = false;
			
			for each( T_ElementClass const& element in sourceContainer )
			{
				Observable* pModel = (*elem_func)(const_cast<T_ElementClass&>(element));
				if(pModel && select(pModel, &parentNode, bApplyQuery))
					bResult = true;
			}
			
			return bResult;
		}
		
		/**
		 * @about  Create QueryNodes for this QuerySelectionNode from a list, deque or vector of objects,
		 *         converted to Observable objects using a custom unary function.
		 *         This function wraps a call to select<>(), with the difference that this fucntion has
		 *         elem_func as an optional parameter, assuming that the sequencial collection already contains
		 *         Observable objects by default.
		 * @param  T_ElementClass [Template Parameter] The class of elements contained in the collection
		 * @param  T_CollectionClass [Template Parameter] The class of the collection.
		 * @param  sourceContainer The container. Must be of type T_CollectionClass.
		 * @param  parentNode The parent node, which the QueryNodes created from the Observable objects in
		 *         the container should be appended to.
		 * @param  elem_func The functor that should be used to convert from T_ElementClass to Observable*.
		 *         Must be a subclass of executable_unary, take a T_ElementClass reference as an argument,
		 *         and return a pointer to an Observable object.
		 * @param  bApplyQuery Tells whether this node's query's applicability to the Observable should be checked.
		 * @return void
		 * @see    selectSequence
		 */
		template<class T_ElementClass, class T_CollectionClass>
		bool selectSequence(
			__in T_CollectionClass& sourceSequence,
			__out QueryNode& parentNode,
			__in_opt bool bApplyQuery = true,
			__in_opt executable_unary<T_ElementClass&, Observable*>* elem_func = NULL
		) const
		{
			struct default_func : public executable_unary<T_ElementClass&, Observable*> {
				Observable* operator() (T_ElementClass& element) {return const_cast<Observable*>((const Observable*) element);}
			} default_elem_func;

			if(!elem_func)
				elem_func = &default_elem_func;

			return select<T_ElementClass, T_CollectionClass>(sourceSequence, parentNode, elem_func, bApplyQuery);
		}
		
		/**
		 * @about  Create this QuerySelectionNodes subnodes. Can only be called once.
		 *         Will call getSubnodeIDsForID for this nodes ID, and create a new QuerySelectionNode
		 *         for each child ID, with this node as its parent.
		 * @return void
		 * @see    getSubnodeIDsForID
		 */
		void createSubnodes();
		
		/**
		 * @about  Create this node's subnodes and expand its treeitem.
		 * @return void
		 * @see    createSubnodes()
		 */
		void expand();

		/**
		 * @about  Collapses this node's treeitem if available.
		 * @return void
		 * @see    singleSelectRootChild()
		 */
		void collapse();
		
		/**
		 * @about  Retreive this nodes treeitem.
		 * @return The treeitem, or null, if this node is not diplayed in a tree control.
		 * @see    expand()
		 */
		HTREEITEM getTreeItem() const {return m_hTreeItem;}
		
		/**
		 * @about  Set this node as selected (relevant for the Query).
		 * @param  bSelected switch selection on or off.
		 * @param  bCheckTreeItem tells if the nodes treeitem should be checked
		 *         according to the value of bSelected.
		 * @return void
		 * @see    expand(), getTreeItem()
		 */
		void setSelected(bool bSelected, bool bCheckTreeItem, bool bAutoExpand = true, bool bSelectParent = true) const;
		
		/**
		 * @about  Returns whether this nodes selected-flag is set
		 * @return True if selected, false if otherwise.
		 * @see    setSelected()
		 */
		bool getSelected() const {return m_bSelected;}
		
		/**
		 * @about  Retreive/Create a subnode for a specific subnode ID.
		 * @param  nID The id of the new subnode to be created/retreived.
		 * @return The specific QuerySelectionNode. 
		 * @see    buildSubnodeDict, getSubnodeIDsForID
		 */
		QuerySelectionNode* getSubnode(INT nID) const;
		
		/**
		 * @about  Retreive this querynode's parent.
		 * @return The parent node if this node isn't a root node, otherwise Null. 
		 */
		const QuerySelectionNode* getParent() {return m_pParent;}
		
		/**
		 * @about  Returns this nodes treeicon resource according to its ID.
		 * @return The resource, or false, if no resource could be determined for this nodes id.
		 */
		INT getItemImage() const;
		
		/**
		 * @about  Returns this node's id as issued in the constructor.
		 * @return The node's id member.
		 */
		INT getID() const {return m_nID;}
		
		/**
		 * @about  Deselects and collapses all children that aren't pNode, if this node is a root node.
		 * @return void.
		 */
		void singleSelectRootChild(const QuerySelectionNode* pNode) const;
		
		/**
		 * @about  Retreive this nodes title. The node's id will be interpreted as a string resource identifier.
		 *         The treeitem of the node will have that string as its title.
		 * @return The string resource identified by this nodes id, with this nodes search term appended to it.
		 * @see    getID()
		 */
		virtual std::string updateTitle() const;
		
		/**
		 * @about  Retreive this nodes title. The node's id will be interpreted as a string resource identifier.
		 *         The treeitem of the node will have that string as its title.
		 * @return The string resource identified by this nodes id.
		 * @see    getID()
		 */
		std::string getTitle() const;
		 
		/**
		 * @about  Set the nodes search term. Result nodes possibly created by a select()-call
		 *         will be filtered by that term.
		 * @param  sTerm The search term.
		 * @return void
		 * @see    setSearchOnlyWords(), setSearchConsiderCase()
		 */
		void setSearchTerm(const std::string& sTerm) {m_sSearchTerm = sTerm; updateTitle();}
		
		/**
		 * @about  Set whether the search term should only be matched as a single word.
		 * @param  bState True if only words should be matched, false if otherwise.
		 * @return void
		 * @see    setSearchTerm(), setSearchConsiderCase()
		 */
		void setSearchOnlyWords(bool bState) {if( (m_nSearchFlags & query_option_whole_word) != bState) m_nSearchFlags ^= query_option_whole_word;}
		
		/**
		 * @about  Set whether the search term should only be matched if the letter casing matches too.
		 * @param  bState True if the search should be case-Sensitive, false if otherwise.
		 * @return void
		 * @see    setSearchTerm(), setSearchOnlyWords()
		 */
		void setSearchConsiderCase(bool bState) {if( (m_nSearchFlags & query_option_ci) == bState) m_nSearchFlags ^= query_option_ci;}
		
		virtual const std::string& getSearchTerm() const {return m_sSearchTerm;}
		virtual const bool getConsiderCase() const {return !(m_nSearchFlags & query_option_ci);}
		virtual const bool getSearchWord() const {return m_nSearchFlags & query_option_whole_word;}
		virtual void invalidate(const Observable* pModel) const;

	private:
		/** For each node id, a list of possible child node ids. Filled in buildSubnodeDict(). */
		static std::map<INT, std::list<INT>> s_mapNodeIDSubnodeIDs;
		
		/** TreeIconManager for the TreeCtrl used by the QueryNodes. */
		static ManagedTreeIcons* s_pSelTreeIcons;
		
		/** This method fills s_mapNodeIDSubnodeIDs. It's called once per runtime in createSubnodes(). */
		static void buildSubnodeDict();
		
		/** Returns whether any of the nodes child nodes is selected. */
		bool hasSelectedChildnodes() const;

		/** Tells whether the node is selected as relevant for the query. */
		mutable bool m_bSelected;
		
		/** Map pointing from node ids to child node objects. Filled in getSubnode/createSubnodes. */
		mutable std::map<INT, QuerySelectionNode*> m_mapChildNodeIDObject;
		
		/** The Query this QueryNode belongs to. Set in the constructor. */
		const Query* m_pQuery;
		
		/** The id of this node. */
		INT m_nID;
		
		/** The state of this node. Currently not used. reserved for future use.*/
		int m_nState;
		
		/** Tells how many subnodes have been created/whether createSubnodes has already been called for this node. */
		int m_nSubnodesCreated;
		
		/** The parent of this QuerySelectionNode, or Null, if this is a root selection node. */
		const QuerySelectionNode* m_pParent;
		
		/** The Treeitem of this QuerySelectionNode, or Null, if this node is not displayed in a TreeView. */
		HTREEITEM m_hTreeItem;
		
		/** The Treectrl this QuerySelectionNode is displayed in, or Null, if this node is not displayed in a TreeView.*/
		CTreeCtrl* m_pTreeCtrl;
		
		/** The search term result selections should be filtered by */
		std::string m_sSearchTerm;
		
		/** Bitfield consisting of none, one or both of Archiving::StringUtil::FindIgnoreCase/FindOnlyWords */
		int m_nSearchFlags;
	};
}

#endif