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

#include "stdafx.h"

#pragma hdrstop

#include "../GlobExport/system.h"
#include "../GlobExport/module.h"
#include "../GlobExport/dependency.h"
#include "../GlobExport/reader.h"
#include "../GlobExport/cyclicreferencefinder.h"

#include <boost/filesystem.hpp>

using namespace boost;

namespace DependencyLogic
{
	System::System() : m_pReader(NULL), m_bDestroyed(false)
	{
		
	}

	System::System( std::string sName, std::vector<std::string> vDirs, std::string sTempDir, SYSTEM_CREATE_PROGRESS_ROUTINE pProgressRoutine ) : m_sName(sName), m_vDirs(vDirs), m_pReader(NULL), m_bDestroyed(false)
	{
		std::list<std::string> lstBinaries;
		unsigned int iCount = 0;
		
		for each(std::string sPath in m_vDirs)
			iCount += interpretBinaryPath(sPath, lstBinaries);
		
		unsigned int iProgress = 0;
		
		for each(std::string sBinary in lstBinaries)
		{
			if( pProgressRoutine )
				if( !pProgressRoutine(iProgress, iCount, sBinary) )
					break;
					
			// add the module
			addModule(sBinary);
			
			iProgress += 1;
			
			// Call the progress routine
			if( pProgressRoutine )
				if( !pProgressRoutine(iProgress, iCount, sBinary) )
					break;
		}
	}
	
	System::~System()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		
		setDestroyed();
	
		CProgressDlg progressDialog;
		progressDialog.SetShowProgressBar(false);
		progressDialog.SetTitle(CFMsg(IDS_DELETESYSTEM));
		progressDialog.ShowModal(NULL);
		
		progressDialog.SetLineText(1, CFMsg(IDS_DELETESYSTEM_MODULES));
		for each(const std::pair<std::string, Module*>& pairModuleName in m_mapModuleNames)
			if(pairModuleName.second != NULL)
				delete pairModuleName.second;
		
		progressDialog.SetLineText(2, CFMsg(IDS_DELETESYSTEM_DEPENDENCIES));
		for each(Dependency* pDependency in m_lsDependencies)
			delete pDependency;
		
