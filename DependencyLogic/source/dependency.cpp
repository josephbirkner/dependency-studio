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

#include "../GlobExport/module.h"
#include "../GlobExport/dependency.h"
#include "../GlobExport/export.h"

namespace DependencyLogic
{
	Dependency::Dependency( Module* pFrom, std::string sTo, std::list<std::string> vExports ) :
		Subject(pFrom->getSystem()),
		m_sTo(sTo),
		m_pTo(NULL),
		m_pFrom(pFrom),
		m_bSystemRegisteredMissingTargetModule(false)
	{
		for each( const std::string& sExport in vExports )
			m_lsExports.push_back(ExportRef(sExport));
			
		m_pFrom->addOutgoingDependency(this);
	}

	Dependency::Dependency() :
		m_pTo(NULL),
		m_pFrom(NULL),
		m_bSystemRegisteredMissingTargetModule(false)
	{
		
	}

	Dependency::~Dependency()
	{
		// if the system is to be scrapped then we dont have to savely detach
		// the dependency object from the referenced exports.
		if(getSystem()->getDestroyed())
			return;
		
		if(getSystemRegisteredMissingTargetModule())
			const_cast<System*>(getSystem())->removeMissingModuleDependency(this);
					
		unrefExports();
	}
		
	void Dependency::setTargetModule( const Module* pModule )
	{
		if( pModule != m_pTo )
		{
			if( m_pTo )
			{
				unrefExports();
				m_pTo = NULL;
			}
			if( pModule )
			{
				AFXASSUME(iequals(m_sTo, pModule->getName()));
				m_pTo = pModule;
				refExports();
			}
		}
	}

	void Dependency::unrefExports()
	{
		for each(const ExportRef& hExportRef in m_lsExports)
		{
			if( hExportRef.pExport )
				hExportRef.pExport->removeDependency(this);
			
			const_cast<ExportRef&>(hExportRef).pExport = NULL;
		}
	}

	void Dependency::refExports()
	{
		AFXASSUME(m_pTo);
		for each(const ExportRef& hExportRef in m_lsExports)
		{
			if( hExportRef.pExport )
				continue;
		
			const_cast<ExportRef&>(hExportRef).pExport = m_pTo->getExport(hExportRef.sExport);
			
			if( hExportRef.pExport )
				hExportRef.pExport->addDependency(this);
		}
	}

	void Dependency::removeExport( const Export* pExport ) const
	{
		for( std::list<ExportRef>::iterator it(m_lsExports.begin()); it != m_lsExports.end(); )
		{
			if((*it).pExport == pExport)
				it = m_lsExports.erase(it);
			else
				it++;
		}
	}

	void Dependency::serialize( ISerializer *encoder )
	{
		encoder->setString(m_pFrom->getName(), "from");
		encoder->setString(m_sTo, "to");
		
		std::string sExports;
		
		for each( const ExportRef& hExport in m_lsExports )
		{
			if( !sExports.empty() )
				sExports += ";";
				
			sExports += hExport.sExport;
		}
		
		encoder->setString(sExports, "exports");
	}

	void Dependency::deserialize( IDeserializer *decoder )
	{
		ArchivingResult nResult;
		m_pFrom = getSystem()->getModule(decoder->getString("from"));
		if(m_pFrom == NULL)
			return;
		std::vector<std::string> it;
		algorithm::split(it, decoder->getString("exports", &nResult), algorithm::is_any_of(";"), algorithm::token_compress_on);
		for each( const std::string& sExport in it)
			m_lsExports.push_back(ExportRef(sExport));
			
		getSourceModule()->addOutgoingDependency(this);
			
		m_sTo = decoder->getString("to", &nResult);
		getSystem()->linkDependency(this);
	}
}