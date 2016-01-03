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

#ifndef _DEPENDENCYCONTROL_CONFIG_H_
#define _DEPENDENCYCONTROL_CONFIG_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

namespace DependencyLogic
{	
	
	/**
	 * @about Wraps calls to various Profile attributes (Recentlist, RecentListMaxLength, TempPath, AllowWrite)
	 *        with the appkey "DependencyStudio".
	 */
	class DEPENDENCYLOGIC_API Config
	{
	public:
		Config(){;}
		~Config(){;}
		
		/**
		 * @about  Returns the temporary path, where the Depends/Dumpbin output should be stored.
		 * @return The value of the DependencyStudio.TempPath config key.
		 * @see    DependsCall, DumpbinCall
		 */
		std::string getTempPath() {return "E:\\Dependencies\\";}
		
		/**
		 * @about  Returns whether the user is allowed to write the database.
		 * @return The value of the DependencyStudio.AllowWrite config key.
		 * @see    System::archive()
		 */
		bool getAllowWrite();
		
		/**
		 * @about  Returns a list of recently used databases. The most recently used
		           will be at the top of the list.
		 * @return The value of the DependencyStudio.RecentList config key, exploded
		 *         by the semicolon character.
		 * @see    addRecentDatabse().
		 */
		std::vector<std::string> getRecentDatabases();
		
		/**
		 * @about  Add a value to the list of recently used databases.
		 *         If the length of the list exceeds DependencyStudio.RecentListMaxLength,
		 *         then the unwanted entries will be popped of the end of the list
		 * @param  sPath The path to be appended
		 * @return void
		 * @see    getRecentDatabases()
		 */
		void addRecentDatabase(std::string sPath);
		
		/**
		 * @about  Returns the maximum number of entries in the list of recently used databases.
		 * @return The value of the DependencyStudio.RecentListMaxLength config key.
		 * @see    getRecentDatabases(), addRecentDatabse()
		 */
		unsigned int getRecentListMaxLength();
		
		/**
		 * @about  Returns whether Dumbin or Depends should be used for Module analysis.
		 * @return True if dumpbin should be used, false if otherwise.
		 * @see    DumpbinCall
		 */
		bool getUseDumpbin();
		
		/**
		 * @about  Returns the user-configured size of the specified column, or lDefault,
		 *         if the size for the column at nIndex has not been configured.
		 * @param  nIndex The zero-based index of the column whose width is to be retreived.
		 * @param  lDefault The value to be returned if nIndex exceeds the current length of the configured column sizes.
		 * @return The size of the column at nIndex in pixels, or lDefault, if not configured.
		 * @see    setUserColWidth
		 */
		long getUserColWidth(int nIndex, long lDefault);
		
		/**
		 * @about  Set the user-configured size of the specified column.
		 * @param  nIndex The zero-based index of the column whose width is to be set.
		 *         The value of (index+1) can exceed the length of the width list by one,
		 *         whereas one size will be appended to the end of it.
		 * @param  lSize The size of the column.
		 * @return void
		 * @see    DumpbinCall
		 */
		void setUserColWidth(int nIndex, unsigned long lSize);
		
		/**
		 * @about  Returns the path to the standard logfile where patch logs are to be saved.
		 * @return The patch. Currently fixed to Null.
		 * @see    Patch
		 */
		std::string	getLogFilePath();
	};
}

#endif