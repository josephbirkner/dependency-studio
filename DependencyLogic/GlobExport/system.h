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

#ifndef _DEPENDENCYCONTROL_SYSTEM_H_
#define _DEPENDENCYCONTROL_SYSTEM_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

#include "ArchiveUtil/GlobExport/ArchiveUtil.hpp"
using namespace Archiving;

#include "observable.h"

typedef bool (*SYSTEM_CREATE_PROGRESS_ROUTINE)(unsigned int iDone, unsigned int iCount, std::string sCurrentModule);

namespace DependencyLogic
{
	class Dependency;
	class Module;
	class Reader;
	class CyclicReference;

	/**
	 * @about  Facade class of the persistence level that aggregates all other Subject objects.
	 *         Can be deserialized using the Reader functor. Can be saved using the archive() function.
	 *         
	 *         A System object encapsulates a collection of Module objects, dependent on each other
	 *         as described by a collection of Dependency objects. A system object can be created from
	 *         a set of pathes containing EXE and/or DLL files, and a name for the database.
	 * @see    Subject, Reader
	 */
	class DEPENDENCYLOGIC_API System : public Observable
	{
		friend class Module;
		friend class Dependency;
		friend class Patch;
	
	public:
		/**
		 * @about  Constructor the create a new System (basically a new Database) from a
		 *         set of pathes that contain the Modules belonging to the system.
		 * @param  sName The name of the System. Can be empty.
		 * @param  vDirs The directory oathes this System reads all of its Module objects from.
		 * @param  sTempDir Currently unused. Reserved for future use.
		 * @param  pProgressRoutine A callback routine that returns a bool and takes two unsigned
		 *         integers and a string as an argument. The first integer is the count of modules
		 *         already processed. The second integer is the total module count. The string
		 *         is the name of the module currently processed. The return values indicates whether
		 *         the creation should be continued (true) or aborted (false).
		 * @see    Reader, archive()
		 */
		System(std::string sName, std::vector<std::string> vDirs, std::string sTempDir, SYSTEM_CREATE_PROGRESS_ROUTINE pProgressRoutine = NULL);
		
		System();

		virtual ~System();
		
	public:
		/**
		 * @about  Obtain a module with a specific name. The module name is the filename
		 *         without extension.
		 * @param  sName The name of the module to be retreived (case-insensitive)
		 * @return The Module, or Null, if the Module could not be found.
		 * @see    addModule()
		 */
		const Module* getModule(std::string sName) const;
		
		/**
		 * @about  Get all dependencies belonging to the System.
		 * @return Const reference to the m_lsDependencies member.
		 * @see    addDependency()
		 */
		const std::list<Dependency*>& getDependencies() const;
		
		/**
		 * @about  Get all modules belonging to the System.
		 * @return Const reference to the m_mapModuleNames member.
		 */
		const std::map<std::string, Module*>& getModules() const;
		
		/**
		 * @about  Currently not used.
		 */
		const std::string& getTempDir() const {return m_sName;}
		
		/**
		 * @about  Obtain all include directories of the System.
		 * @return const vector of pathes.
		 * @see    m_vDirs
		 */
		const std::vector<std::string> getDirs() const {return m_vDirs;}
	
		/**
		 * @about  Obtain the name of the System.
		 * @return The name of the System. Migth be empty.
		 */
		virtual std::string getName() const {return m_sName;}
		
		/**
		 * @about  Tells if this system is currently being destroyed.
		 * @return True, if the destructor of the system is curently in execution, false if otherwise.
		 */
		bool getDestroyed() const {return m_bDestroyed;}
		
		/**
		 * @about  Compiles a set of the names of all modules that are currently missing in the database.
		 * @return Returns the compiled set.
		 */
		std::set<std::string> getMissingModuleNames();
		
	public:
		/**
		 * @about  Add a dependency to the System, with a soure module instance, a target
		 *         module name, and a list of names of exported functions required from the module.
		 * @param  pFrom The module the dependency originates from. Must not be Null.
		 * @param  sTo The name of the target module. The module does not necessarily have to exist.
		 * @param  lsExports The undecorated names of the exports required from the target module.
		 * @return The new dependency object, or an existent one, if a Dependency from pFrom to sTo
		 *         already existed. In that case, the export names in lsExports will NOT be added
		 *         to the existing dependency.
		 * @see    getDependencies()
		 */
		Dependency* addDependency(Module* pFrom, std::string sTo, std::list<std::string> lsExports = std::list<std::string>());
		
