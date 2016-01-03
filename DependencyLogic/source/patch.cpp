#include "StdAfx.h"

#pragma hdrstop

#include "../GlobExport/patch.h"
#include "../GlobExport/module.h"
#include "../GlobExport/dependency.h"
#include "../GlobExport/errorlist.h"
#include "../GlobExport/reader.h"


namespace DependencyLogic 
{
	Patch::Patch( __in PatchMode nMode, __in std::vector<std::string> vsPathes, __in __notnull System* pSystem, __in_opt std::string sLogFile /*= ""*/ ) : m_pLogFile(NULL), m_nMode(nMode), m_pSystem(pSystem), m_pOther(NULL)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		
		if(!sLogFile.empty())
			m_pLogFile = fopen(sLogFile.c_str(), "a");
			
		for each(std::string sPath in vsPathes)
		{
			if(PathFileExists(sPath.c_str()))
			{
				if(PathIsDirectory(sPath.c_str()))
				{
					// Insert all modules from a directory
					std::list<std::string> lstBinaries;
					m_pSystem->interpretBinaryPath(sPath, lstBinaries);
					for each(std::string sBin in lstBinaries)
						addModulePathForPatch(sBin);
				}
				else
				{
					if(nMode == TestWithOtherSystem)
					{
						// Insert modules from another database if they
						// are missing in this database.
						m_nMode = PatchTestSilent;
						
						if(m_pOther = Reader(sPath))
						{
							std::set<std::string> setMissingModuleNames = m_pSystem->getMissingModuleNames();
							for each(const std::string& sMissingModule in setMissingModuleNames)
							{
								const Module* pModule = m_pOther->getModule(sMissingModule);
								if(pModule)
									addModuleForPatchFromOtherSystem(pModule);
							}
						}
					}
					else
						// Insert a specific module
						addModulePathForPatch(sPath);
				}
			}
			else
				log((const char*) CFMsg(IDS_PATCH_LOG_MODULE_NOT_FOUND));
		}
		
		std::string sImplodedModuleList;
		for each(ModuleAndNewPath const& aModule in m_lsModulePathes)
			sImplodedModuleList += aModule.second + "\r\n\t";
			
		log((const char*) CFMsg(IDS_PATCH_LOG_ANNOUNCEMENT, pSystem->getName().c_str(), m_lsModulePathes.size(), sImplodedModuleList.c_str()));
		
		m_dlgProgress.SetTitle(CFMsg(IDS_PATCH_IN_PROGRESS));
		m_dlgProgress.ShowModal(NULL);
	}

	Patch::~Patch()
	{
		if(m_pLogFile)
			fclose(m_pLogFile);
			
		#ifndef _DEBUG
		delete m_pOther; // this is too slow in debug mode
		#endif
	}

	int Patch::displayLog()
	{
		std::vector<std::string> tempVektor(m_lsResult.begin(), m_lsResult.end());
		return MessageBox(NULL, algorithm::join(tempVektor, std::string("\r\n")).c_str(), CFMsg(IDS_PATCH_LOG_TITLE), MB_OK|MB_TOPMOST);
	}
	
	bool Patch::execute()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		bool bError = false;
		
		int nUserChoice = IDOK;
		
		if(m_nMode < PatchReplaceForce)
		{
			int nTotal = m_lsModulePathes.size();
			int nDone = 0;
			
			// Set the new pathes on all modules
			for each(ModuleAndNewPath moduleAndNewPath in m_lsModulePathes)
			{
				m_dlgProgress.SetLineText(1, CFMsg(IDS_PATCH_COLLECTING_INFORMATION, moduleAndNewPath.first->getCurrentPath().c_str()));
				moduleAndNewPath.first->setCurrentPath(moduleAndNewPath.second);
				m_dlgProgress.UpdateProgress((DWORD) ++nDone, (DWORD) nTotal);
			}
			
			// Adapt exports from other system's modules
			for each(Module* pModule in m_lsOtherSystemModules)
			{
				if(m_pOther)
				{
					pModule->adaptExportsAndResolveErrors(m_pOther->getModule(pModule->getName()));
				}
			}
			
			// Compile the new error list
			ErrorList lsErrors;
			
			for each(ModuleAndNewPath moduleAndNewPath in m_lsModulePathes)
				lsErrors += moduleAndNewPath.first->getErrors();
				
			for each(Module* pModule in m_lsOtherSystemModules)
				lsErrors += pModule->getErrors();
				
			// Subtract errors that existed before this patch
			lsErrors -= m_lsOldErrors;
			
			if(lsErrors.getCount() > 0)
			{
				log( (const char*) CFMsg(IDS_PATCH_LOG_MODULE_CAUSING, lsErrors.getCount()) );
				log( lsErrors.getString() );
			}
			else
				log( (const char*) CFMsg(IDS_PATCH_LOG_MODULE_OK) );
			
			log( (const char*) CFMsg(IDS_PATCH_LOG_COMPLETE) );
			
			bError = lsErrors.getCount() > 0;
			
			m_dlgProgress.EndDialog();
			
			if(m_nMode > PatchTestSilent)
				nUserChoice = displayLog();
		}
		
		if(m_nMode > PatchTest && nUserChoice == IDOK)
		{
		}
		
		return bError;
	}

	void Patch::log( std::string sMessage )
	{
		m_lsResult.push_back(sMessage);
		if(m_pLogFile)
			fputs((sMessage+"\r\n").c_str(), m_pLogFile);
	}
	
	bool Patch::addModuleForPatchFromOtherSystem( Module const* pOtherModule)
	{
		bool bSuccess = pOtherModule != NULL &&
		                pOtherModule->getSystem() == m_pOther &&
		                m_pSystem->getModule(pOtherModule->getName()) == NULL;
		
		if(bSuccess == false)
			return false;
	
		Module* pModule = const_cast<Module*>(m_pSystem->addEmptyModule(pOtherModule->getName(), pOtherModule->getCurrentPath()));
		if(pModule == NULL)
			return false;
			
		m_lsOldErrors += pModule->getErrors();
		m_lsOtherSystemModules.push_back(pModule);
		
		return true;
	}

	bool Patch::addModulePathForPatch( const std::string& sPath )
	{
		bool bSuccess = PathFileExists(sPath.c_str()) && PathIsDirectory(sPath.c_str()) == FALSE;
		if(bSuccess == false)
			return false;
	
		Module* pModule = const_cast<Module*>(m_pSystem->getModule(filesystem::path(sPath).filename().string()));

		if(!pModule)
		{
			pModule = m_pSystem->addModule(sPath);
			if(pModule == NULL)
				return false;
		}
		else
			m_lsOldErrors += pModule->getErrors();
		
		m_lsModulePathes.push_back(ModuleAndNewPath(pModule, sPath));
		return true;
	}
}
