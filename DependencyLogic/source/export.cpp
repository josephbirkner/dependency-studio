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

#include "../GlobExport/export.h"

#pragma hdrstop

namespace DependencyLogic
{
	Export::Export( std::string sName, const Module* pModule ) :
		Subject(pModule->getSystem()),
		m_sSignature(sName),
		m_pModule(pModule)
	{
	}

	Export::Export()
	{

	}

	Export::~Export()
	{
		// if the system is to be scrapped then we dont have to savely disconnect
		// the export object from the referencing dependencies.
		if(!getSystem()->getDestroyed())
			for each(const Dependency* pDependency in m_lsDependencies)
				pDependency->removeExport(this);
	}

	void Export::addDependency( const Dependency* pDependency ) const
	{
		m_lsDependencies.push_back(pDependency);
	}

	void Export::setModule( const Module* pModule )
	{
		m_pModule = pModule;
	}

	void Export::serialize( ISerializer *encoder )
	{
		encoder->setString(m_sSignature, "name");
	}

	void Export::deserialize( IDeserializer *decoder )
	{
		ArchivingResult nResult;
		m_sSignature = decoder->getString("name", &nResult);
	}

	void Export::removeDependency( const Dependency* pDependency ) const
	{
		for( std::list<const Dependency*>::iterator it = m_lsDependencies.begin(); it != m_lsDependencies.end(); )
		{
			if(*it == pDependency)
				it = m_lsDependencies.erase(it);
			else
				it++;
		}
	}

	std::string Export::getName() const
	{
		char vchUndecorated[512];
		if(UnDecorateSymbolName(getSignature().c_str(), vchUndecorated, 512, UNDNAME_COMPLETE) == 0)
			return getSignature();
		else
			return vchUndecorated;
	}

}
