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

#ifndef _DEPENDENCYCONTROL_CYCLIC_H_
#define _DEPENDENCYCONTROL_CYCLIC_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

namespace DependencyLogic
{
	class CyclicReferenceDetection;

	/**
	 * @about Persistence level class that represents a single cyclic reference made up
	 *        by a circle of Dependency objects.
	 * @see   CyclicReferenceDetection, System
	 */
	class DEPENDENCYLOGIC_API CyclicReference : public Subject
	{
		friend class IDeserializer;
		friend class CyclicReferenceDetection;
		
	private:
		CyclicReference();
		CyclicReference(const std::list<const Dependency*>& lsDependencies, const System* pSystem);
		virtual ~CyclicReference() {}
		
	public:
		const std::list<const Dependency*>& getDependencies() const {return m_lsDependencies;}
		
		virtual void serialize(ISerializer *encoder) {;}
		virtual void deserialize(IDeserializer *decoder) {;}
		virtual std::string getClassName() const {return "CylicReference";}
		std::string getName(void) const {return m_sName;}
		
	private:
		std::list<const Dependency*> m_lsDependencies;
		std::string m_sName;
	};
}

#endif