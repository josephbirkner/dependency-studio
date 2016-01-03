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

#ifndef _DEPENDENCYCONTROL_DUMPBINCALL_H_
#define _DEPENDENCYCONTROL_DUMPBINCALL_H_

#include <StdAfx.h>

#include "dependscall.h"

namespace DependencyLogic
{
	/**
	 * @about Specializes the functionality of DependsCall to work for Dumpbin.
	 *        Dumpbin works almost hundred-fold faster, producing the desired output with
	 *        much less overhead.
	 * @see   DependsCall
	 */
	class DumpbinCall : public DependsCall
	{
	public:
		/**
		 * @about  Functor constructor. Will call initCmdLine, run, and parseResult in that order.
		 *         An assertion will be issued, when sModulePath does not exist.
		 * @param  sModulePath The path to the module to be processed.
		 * @see    getExports(), getImportedModuleFunctions()
		 */
		DumpbinCall(std::string sModulePath);
	
	protected:
		/**
		 * @about  Initializes the m_sCmdLine member to call Dumpbin instead of Depends.
		 * @param  sModulePath The path to the module to be analyzed.
		 * @return void
		 * @see    run()
		 */
		virtual void initCmdLine( std::string sModulePath );
		
		/**
		 * @about  Parses the output of Dumpbin.
		 * @return void
		 * @see    parseLine()
		 */
		virtual void parseResult();
		
		enum FileType {TypeExe, TypeDll} m_nType;
	};
}

#endif
