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

#ifndef _DEPENDENCYCONTROL_DEPENDENCY_H_
#define _DEPENDENCYCONTROL_DEPENDENCY_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

using namespace Archiving;

#include "dependency.h"
#include "export.h"
#include "module.h"

namespace DependencyLogic
{
	/**
	 * @about  Struct to manage the reference of a Dependency to a specific exported function.
	 *         Dependency instances wil hold a list of ExportRef objects for each Export they
	 *         require from theire target Module
	 *         
	 *         ExportRefs belong to the persistence level and are representable by QueryNodes,
	 *         therefore they inherit from Observable.
	 * @see    Dependency, Export
	 */
	struct DEPENDENCYLOGIC_API ExportRef : public Observable
	{
		friend class Dependency;
	
	public:
		/**
		 * @about  Constructor to create an ExportRef object
		 *         from the name of a required exported function.
		 * @param  s The name of the required Export.
		 * @see    sExport
		 */
		ExportRef(std::string s) : sExport(s), pExport(NULL) {}
		
		/**
		 * @about  Operator to retreive the value of the private sExport field.
		 * @return The value of sExport as a const reference.
		 * @see    sExport, operator const Export*()
		 */
		operator const std::string& () const {return sExport;}
		
		/**
		 * @about  Operator to retreive the value of the private pExport field.
		 * @return The const Export* that was resolved from sExport, or Null, if
		 *         No appropriate Export object could be found.
		 * @see    Export, Dependency::refExports(), Dependency::unrefExports()
		 */
		operator const Export*() const {return pExport;}
		
		virtual void serialize(ISerializer *encoder) {;}
		virtual void deserialize(IDeserializer *decoder) {;}
		virtual std::string getName() const {return sExport;}
		
	private:
		/** The name of the required Export. */
		std::string sExport;
		
		/** The actual instance of the required Export, or Null, if it couldn't be found. */
		const Export* pExport;
	};
	
	/**
	 * @about Persistence level class representig a dependency between two modules.
	 *        The dependency knows the modules involved (source module and target module if available),
	 *        as well as the names (and instances, if available) of the exports
	 *        the source module requires from the target module, as a list of
	 *        ExportRef objects.
	 * @see   ExportRef, Module
	 */
	class DEPENDENCYLOGIC_API Dependency : public Subject, public IInstanceCounter<Dependency>
	{
		friend class System;
		friend class IDeserializer;
		friend class Module;
	
	private:
		/**
		 * @about  Constructor. Must only be called by the System.
		 * @param  pFrom The Module that requires the Dependency.
		 * @param  sTo The name of the module pFrom is dependent on.
		 * @param  vExports The names of the functions pFrom requires to be exported from sTo.
		 * @see    System::addDependency()
		 */
		Dependency(Module* pFrom, std::string sTo, std::list<std::string> vExports);
		
		/**
		 * @about  Destructor. Will remove itself from the incoming references of the
		 *         target module, and the outgoing references of the source module,
		 *         and the referencing dependencies of the identified exports.
		 * @see    Export::removeDependency
		 */
		virtual ~Dependency();
		Dependency();
				
	public:
		/**
		 * @about  Set the target module of the dependency.
		 *         The modules name must match, an assertion will be raised if not.
		 *         If the module was set to a different instance previously, the unrefExports() method
		 *         will be called, and then refExports() called again on the new instance.
		 * @param  pModule The Module instance this dependency points to.
		 * @return void
		 * @see    getTargetModule(), getTargetModuleName(), refExports() unrefExports()
		 */
		void setTargetModule(const Module* pModule);
	
		/**
		 * @about  Get the name of this dependencies target module.
		 * @return The name of the target module, as issued in the ctor.
		 * @see    setTargetModule()
		 */
		const std::string& getTargetModuleName() const {return m_sTo;}
		
		/**
		 * @about  Get the target module instance, must have been set via setTargetModule().
		 * @return The Module instance, or Null, if the target module name could not be reolved.
		 * @see    setTargetModule()
		 */
		const Module* getTargetModule() const {return m_pTo;}
		
		/**
		 * @about  Obtain the source module instance this dependency originates from.
		 * @return The source module instance. Not null.
		 */
		const Module* getSourceModule() const {return m_pFrom;}
		
		/**
		 * @about  Called by the export destructor to inform the dependency that the
		 *         Export does not exist anymore.
		 * @param  pExport The Export instance this Dependency is supposed to forget about.
		 *         The pExport member of the appropriate ExportRef entry in m_lsExports will
		 *         be set to Null.
		 * @return void
		 * @see    refExports(), unrefExports(), ExportRef
		 */
		void removeExport(const Export* pExport) const; //< called by the export dtor
		
		/**
		 * @about  Retreive all ExportRef objects of this Dependency.
		 * @return Const list of ExportRef objects.
		 * @see    ExportRef
		 */
		const std::list<ExportRef>& getExports() const {return m_lsExports;}
		
		/**
		 * @about  Retreive the name of this Dependency.
		 *         The name is the combination of SourceModuleName + " -> " + TargetModuleName.
		 * @return String with the specified contents.
		 * @see    Observable::getName()
		 */
		virtual std::string getName() const {return getSourceModule()->getName() + " -> " + getTargetModuleName();}
		
		/**
		 * @about  Called by the system, to check whether the dependency is already registered
		 *         as missing its target module.
		 * @return True if already registered, false if unregistered.
		 * @see    setSystemRegisteredMissingTargetModule
		 */
		bool getSystemRegisteredMissingTargetModule() const { return m_bSystemRegisteredMissingTargetModule; }
		
		/**
		 * @about  Called by the system, to set whether the dependency is already registered
		 *         as missing its target module.
		 * @param  val True if already registered, false if unregistered.
		 * @see    setSystemRegisteredMissingTargetModule
		 */
		void setSystemRegisteredMissingTargetModule(bool val) { m_bSystemRegisteredMissingTargetModule = val; }
	
	private:
		/** The module the dependency originates from. */
		const Module* m_pFrom;
		
		/** The name of the dependencies target module */
		std::string m_sTo;
		
		/** The target module instance. Might be Null. */
		const Module* m_pTo;
		
		/** The list of ExportRef objects that make up this Dependency. */
		mutable std::list<ExportRef> m_lsExports;
		
		/** Tells whether the dependency is already registered as missing its target module. */
		bool m_bSystemRegisteredMissingTargetModule;

		/**
		 * @about  Dereferences the ExportRef objects. (for each none-Null pExport member
		 *         of an ExportRef entry in m_lsExports, calls Export::removeDependency() and sets the
		 *         pExport member to Null.
		 * @return void
		 * @see    refExports(), ExportRef
		 */
		void unrefExports();
		
		/**
		 * @about  Tries to determine an Export object for each ExportRef in m_lsExports
		 *         by getting an Export object with the ExportRef.sExport name from the
		 *         target module and assigning it to ExportRef.pExport. This of course only
		 *         works, if setTargetModule() has been called, and an actual target module
		 *         has been set.
		 * @return void
		 * @see    unrefExports(), ExportRef, setTargetModule()
		 */
		void refExports();
		
	/* Implementation of the IArchivableObject Interface
	 */
	public:
		virtual void serialize(ISerializer *encoder);
		virtual void deserialize(IDeserializer *decoder);
		virtual std::string getClassName() const {return "Dependency";}
	};
}

#endif