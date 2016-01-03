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

#include "../GlobExport/cyclicreferencefinder.h"
#include "../GlobExport/dependency.h"
#include "../GlobExport/module.h"
#include "../GlobExport/cyclic.h"

namespace DependencyLogic
{
	CyclicReferenceDetection::CyclicReferenceDetection( System* pSystem ) : m_pSystem(pSystem), m_iTotalDependencies(pSystem->getDependencies().size()), m_iWalkedDependencies(0)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		pSystem->incCyclicRefSearchID();
		
		m_dlgProgress.SetTitle(CFMsg(IDS_CYCLICREFSEARCH_TITLE));
		m_dlgProgress.SetLineText(1, CFMsg(IDS_CYCLICREFSEARCH_ANALYZING, 0, m_iTotalDependencies));
		m_dlgProgress.ShowModal(NULL);
		
		for each(const std::pair<std::string, Module*>& pairModuleName in pSystem->getModules())
		{
			if(pairModuleName.second != NULL)
				if(pairModuleName.second->getCyclicRefSearchID() < pSystem->getCyclicRefSearchID())
					walkDependencies(pairModuleName.second);
		}
		
		m_dlgProgress.EndDialog();
	}

	void CyclicReferenceDetection::walkDependencies( Module* pModule )
	{
		// cylic ref found
		if(pModule->getInSearchStack())
		{
			std::list<const Dependency*> lsCyclicDependencies;
		
			// compile all dependencies that belong to the path
			for each(const Dependency* pDependency in m_lsWalkedPath)
			{
				lsCyclicDependencies.push_back(pDependency);
				
				if(pDependency->getSourceModule() == pModule)
					break;
				
				// the execution must have ended if the current pDependency is the beginning
				// of the walked path, otherwise the resulting cyclic path cannot be really cyclic
				AFXASSUME(pDependency != *(--m_lsWalkedPath.end()) && "Internal condition violated.");
			}
			
			m_lsCyclicRefs.push_back(new CyclicReference(lsCyclicDependencies, m_pSystem));
		}
		else
			pModule->setInSearchStack(true);
		
		if(pModule->getCyclicRefSearchID() < m_pSystem->getCyclicRefSearchID())
		{
			pModule->setCyclicRefSearchID(m_pSystem->getCyclicRefSearchID());
			
			// walk the outgoing dependencies ...
			for each(const Dependency* pDependency in pModule->getOutgoingDependencies())
			{
				if(pDependency->getTargetModule() == NULL)
					continue;
					
				m_lsWalkedPath.push_front(pDependency);
				m_iWalkedDependencies++;
				m_dlgProgress.SetLineText(1, CFMsg(IDS_CYCLICREFSEARCH_ANALYZING, m_iWalkedDependencies, m_iTotalDependencies));
				const_cast<Dependency*>(pDependency)->setCyclicRefSearchID(m_pSystem->getCyclicRefSearchID());
				walkDependencies(const_cast<Module*>(pDependency->getTargetModule()));
				m_lsWalkedPath.pop_front();
			}
		}
		
		pModule->setInSearchStack(false);
	}
}