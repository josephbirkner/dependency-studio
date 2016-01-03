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

#ifndef _DEPENDENCYCONTROL_SUBJECT_H_
#define _DEPENDENCYCONTROL_SUBJECT_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

#include "ArchiveUtil/GlobExport/ArchiveUtil.hpp"
using namespace Archiving;

#include "system.h"
#include "observable.h"

namespace DependencyLogic
{
	/**
	 * @about Persistence level superclass for all System-aggregated Observable classes.
	 * @see   Module, Dependency, Export
	 */
	class DEPENDENCYLOGIC_API Subject : public Observable
	{
	public:
		/**
		 * @about  Contructor to create a subject (System aggregate) for a
		 *         specific System.
		 * @param  pSystem The System the Subject belongs to.
		 * @see    System
		 */
		Subject(const System* pSystem) : m_pSystem(pSystem){};
		
		Subject() : m_pSystem(NULL) {;}
		virtual ~Subject() {;}
		
	public:
		/**
		 * @about  Accessor for the System member of the System-aggregated object (Subject)
		 * @return const System for read access
		 * @see    setSystem()
		 */
		const System* getSystem() const {return m_pSystem;}
		
		/**
		 * @about  Setter for the System member of the System-aggregated object (Subject)
		 * @param  pSystem The System object this Subject belonsg to.
		 * @return void
		 * @see    getSystem()
		 */
		void setSystem(const System* pSystem) {m_pSystem = pSystem;}
	
	private:
		/** Read-Access pointer to the System this Subject belongs to. */
		const System* m_pSystem;
	};
}

#endif