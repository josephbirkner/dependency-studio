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

#include "stdafx.h"

#pragma hdrstop

#include "../GlobExport/module.h"
#include "../GlobExport/export.h"
#include "../GlobExport/config.h"
#include "../GlobExport/errorlist.h"

#include "../include/dependscall.h"
#include "../include/dumpbincall.h"

namespace DependencyLogic
{
	Module::Module( std::string sOrigPath, const System* pSystem ) : Subject(pSystem), m_sOrigPath(sOrigPath), m_bInSearchStack(false)
	{
		m_sName = filesystem::path(sOrigPath).filename().string();
		setCurrentPath(sOrigPath);
	}
	
	Module::Module( std::string sName, std::string sOrigPath, const System* pSystem ) : Subject(pSystem), m_sOrigPath(sOrigPath), m_bInSearchStack(false), m_sName(sName), m_sCurrentPath(sOrigPath)
	{
		getSystem()->linkModule(this);
	}

	Module::Module() : m_bInSearchStack(false)
	{
		
	}

	void Module::setCurrentPath( std::string sNewPath )
	{
		typedef std::tr1::shared_ptr<DependsCall> DependsCallPtr;
		clear();

		AFXASSUME( algorithm::iequals(filesystem::path(sNewPath).filename().string(), m_sName) );
		m_sCurrentPath = sNewPath;
		
		DependsCallPtr psCall( (DependsCall*) NULL );
		
		if(Config().getUseDumpbin())
			psCall = DependsCallPtr(new DumpbinCall(sNewPath));
		else
			psCall = DependsCallPtr(new DependsCall(sNewPath));
			
		for each( const std::string& sExport in psCall->getExports() )
			m_mapExportNames[sExport] = new Export(sExport, this);
			
		for each( const std::pair<std::string, std::list<std::string>> pairImportedModuleExportedFunctions in psCall->getImportedModuleFunctions() )
			if(!pairImportedModuleExportedFunctions.first.empty())
				getSystem()->addDependency(this, pairImportedModuleExportedFunctions.first, pairImportedModuleExportedFunctions.second);
		
		getSystem()->linkModule(this);
	}
	
	ErrorList Module::getErrors()
	{
		ErrorList lsRet;
		
		for each(const Dependency* pDependency in m_lsIncoming)
			for each(const ExportRef& exportRef in pDependency->getExports())
				if( (const Export*) exportRef == NULL )
					lsRet.insert(
						(const char*) CFMsg(
							IDS_MODULE_ERROR_UNRESOLVED_EXPORT,
							pDependency->getSourceModule()->getName().c_str(),
							((std::string) exportRef).c_str(),
							getName().c_str()
						)
					);
				
		for each(const Dependency* pDependency in m_lsOutgoing)
			for each(const ExportRef& exportRef in pDependency->getExports())
				if( (const Export*) exportRef == NULL )
					lsRet.insert(
						(const char*) CFMsg(
							IDS_MODULE_ERROR_UNRESOLVED_EXPORT,
							getName().c_str(),
							((std::string) exportRef).c_str(),
							pDependency->getTargetModuleName().c_str()
						)
					);
				
		return lsRet;
	}

	Module::~Module()
	{
		clearExports();
	}

	void Module::clearExports()
	{
		for each( std::pair<std::string, Export*> pairExportName in m_mapExportNames )
			if( pairExportName.second )
				delete pairExportName.second;
		
		if(!getSystem()->getDestroyed())
			m_mapExportNames.clear();
	}

	void Module::addIncomingDependency( const Dependency* pDependency ) const
	{
		m_lsIncoming.push_back(pDependency);
	}

	void Module::addOutgoingDependency( const Dependency* pDependency ) const
	{
		m_lsOutgoing.push_back(pDependency);
	}

	void Module::serialize( ISerializer *encoder )
	{
		encoder->setString(m_sName, "name");
		encoder->setString(m_sOrigPath, "path");
		
		std::list<IArchivableObject*> ls;
		
		// serialize exports
		for each( std::pair<std::string, Export*> pairExportName in m_mapExportNames )
			if(pairExportName.second)
				ls.push_back(pairExportName.second);
		
		encoder->setArray(ls, "exports");
	}

	void Module::deserialize( IDeserializer *decoder )
	{
		ArchivingResult nResult;
	
		m_sName = decoder->getString("name", &nResult);
		m_sOrigPath = decoder->getString("path", &nResult);
		m_sCurrentPath = decoder->getString("path", &nResult);
		
		// deserialize exports
		std::list<Export*>* plsExports = decoder->getArray<Export>("exports", &nResult);
		
		if(nResult == Found)
			for each(Export* pExport in *plsExports)
				m_mapExportNames[pExport->getSignature()] = pExport;
				
		delete plsExports;
	}

	void Module::clear()
	{
		if(!m_lsIncoming.empty() || !m_lsOutgoing.empty())
			getSystem()->unlinkModule(this);
			
		m_lsOutgoing.clear();
		m_lsIncoming.clear();
		clearExports();
	}

	void Module::copyFromCurrentPathToOrigPath()
	{
		AFXASSUME(false && "Not Implemented!");
	}

	void Module::removeIncomingDependency( const Dependency* pDependency ) const
	{
		std::list<const Dependency*>::iterator foundIterator = std::find(m_lsIncoming.begin(), m_lsIncoming.end(), pDependency);
		if (foundIterator != m_lsIncoming.end())
			m_lsIncoming.erase(foundIterator);
	}

	void Module::adaptExportsAndResolveErrors( Module const* pOther )
	{
		for each(const std::pair<std::string, Export*>& exportNamePair in pOther->getExports())
		{
			if(exportNamePair.first.empty() == false && m_mapExportNames[exportNamePair.first] == NULL)
				m_mapExportNames[exportNamePair.first] = new Export(exportNamePair.first, this);
		}
		
		for each(Dependency const* pDependency in m_lsIncoming)
		{
			const_cast<Dependency*>(pDependency)->refExports();
		}
	}
}
