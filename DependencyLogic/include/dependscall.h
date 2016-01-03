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

#ifndef _DEPENDENCYCONTROL_DEPENDSCALL_H_
#define _DEPENDENCYCONTROL_DEPENDSCALL_H_

#include "StdAfx.h"

namespace DependencyLogic
{
	/**
	 * @about Functor that makes a call to Depends for a specific module path, and evaluates the
	 *        output, writing the modules exported functions in a list, and for each
	 *        module imported by that module the name of the imported module and the
	 *        list of function names required from the imported module into a map.
	 * @see   DumpbinCall, Module::setPath()
	 */
	class DependsCall
	{
	public:
		/**
		 * @about  Functor constructor. Will call initCmdLine, run, and parseResult in that order.
		 *         An assertion will be issued, when sModulePath does not exist.
		 * @param  sModulePath The path to the module to be processed.
		 * @see    getExports(), getImportedModuleFunctions()
		 */
		DependsCall(std::string sModulePath);
		DependsCall(){;}

		~DependsCall();
		
		/**
		 * @about  Returns the list of export names provided by the analyzed module.
		 * @return Const list of the export names, in an undecorated form.
		 */
		const std::list<std::string>& getExports() {return m_lsExports;}
		
		/**
		 * @about  Returns a map, pointing from the name of an imported module to a
		 *         list of functions required from that module.
		 * @return const map<string, list<string>>
		 */
		const std::map<std::string, std::list<std::string>>& getImportedModuleFunctions() {return m_mapImportedModuleFunctions;}
		
	protected:
		/** The command line that will be executed in the run() method and built in the initCmdLine() method. */
		std::string m_sCmdLine;
		
		/** Path to the output file, presumably within the configured temp directory. */
		std::string m_sOutputFile;
		
		/** Path to the module to be analyzed, passed in the ctor. */
		std::string m_sModule;
		
		/** The list of exports, filled in parseResult(). */
		std::list<std::string> m_lsExports;
		
		/** The map of imported module name function names, filled in parseResult(). */
		std::map<std::string, std::list<std::string> > m_mapImportedModuleFunctions;
		
		/** The success flag. */
		bool m_bSuccess;
		
		void execute( std::string &sModulePath );
		
		/**
		 * @about  Initializes the commnand line m_sCmdLine member for the call to Depends.
		 * @param  sModulePath Path to the module to be processed.
		 * @return void
		 * @see    run()
		 */
		virtual void initCmdLine( std::string sModulePath );
		
		/**
		 * @about  Executes the command line via CreateProcess, waits for it via WaitForSingleObject,
		 *         and verifies that the output file under m_sOutputFile has been written.
		 * @return True if successfull, false if otherwise.
		 * @see    initCmdLine()
		 */
		virtual bool run();
		
		/**
		 * @about  Parses the output file, filling the m_lsExports and m_mapImportedModuleFunctions members.
		 * @return void
		 * @see    parseLine(), trimEntryFlags(), readLibraryName(), reduceToFunctionName(), countIndents()
		 */
		virtual void parseResult();
		
		/**
		 * @about  Parses a line from the given FILE into the given string.
		 *         Automatically recognizes EOF, can work with both \n and \r\n.
		 * @param  pFile The file to read a line from.
		 * @param  sResult The variable to store the line read from pFile.
		 *         Will not contain the line break/return character anymore.
		 * @return void
		 * @see    parseResult()
		 */
		inline void parseLine(__in FILE*& pFile, __out std::string& sResult);
		
		/**
		 * @about  Trims the entry flags (a code of characters in quare brackets, like "[xyz]")
		 *         prepended to each entry in the Depends output import/export function listing.
		 * @param  sLine The line the flags should be trimmed from.
		 * @return The entry flags trimmed from the line.
		 * @see    parseLine()
		 */
		std::string trimEntryFlags(__inout std::string& sLine);
		
		/**
		 * @about  Reads the name of a library from a line containing entry flags and a path to the library.
		 * @param  sLine The line that contains the specified data.
		 * @param  sName Variable to store the library name in read from the line.
		 * @return void
		 * @see    trimEntryFlags()
		 */
		void readLibraryName( __inout std::string& sLine, __out std::string& sName );
		
		/**
		 * @about  Removes any unwanted spaces from the line and reduces its content to the function name.
		 * @param  sLine The line containing the function name, all other content will be removed.
		 * @return void
		 */
		void reduceToFunctionName(__inout std::string& sLine);
		
		/**
		 * @about  Counts the indents of the line. Five prepended spaces will be interpretes as one indent.
		 * @param  sLine The line to count the indents of.
		 * @return The number of times sLine has been indented.
		 */
		int countIndents(const std::string& sLine);
		
		/**
		 * @about  Checks whether the given string contains only decimal digits (0-9).
		 * @param  s The string to be checked.
		 * @return True, if only digits were found, false if otehrwise.
		 */
		bool isNumeric(const std::string s);
	};
}

#endif