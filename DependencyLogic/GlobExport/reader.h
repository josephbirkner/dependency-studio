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

#ifndef _DEPENDENCYCONTROL_READER_H_
#define _DEPENDENCYCONTROL_READER_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

// PCH/STL/Windows/RogueWave
#include "StdAfx.h"

using namespace Archiving;

#include "progressdlg.hpp"

namespace DependencyLogic
{
	class Module;

	/**
	 * @about Functor that encapsulates the loading process of a dependency database XML.
	 *        Will create an XMLArchive, and extract the System object from it, thereby triggering
	 *        the deserialization of all contained modules, exports and dependencies.
	 *        
	 *        The Path loaded will immidiately be added to the list of recently opened databases.
	 *        Also, since loading the database can take a second or two, the Reader class will
	 *        display a progress dialog depicting exactly what the program is currently doing
	 *        for an enhanced user experience.
	 * @see   System, Config::getRecentDatabases()
	 */
	class DEPENDENCYLOGIC_API Reader : public IArchiveDelegate
	{
		friend class System;
	
	public:
		/**
		 * @about  Triggers the deserialization of the system object
		 *         contained in the database pointed to by sXmlPath.
		 * @param  sXmlPath The path to the xml file that holds an XML
		 *         structure readable by the Archiving::XMLArchive. The
		 *         path will be added to the list of recently opened databases.
		 * @see    operator System*()
		 */
		Reader(std::string sXmlPath);
		
		/**
		 * @about  Returns the system extracted from the xml.
		 * @return The System object, or Null, if loading failed.
		 * @see    System
		 */
		operator System*() {return m_pSystem;}
		
	protected:
		/** The system extracted from the xml file. */
		System* m_pSystem;
		
		/** The module whose exports are currently being loaded */
		Module* m_pModule;
		
		/** The progress dialog */
		CProgressDlg m_dlgProgress;
		
		/** The XMLArchive currently loading */
		Archiving::XMLArchive* m_pArchive;
		
		/** Archiving result varible used in different Archive calls */
		Archiving::ArchivingResult m_nResult;
		
		/** The count of modules loaded (second), paired with the total count of modules (first). */
		mutable std::pair<unsigned int, unsigned int> m_nModules;
		
		/** The count of dependencies loaded (second), paired with the total count of dependencies (first). */
		mutable std::pair<unsigned int, unsigned int> m_nDependencies;
	
		/**
		 * @about  Delegate method from the IArchiveDelegate interface
		 *         to track the loading progress.
		 */
		virtual IArchivableObject* handleInstance( const IArchivableObject* pObject );
		
		/**
		 * @about  Delegate method from the IArchiveDelegate interface
		 *         to check if the given object was loaded completely.
		 */
		virtual bool afterDeserializeObject(IArchivableObject *pObject);
		
		/**
		 * @about  Sets m_nModules.first to the toal count of modules in the system.
		 *         Called by System::deserialize.
		 *         Figure required to track the loading process.
		 * @param  nCount The module count.
		 * @return void
		 * @see    System, m_nModules
		 */
		void setModuleCount(unsigned int nCount) const {m_nModules.first = nCount;}
		
		/**
		 * @about  Sets m_nDependencies.first to the total count of dependencies in the system.
		 *         Called by System::deserialize.
		 *         Figure required to track the loading process.
		 * @param  nCount The module count.
		 * @return void
		 * @see    System, m_nDependencies
		 */
		void setDependencyCount(unsigned int nCount) const {m_nDependencies.first = nCount;}

	};
}

#endif