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

#ifndef _DEPENDENCYCONTROL_CYCLICREFDETECTION_H_
#define _DEPENDENCYCONTROL_CYCLICREFDETECTION_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

namespace DependencyLogic
{
	class CyclicReference;

	/**
	 * @about Functor to manage the detection of cyclic references.
	 * @see	  System, CyclicReference  
	 */
	class DEPENDENCYLOGIC_API CyclicReferenceDetection
	{
		friend class System;
	
	private:
		CyclicReferenceDetection(System* pSystem);
		
		void walkDependencies(Module* pModule);
		
		inline operator std::list<CyclicReference*>& () {return m_lsCyclicRefs;}
		
		/** The list of cyclic references detected up to now. */
		std::list<CyclicReference*> m_lsCyclicRefs;
		
		/** The progress dialog depicting the progress of the search. */
		CProgressDlg m_dlgProgress;
		
		/** The stack of recursively walked modules. Grows and shrinks with the recursion.
		 * Used to track the dependencies of the cyclic reference if the recursion ends up
		 * at a module which it has already touched. The most recent stack entry is on top of the list.
		 */
		std::list<const Dependency*> m_lsWalkedPath;
		
		/** The system whose cyclic references are being searched. */
		System* m_pSystem;
		
		/** The total count of dependencies in the system. */
		int m_iTotalDependencies;
		
		/** The count of dependencies already processed. */
		int m_iWalkedDependencies;
	};
}

#endif