		progressDialog.EndDialog();
	}

	const Dependency* System::addDependency( Module* pFrom, std::string sTo, std::list<std::string> lsExports /*= std::list<std::string>()*/ ) const
	{
		Dependency* pDependency = new Dependency(pFrom, sTo, lsExports);
		pDependency->setSystem(this);
		
		linkDependency(pDependency);
			
		m_lsDependencies.push_back(pDependency);
		return pDependency;
	}

	Dependency* System::addDependency( Module* pFrom, std::string sTo, std::list<std::string> lsExports /*= std::list<std::string>()*/ )
	{
		AFXASSUME(! iequals(pFrom->getName(), sTo));
		return const_cast<Dependency*>(const_cast<const System*>(this)->addDependency(pFrom, sTo, lsExports));
	}

	const Module* System::getModule( std::string sName ) const
	{
		to_lower(sName);
		return m_mapModuleNames[sName];
	}

	const std::list<Dependency*>& System::getDependencies() const
	{
		return m_lsDependencies;
	}

	const std::map<std::string, Module*>& System::getModules() const
	{
		return m_mapModuleNames;
	}

	Module* System::addModule( std::string sPath )
	{
		Module* pModule = new Module(sPath, this);
		return pModule;
	}

	Module* System::addEmptyModule( std::string sName, std::string sPath )
	{
		Module* pModule = new Module(sName, sPath, this);
		return pModule;
	}
	
	void System::serialize( ISerializer *encoder )
	{
		// serialize main properties
		encoder->setString(m_sName, "name");
		encoder->setString(m_sTempDir, "temp_dir");
		encoder->setString(algorithm::join(m_vDirs, std::string(";")), "path");
		
		std::list<IArchivableObject*> ls;
		
		// serialize modules
		for each(const std::pair<std::string, Module*>& pairModuleName in m_mapModuleNames)
			if(pairModuleName.second != NULL)
				ls.push_back(pairModuleName.second);
		
		encoder->setArray(ls, "modules");

		// serialize dependencies
		encoder->setArray(std::list<IArchivableObject*>(m_lsDependencies.begin(), m_lsDependencies.end()), "dependencies");
	}

	void System::deserialize( IDeserializer *decoder )
	{
		ArchivingResult nResult;
		
		m_sName = decoder->getString("name", &nResult);
		m_sTempDir = decoder->getString("temp_dir", &nResult);
		std::vector<std::string> vsDirs;
		algorithm::split(vsDirs, decoder->getString("path", &nResult), algorithm::is_any_of(";"), algorithm::token_compress_on);
		m_vDirs.insert(m_vDirs.end(), vsDirs.begin(), vsDirs.end());
		
		if(m_pReader)
		{
			m_pReader->setDependencyCount(decoder->getArrayCount<Dependency>("dependencies", &nResult));
			m_pReader->setModuleCount(decoder->getArrayCount<Module>("modules", &nResult));
		}
		
		// deserialize modules
		std::list<Module*>* plsModules = decoder->getArray<Module>("modules", &nResult);
		
		for each( Module* pModule in *plsModules )
			linkModule(pModule);
			
		delete plsModules;
			
		// deserialize dependencies
		std::list<Dependency*>* plsDependencies = decoder->getArray<Dependency>("dependencies", &nResult);
		m_lsDependencies = *plsDependencies;
		delete plsDependencies;
	}

	void System::archive( std::string sPath )
	{
		CProgressDlg dlgProgress;
		Archiving::XMLArchive a;
		a.setObject(this, "system");
		a.save(sPath);
	}
	
	void System::linkDependency(__notnull Dependency* pDependency ) const
	{
		const Module* pTo = getModule(pDependency->getTargetModuleName());
		
		if( pTo )
		{
			pDependency->setTargetModule(pTo);
			pTo->addIncomingDependency(pDependency);
		}
		else
			addDependencyForMissingModule(pDependency);
	}
	
	void System::unlinkModule(__notnull Module* pModule) const
	{
		for(std::list<Dependency*>::iterator it = m_lsDependencies.begin(); it != m_lsDependencies.end(); /*it incremented in loop body*/ )
		{
			if( (*it)->getSourceModule() == pModule )
			{
				if((*it)->getTargetModule() != NULL)
					(*it)->getTargetModule()->removeIncomingDependency(*it);
					
				delete *it;
				it = m_lsDependencies.erase(it);
			}
			else
			{
				if( (*it)->getTargetModule() == pModule )
					(*it)->setTargetModule(NULL);
					
				addDependencyForMissingModule(*it);
				it++;
			}
		}
	}

	void System::linkModule( Module* pModule ) const
	{
		m_mapModuleNames[algorithm::to_lower_copy(pModule->getName())] = pModule;
		std::list<Dependency*>& lsMissers = getDependenciesForMissingModuleName(pModule->getName());
		
		for each(Dependency* pDependency in lsMissers)
		{
			// set all dependencies for this module as linked
			const_cast<System*>(this)->linkDependency(pDependency);
			pDependency->setSystemRegisteredMissingTargetModule(false);
		}
		
		lsMissers.clear();
	}

	void System::addDependencyForMissingModule( __notnull Dependency* pDependency ) const
	{
		if(pDependency->getSystemRegisteredMissingTargetModule())
			return;
			
		pDependency->setSystemRegisteredMissingTargetModule(true);
		getDependenciesForMissingModuleName(pDependency->getTargetModuleName()).push_back(pDependency);
	}

	void System::removeMissingModuleDependency( Dependency* pDependency )
	{
		std::list<Dependency*>& lsMissers = getDependenciesForMissingModuleName(pDependency->getTargetModuleName());
		for(std::list<Dependency*>::iterator it = lsMissers.begin(); it != lsMissers.end(); /*it incremented in loop body*/ )
			if(*it == pDependency)
			{
				pDependency->setSystemRegisteredMissingTargetModule(false);
				it = lsMissers.erase(it);
				return;
			}
			else
				it++;
	}

	std::list<Dependency*>& System::getDependenciesForMissingModuleName( std::string sModule ) const
	{
		algorithm::to_lower(sModule);
		return m_mapMissingModuleDependencies[sModule];
	}

	const std::list<CyclicReference*>& System::getCyclicReferences()
	{
		return m_lsCyclicRefs;
	}
	
	void System::updateCyclicRefs()
	{
		incCyclicRefSearchID();
		
		for each(CyclicReference* pRef in m_lsCyclicRefs)
			delete pRef;
			
		m_lsCyclicRefs.clear();
		m_lsCyclicRefs = CyclicReferenceDetection(this);
	}

	int System::interpretBinaryPath( __in std::string sDirectory, __out std::list<std::string>& lstBinaries ) const
	{
		WIN32_FIND_DATA findFileData;

		boost::filesystem::path aPath(sDirectory);
		std::string sFileMask = sDirectory;
		if(boost::filesystem::is_directory(aPath))
			sFileMask += "\\*.*";
		else if(boost::filesystem::is_regular_file(aPath)) {
			size_t nExtSepPos = sDirectory.find_last_of('.');
			if(nExtSepPos == std::string::npos)
				return 0;
			std::string sExt = sDirectory.substr(nExtSepPos + 1, sDirectory.length() - nExtSepPos - 1);
			if(isAcceptableBinaryExtension(sExt.c_str())) {
					lstBinaries.push_back(sDirectory);
					return 1;
			}
			return 0;
		}
		else { // Strip selector from path
			size_t nDirSepPos = sDirectory.find_last_of('\\');
			size_t nDirSepPosSlash = sDirectory.find_last_of('/');
			if(nDirSepPosSlash != std::string::npos && nDirSepPosSlash > nDirSepPos)
				nDirSepPos = nDirSepPosSlash;
			if(nDirSepPos != std::string::npos)
				sDirectory = sDirectory.substr(0, nDirSepPos);
		}
		
		HANDLE hFind = FindFirstFile( (sFileMask).data(), &findFileData );
		int iCount = 0;

		if( hFind != INVALID_HANDLE_VALUE )
		{
			do
			{
				if( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
					continue;

				std::string sFileName = sDirectory + "\\" + findFileData.cFileName;

				if( isAcceptableBinaryExtension(filesystem::path(sFileName).extension().string().c_str()) )
				{
					lstBinaries.push_back(sFileName);
					++iCount;
				}
			} while( ::FindNextFile( hFind, &findFileData ) != FALSE );

			// Close the find file handle
			::FindClose( hFind );
		}
		return iCount;
	}

	std::set<std::string> System::getMissingModuleNames()
	{
		std::set<std::string> setMissingModuleNames;
		for each(Dependency* pDependency in m_lsDependencies)
			if(pDependency->getSystemRegisteredMissingTargetModule())
				setMissingModuleNames.insert(pDependency->getTargetModuleName());
		return setMissingModuleNames;
	}

	bool System::isAcceptableBinaryExtension( const char* sExt ) const
	{
		std::string sOrig = sExt;
		std::string sLower = boost::algorithm::to_lower_copy(sOrig);
		return sLower == ".dll" || sLower == ".exe" || sLower == ".sys";
	}

}

