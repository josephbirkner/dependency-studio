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

#ifndef _DEPENDENCYCONTROL_MODULE_H_
#define _DEPENDENCYCONTROL_MODULE_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

using namespace Archiving;

namespace DependencyLogic
{
	class Dependency;
	class Export;
	class System;
	class ErrorList;

	/**
	 * @about Persistence level class that represents a Module (EXE or DLL). The module knows
	 *        about all of its exported functions, all of its incoming dependencies and all of
	 *        its outgoing dependencies. It is created and managed by the System it belongs to.
	 * @see   System, Dependency, Export
	 */
	class DEPENDENCYLOGIC_API Module : public Subject
	{
		friend class System;
		friend class IDeserializer;
	
	private:
		/**
		 * @about  Private constructor, only to be used by the System. Create a module
		 *         from an EXE or DLL path, with the System it belongs to.
		 * @param  sOrigPath The path to the binary module the new Module object should represent.
		 *         setCurrentPath() will be called with the value of sOrigPath.
		 * @param  pSystem The System the new module should belong to.
		 * @see    setCurrentPath()
		 */
		Module(std::string sOrigPath, const System* pSystem); // Use System::addModule()
		Module(std::string sName, std::string sOrigPath, const System* pSystem); // Use System::addEmptyModule()
		
		virtual ~Module();
		Module();
		
		void removeIncomingDependency( const Dependency* pDependency ) const; //< Called by the owning System

	public:
		/**
		 * @about  Add a dependency to the list of incoming dependencies
		 *         requiring this Module as theire target.
		 * @param  pDependency The Dependency to be added. Whether it already has been
		 *         added will not be checked!
		 * @see    Dependency, getIncomingDependencies()
		 */
		void addIncomingDependency(const Dependency* pDependency) const; //< Called by the owning System
		
		/**
		 * @about  Add a dependency to the list of outgoing dependencies
		 *         requiring functionality from a different module.
		 * @param  pDependency The Dependency to be added. Whether it already has been
		 *         added will not be checked!
		 * @see    Dependency, getOutgoingDependencies()
		 */
		void addOutgoingDependency(const Dependency*) const; //< Called by the owning System
		
		/**
		 * @about  Set the current path of the module. The path should point
		 *         to a file that matches the name the module already has.
		 *         All incoming Dependencies will be unreferenced. All outgoing Dependencies
		 *         will be deleted. A DumpbinCall will then be made, with the new path as
		 *         the target. New export objects will be created. The Ougoing references will
		 *         be recreated. The refExports() will be called on each incoming Dependency.
		 * @param  sPath The alternate module path. Will not overwrite the original path,
		 *         which can still be subsequently retreived using the getOrigPath() method.
		 * @return void
		 * @see    getName(), getCurrentPath(), getOrigPath(), DependsCall, DumpbinCall
		 */
		void setCurrentPath(std::string sPath);
		
		/**
		 * @about  Draw exports this module is supposed to have from another module,
		 *         that might originate from a different system. The module will retry
		 *         to resolve its incoming dependencies afterwards.
		 * @param  pOther The module whose exports are to be adopted.
		 * @return void
		 * @see    getName(), getCurrentPath(), getOrigPath(), DependsCall, DumpbinCall
		 */
		void adaptExportsAndResolveErrors(Module const* pOther);

		/**
		 * @about  Calls getSystem()->unlinkModule(this) and clearExports().
		 * @return void 
		 * @see    System::unlinkModule(), clearExports()
		 */
		void clear();

		/**
		 * @about  Clears the Export list (Deletes all of this Module's Exports).
		 * @return void
		 */
		void clearExports();

		/**
		 * @about  Retreive the incoming Dependencies (Dependencies that require Exports from this module.
		 * @return Const list of the incoming dependencies.
		 * @see    addIncomingDependency(), Dependency
		 */
		const std::list<const Dependency*>& getIncomingDependencies() const {return m_lsIncoming;}
		
		/**
		 * @about  Retreive the incoming Dependencies (Dependencies that require Exports from this module).
		 * @return Const list of the incoming dependencies.
		 * @see    addIncomingDependency(), Dependency
		 */
		const std::list<const Dependency*>& getOutgoingDependencies() const {return m_lsOutgoing;}
		
		/**
		 * @about  Retreive the EXE/DLL path this Module object
		 *         is currently reading its Dependencies/Exports from.
		 * @return The current path. Might be the same as getOrigPath(), if
		 *         setCurrentPath() has not been called.
		 * @see    setCurrentPath(), getOrigPath()
		 */
		const std::string& getCurrentPath() const {return m_sCurrentPath;}
		
		/**
		 * @about  Path to the original version of the module within the systems
		           directories, as issued in the ctor.
		 * @return The original path. Is not necessarily the current one.
		 * @see    getCurrentPath()
		 */
		const std::string& getOrigPath() const {return m_sOrigPath;}
		
		/**
		 * @about  Obtain an Export of this Module by its undecorated name.
		 * @param  Dependency objects will try to determine the Export objects
		 *         for theire required Functions using this method.
		 * @return The Export object, or Null, if no Export with a matching name was found.
		 * @see    Export::getName(), ExportRef, Dependency::refExports()
		 */
		const Export* getExport(std::string sName) const {return m_mapExportNames[sName];}		
		
		/**
		 * @about  Get all exports of this module.
		 * @return Const list of Export objects.
		 * @see    setCurrentPath()
		 */
		const std::map<std::string, Export*>& getExports() const {return m_mapExportNames;}
		
		/**
		 * @about  Obtain the name of the module. The name is determined from the
		 *         module path issued in the destructor, without direcories or file
		 *         extension.
		 * @return The name of the module.
		 * @see    Observable::getName()
		 */
		virtual std::string getName() const {return m_sName;}
		
		/**
		 * @about  Returns the list of errors (missing exports)
		 *         in dependencies from and to this module.
		 * @return The ErrorList containing the errors.
		 */
		ErrorList Module::getErrors();
		
		/**
		 * @about  Copies the file from m_sCurrentPath to m_sOrigPath.
		 *         This will set the current path back to the orig path afterwards.
		 *         
		 *         NOT IMPLEMENTED.
		 * @return void.
		 */
		void copyFromCurrentPathToOrigPath();
		
		bool getInSearchStack() const { return m_bInSearchStack; }
		void setInSearchStack(bool val) { m_bInSearchStack = val; }
		
	private:
		/** The name of the module (orig path w/o directories or file extension). */
		std::string m_sName;
		
		/** The original path of the module */
		std::string m_sOrigPath;
		
		/** The path this module is currently using. */
		std::string m_sCurrentPath;
		
		/** The dependencies requiring functions from this module. */
		mutable std::list<const Dependency*> m_lsIncoming;
		
		/** The dependencies this module requires. */
		mutable std::list<const Dependency*> m_lsOutgoing;
		
		/** Map pointing from an undecorated export name to the appropriate Export object. */
		mutable std::map<std::string, Export*> m_mapExportNames;
		
		/** Indicates, whether the observable object is currently an entry in the CyclicReferenceDetection module stack. */
		bool m_bInSearchStack;

	/* Implementation of the IArchivableObject Interface
	 */
	public:
		virtual void serialize(ISerializer *encoder);
		virtual void deserialize(IDeserializer *decoder);
		virtual std::string getClassName() const {return "Module";}
	};
}

#endif