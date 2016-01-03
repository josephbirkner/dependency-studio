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

#ifndef _DEPENDENCYCONTROL_QUERYOBSERVER_H_
#define _DEPENDENCYCONTROL_QUERYOBSERVER_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

namespace DependencyLogic
{
	class Observable;
	
	/**
	 * @about Interface to hide the Query frontend level from the persistence level.
	 *        Holds all interface methods a persistent Observable object needs to
	 *        interact with the Query level.
	 *        
	 *        That is, informing a Query observing an Observable object that the object is
	 *        about to be deleted, and matching Query parameters like the search term
	 *        with the Observable objects name.
	 * @see   Observable
	 */
	class IQueryObserver
	{
		friend class Observable;
		
	public:
		/**
		 * @about  Get the search term of the Query
		 *         Only has a meaning, if either module names or export names 
		 * @return The search term, or an empty string.
		 */
		virtual const std::string& getSearchTerm() const = 0;
		
		/**
		 * @about  Returns, whether the search term is to be matched case-sensitively.
		 * @return True for case-sensitivity, false if otherwise.
		 */
		virtual const bool getConsiderCase() const = 0;
		
		/**
		 * @about  Returns, whether only whole words should be matched with the search term.
		 * @return True if only words should be matched, false if otherwise.
		 */
		virtual const bool getSearchWord() const = 0;
		
	protected:
		/**
		 * @about  Informs the observing search, that a Observable object is about to be deleted,
		 *         and any references to it should be removed from the QuerNode result tree.
		 * @param  pModel The Observable object that should be forgotten about. The Observable object
		 *         calls this method in its destructor on each IQueryObserver it is registered with.
		 * @return void
		 * @see    Observable::invalidate()
		 */
		virtual void invalidate(const Observable* pModel) const = 0;
	};
}

#endif