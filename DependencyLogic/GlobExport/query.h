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

#ifndef _DEPENDENCYCONTROL_QUERY_H_
#define _DEPENDENCYCONTROL_QUERY_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

#include "observer.h"

using namespace Archiving;

namespace DependencyLogic
{
	class QuerySelectionNode;
	class QueryNode;
	class Observable;
	class QueryView;

	/**
	 * @about Represents a query of the persistence level, implementing the QueryParameters
	 *        introduced by the IQueryObserver interface, sorting its results into a tree
	 *        of QueryNode objects in a hierarchy defined by a tree of QuerySelectionNode
	 *        objects.
	 *        
	 *        The hierarchy of QuerySelectionNodes that defines the hierarchy of the query result
	 *        can be defined using a CTreeCtrl, issuable in the ctor.
	 *        
	 *        Once all parameters are set, a query can be executed using the execute() method.
	 *        You can then display the results of the query using the display() method.
	 * @see   IQueryObserver, QueryNode, QuerySelectionNode
	 */
	class DEPENDENCYLOGIC_API Query
	{
		friend class Observable;
		friend class QueryView;
		friend class Query;
	
	public:
		/**
		 * @about  Constructs a Query object with default parameters and an empty hierarchy,
		 *         for a specific system.
		 * @param  pSelTree The CTreeCtrl, where the search result hierarchy is to be configurable.
		 * @param  pSystem The System this search is supposed to be applied to.
		 */
		Query(CTreeCtrl* pSelTree, System* pSystem);
		Query(Query& refQuery);
		
		/**
		 * @about Deletes all QueryNodes and QuerySelectionNodes belonging to this Query.
		 */
		~Query();
		
		/**
		 * @about  Executes the query with its current search parameters on the System
		 *         object issued in the destructor. Fills the m_lsResults member with
		 *         QueryNode objects.
		 * @return void
		 * @see    display()
		 */
		void execute();
		
		/**
		 * @about  Writes all the information from the query result tree into a text file under sPath.
		 *         execute() must have been called before.
		 * @return void
		 * @param  sPath The path to the file where the results should be written to.
		 */
		void exportResults(std::string sPath) const;
		
		/**
		 * @about  Changes the selection tree ctrl.
		 * @param  pSelTreeCtrl The tree ctrl the query selection is to be displayed in,
		 *         or Null, if the selection nodes are just to be removed from theire current tree.
		 * @return void
		 */
		void setSelTree(CTreeCtrl* pSelTreeCtrl) const;
		
	protected:
		/**
		 * @about  Acting as an observer of Observable instances, this method
		 *         is called by those instances if the instance gets deleted.
		 *         
		 *         The query will check for one of its QueryNodes to hold the
		 *         Observable pointer, and call the QueryNode::invalidate()
		 *         method on each one that does.
		 * @param  pModel The Observable object that is to 
		 * @return void
		 * @see    Observable, QueryNode::invalidate()
		 */
		virtual void invalidate(const Observable* pModel) const;
		
	private:
		/**
		 * @about  Displays the Query's QueryNode m_lsResults list in a QueryView control.
		 *         This method is called by the QueryView::setQuery().
		 * @param  pCtrl The QueryView to display the results.
		 * @return void
		 * @see    QueryView::display()
		 */
		void display(QueryView& pCtrl) const;
		
		/** The tree control used to configure the QuerySelectionNode hierarchy of this Query. Might be Null. */
		mutable CTreeCtrl* m_pSelTreeCtrl;
		
		/** The list holding the queries root QuerySelectionNode objects. */
		std::list<QuerySelectionNode*> m_lsSelection;
		
		/** The list holding the QueryNode objects (if any), if execute() has been called */
		std::list<QueryNode*> m_lsResult;
		
		/** The System to be queried. */
		System* m_pSystem;
		
		/** The QueryView this Query is displayed in. Might be Null. */
		mutable QueryView* m_pQueryView;
	};
}

#endif