		/**
		 * @about  Add a module for a specific module path. A new module Object
		 *         will be created from the path if it exists, and a Module
		 *         with the specified file name does not already exist.
		 *         The new module object will then be inserted into the System.
		 * @param  sPath Path to the module that is to be represented by a new
		 *         Module object. mUst be a path to an exe or dll file.
		 * @return void
		 * @see    getModule()
		 */
		Module* addModule(std::string sPath);
		
		/**
		 * @about  Add a module for a specific module name and a dummy path.
		 *         A new module Object will be created from the path if it exists, and a Module
		 *         with the specified name if it does not already exist.
		 *         The new module object will then be inserted into the System.
		 * @param  sPath Path to the module that is to be represented by a new
		 *         Module object. mUst be a path to an exe or dll file.
		 * @return void
		 * @see    getModule()
		 */
		Module* addEmptyModule(std::string sName, std::string sDummyPath);
		
		void linkModule( Module* pModule ) const;

		/**
		 * @about  Obtain all dependencies that require a specific module
		 *         that is not yet added by the name of that module.
		 * @param  sModule The name of the missing module (case-insensitive).
		 * @return List of the dependencies that require a module by the name of sModule.
		 * @see    Dependency::setModule()
		 */
		std::list<Dependency*>& getDependenciesForMissingModuleName(std::string sModule) const;
		
		/**
		 * @about  Store the System in an XML archive.
		 * @param  sPath Path where the XML file should be saved.
		 *         If omitted, the document will be saved under the location
		 *         the System object was loaded from.
		 * @return void
		 * @see    Reader::setModuleCount() 
		 */
		void archive(std::string sPath = "");
		
		/**
		 * @about  Set the reader that currently loads the System.
		 *         Called by the reader for the System to inform the Reader
		 *         in its deserialize() method about the total module/dependency counts.
		 * @param  pReader The reader object.
		 * @return void
		 * @see    Reader::setModuleCount(), Reader::setDependencyCount()
		 */
		void setReader(const Reader* pReader) {m_pReader = pReader;}
		
		/**
		 * @about  Utility method that returns the absolute pathes of all binaries in a specific directory.
		 * @param  [in] sDirectory The directory to be searched.
		 * @param  [out] lstBinaries A reference to the list the binary pathes found are appended to.
		 */
		int interpretBinaryPath( __in std::string sDirectory, __out std::list<std::string>& lstBinaries ) const;
		bool isAcceptableBinaryExtension( const char* sExt ) const;
		
		const std::list<CyclicReference*>& getCyclicReferences();
		
		void System::updateCyclicRefs();
	
	private:
		/** Tells if this system is currently being destroyed. */
		bool m_bDestroyed;
		
		/** Called by the constructor. */
		void setDestroyed() { m_bDestroyed = true; }
		
		/** The name of the System. */
		std::string m_sName;
		
		/** Not used. */
		std::string m_sTempDir;
		
		/** The directories this System is reading its binaries from. */
		std::vector<std::string> m_vDirs;
		
		/** Count of modules in the System */
		unsigned int m_nModules;
		
		/** Count of dependencies in the System */
		unsigned int m_nDependencies;
		
		/** Map that points from the lowercase name of a module to the Module instance. */
		mutable std::map<std::string, Module*> m_mapModuleNames;
		
		/** List of all the dependencies belonging to the System. */
		mutable std::list<Dependency*> m_lsDependencies;
		
		/** Map that points from the name of a missing module to a list of all the dependencies that require it. */
		mutable std::map<std::string, std::list<Dependency*>> m_mapMissingModuleDependencies;
		
		/** The reader object this System was created with. Might be dangling! Only safe to use in the System ctor. */
		const Reader* m_pReader;
		
		/** List of cyclic references. See updateCyclicRefs, getCyclicRefs.*/
		std::list<CyclicReference*> m_lsCyclicRefs;
		
		/** Private const version of addDepenency() that can be called by the Module. */
		const Dependency* addDependency(Module* pFrom, std::string sTo, std::list<std::string> lsExports = std::list<std::string>()) const; //< Called by the module
		
		/** Finds and sets the given dependencies target module, and will add the Dependency
		    to the source modules outgoing and the target modules incoming dependencies. */
		void linkDependency(Dependency* pDependency ) const;
		
		/**  */
		void addDependencyForMissingModule( __notnull Dependency* pDependency ) const;

		/**  */
		void removeMissingModuleDependency( Dependency* pDependency );

		/**  */
		void unlinkModule(__notnull Module* pModule) const;

	public:
		virtual void serialize(ISerializer *encoder);		
		virtual void deserialize(IDeserializer *decoder);
		virtual std::string getClassName() const {return "System";}
	};
}

#endif