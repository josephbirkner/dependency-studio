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

#ifndef _DEPENDENCYCONTROL_EXPORT_H_
#define _DEPENDENCYCONTROL_EXPORT_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "stdafx.h"

using namespace Archiving;

#include "dependency.h"
#include "module.h"

namespace DependencyLogic
{
	class Dependency;

	/**
	 * @about Persistence level class that represents an Export of a specific Module,
	 *        used by zero or more Dependencies.
	 *        It is identified by its string signature.
	 * @see   Dependency, Module, ExportRef
	 */
	class DEPENDENCYLOGIC_API Export : public Subject, public IInstanceCounter<Export>
	{
		friend class Module;
		friend class IDeserializer;
	
	private:
		/**
		 * @about  Private contructor to be called by Modules.
		 * @param  sName The name of the Export, in an undecorated, hman-readable form.
		 * @param  pModule The Module the new Export should belong to.
		 * @see    Module
		 */
		Export(std::string sName, const Module* pModule);
		
		Export();
		virtual ~Export();
		
	public:
		/**
		 * @about  Add a dependency as requiring this Export. Called by Dependency::refExports.
		 * @param  pDependency The Dependency to be added to the m_lsDependencies list.
		 * @return void
		 * @see    Dependency::refExports
		 */
		void addDependency(const Dependency* pDependency) const;
		
		/**
		 * @about  Remove a dependency requiring this Export. Called by Dependency::unrefExports.
		 * @param  pDependency The Dependency to be added to the m_lsDependencies list.
		 * @return void
		 * @see    Dependency::unrefExports
		 */
		void removeDependency(const Dependency*) const;
		
		/**
		 * @about  Set the Module this export belongs to.
		 * @param  pModule The Module this export belongs to.
		 * @return void
		 * @see    getModule
		 */
		void setModule(const Module* pModule);
	
		/**
		 * @about  Get the name of the Export, which is its undecorated Signature.
		 * @return The name (unique identifier) of the export.
		 * @see    Observable::getName, getSignature
		 */
		virtual std::string getName() const;
		
		
		/**
		 * @about  Get the signature of the Export.
		 * @return The signature of the export.
		 * @see    Observable::getName, getName
		 */
		virtual std::string getSignature() const {return m_sSignature;}
		
		/**
		 * @about  Retreive all dependencies referncing this export.
		 * @return Const reference to m_lsDependencies
		 * @see    addDependency
		 */
		const std::list<const Dependency*>& getDependencies() const {return m_lsDependencies;}
		
		/**
		 * @about  Get the module this Export belongs to.
		 * @return The module. Not Null.
		 * @see    setModule()
		 */
		const Module* getModule() const {return m_pModule;}
	
	private:
		/** The name of the Export in an undecorated form. */
		std::string m_sSignature;
		
		/** The module this Export originates from. */
		const Module* m_pModule;
		
		/** All Dependencies that require this Export. */
		mutable std::list<const Dependency*> m_lsDependencies;
	
	public:
		virtual void serialize(ISerializer *encoder);
		virtual void deserialize(IDeserializer *decoder);
		virtual std::string getClassName() const {return "Export";}
	};
}

#endif