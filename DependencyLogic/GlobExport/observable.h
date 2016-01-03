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

#ifndef _DEPENDENCYCONTROL_OBSERVABLE_H_
#define _DEPENDENCYCONTROL_OBSERVABLE_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

#include "observer.h"

namespace DependencyLogic
{
	class Query;
	
	/**
	 * @about Class to manage the observation of Subjects by Queries. A Query inherits from IQueryObserver and calls
	 *        addObserver() on each Observable object that is displayed in its result tree.
	 *        The Observable object informs all of its observers when it is deleted, using the IQueryObserver::invalidate method.
	 *        
	 *        The Observable objects are held by QueryNodes, for the QueryNode to know the data it represents. Different QueryNode
	 *        subclasses hold different Observable subclasses.
	 *        
	 *        Observable objects generally belong to the persistence level, and therefore inherit the IArchivableObject interface.
	 * @see   IQueryObserver, QueryNode, QueryNode_T
	 */
	class DEPENDENCYLOGIC_API Observable : public IArchivableObject
	{
		public:
			Observable() : m_iCyclicRefSearchID(0) {;}
		
			/**
			 * @about Destructor. Iterates through all entries in m_lsObservers and calls the
			 *        IQueryObserver::invalidate() method on each.
			 */
			virtual ~Observable();
		
		public:
			/**
			 * @about  Add an observer to the observer list. The observers applicability on
			 *         this object will not be checked, nor whether the observer has already
			 *         been added!
			 * @param  pObserver The observer to be added to the Observables observer list.
			 * @return void
			 * @see    isApplicable()
			 */
			void addObserver(const IQueryObserver* pObserver);
			
			/**
			 * @about  Checks for an IQueryObservers query parameters to fit the data
			 *         of this observable object. The IQueryObservers search term and
			 *         other search flags will be checked to match the string returned by
			 *         the getName() method.
			 * @param  pObserver The observer whose parameters are to be matched.
			 * @return True if the query parameters match, false if otherwise.
			 * @see    IQueryObserver, getName()
			 */
			bool isApplicable(const IQueryObserver* pObserver);
			
			/**
			 * @about  Returns the presumed name of the observable object. The name can be
			 *         any string uniquely identifying the object, but should be human-readable.
			 *         The specific implementation is handled by the subclass.
			 *         
			 *         QueryNode objects will resort to this method to determine the titles of
			 *         theire treeitems.
			 * @return The string to be interpreted as the name.
			 * @see    IsApplicable, QueryNode::getTitle.
			 */
			virtual std::string getName() const = 0;
			
			int getCyclicRefSearchID() {return m_iCyclicRefSearchID;}

			void incCyclicRefSearchID() {m_iCyclicRefSearchID++;}
			
			void setCyclicRefSearchID(int id) {m_iCyclicRefSearchID = id;}
			
		private:
			/** The list of observers added using the addObserver() method. */
			std::list<const IQueryObserver*> m_lsObservers;
			
			/** Indicates the id of the current cyclic ref search. Will be incremented with each search. */
			int m_iCyclicRefSearchID;
	};
}

#endif