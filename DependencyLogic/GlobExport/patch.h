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

#ifndef _DEPENDENCYCONTROL_PATCH_H_
#define _DEPENDENCYCONTROL_PATCH_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

#include "contrib\DependencyControl\DependencyLogic\GlobExport\errorlist.h"

namespace DependencyLogic
{
	enum PatchMode
	{
		PatchTestSilent = 1,      /** Insertion will be simulated (but the actual binaries will not be replaced) */ 
		PatchTest,                /** Insertion will be simulated (but the actual binaries will not be replaced),
		                              and the results of the simulated patch will be displayed. */ 
		TestWithOtherSystem,    /** A special resolve test: Using this mode, you can check, whether missing modules
		                              in your database can be resolved using another database file. */
		PatchReplaceShowResults,  /** Insertion will be simulated (before the actual binaries get replaced).
		                              The user can abort the replacement over the display of the simulation results. */ 
		PatchReplaceForce         /** The binaries will be replaced and the database updated, without any user interaction,
		                              regardless of dependency errors introduced by the patch. Optionally, the results will be
		                              written to a patch logfile. */
	};

	/**
	 * @about Functor to execute or simulate a patch for one or more binaries.
	 *        A progress dialog will be shown depicting the state of the insertion.
	 *        A PatchMode can be issued to control the behaviour of the functor.
	 * @see   operator bool(), operator int()
	 */
	class DEPENDENCYLOGIC_API Patch
	{
	public:
		/**
		 * @about Functor ctor. Will execute the patching/database update.
		 * @param nMode Indicates one of PatchTest, PatchReplaceShowResults or PatchReplaceForce.
		 * @param vsPathes 
		 * @param pSystem 
		 * @param sLogFile 
		 */
		Patch(__in PatchMode nMode, __in std::vector<std::string> vsPathes, __in __notnull System* pSystem, __in_opt std::string sLogFile = "");

		/** Displays all log messages in a MessageBox. */
		int displayLog();

		/** Patch destructor. Will close the logfile if necessary. */
		~Patch();
		
		/** Writes a log message to the log file if existing. */
		void log(std::string sMessage);
		
		/** Executes the patch operation. Returns whether any errors were detected. */
		bool execute();

	private:
		typedef std::pair<Module*, std::string> ModuleAndNewPath;
		bool addModulePathForPatch( const std::string& sPath );
		bool addModuleForPatchFromOtherSystem( Module const* pOtherModule);

		/** The log file which results are written to. Might be Null. */
		FILE* m_pLogFile;
	
		/** All the lines that will be displayed/written to the log. */
		std::list<std::string> m_lsResult;
		
		/** ProgressDialog displaying the progress of the update process for all given modules. */
		CProgressDlg m_dlgProgress;
		
		/** PatchMode issued in the constructor */
		PatchMode m_nMode;
		
		/** List of errors taht existed before the Patch was introduced */
		ErrorList m_lsOldErrors;
		
		/** List of the module instances with the new pathes they should have */
		std::list<ModuleAndNewPath> m_lsModulePathes;
		
		/** Modules to be imported from another system (resolve_with_other_system) */
		std::list<Module*> m_lsOtherSystemModules;
		
		/** The system the Patch is executed on */
		System* m_pSystem;
		
		/** For the resolve_with_other_system-command, this is the other system */
		System* m_pOther;
	};
}

#endif