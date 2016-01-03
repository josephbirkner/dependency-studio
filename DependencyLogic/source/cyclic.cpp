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

#include "Contrib\DependencyControl\DependencyLogic\GlobExport\cyclic.h"
#include "Contrib\DependencyControl\DependencyLogic\GlobExport\dependency.h"

namespace DependencyLogic
{
	CyclicReference::CyclicReference( const std::list<const Dependency*>& lsDependencies, const System* pSystem ) : Subject(pSystem), m_lsDependencies(lsDependencies)
	{
		for each(const Dependency* pDependency in m_lsDependencies)
			m_sName += pDependency->getSourceModule()->getName() + ", ";
			
		m_sName.erase(m_sName.size() - 2, 2);
	}
}
