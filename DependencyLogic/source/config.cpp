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

#include "StdAfx.h"

#pragma hdrstop

#include "../GlobExport/config.h"

#include "DynLayout/GlobExport/profile.h"

using namespace boost;

namespace DependencyLogic
{
	std::vector<std::string> Config::getRecentDatabases()
	{
		char vchRecentList[5000];
		vchRecentList[0] = '\0';
		Layout::Registry::getInstance()->readString("DependencyStudio", "RecentList", vchRecentList, 5000, "");
		std::vector<std::string> vRecentList;
		algorithm::split(vRecentList, std::string(vchRecentList), algorithm::is_any_of(";"));
		return vRecentList;
	}

	void Config::addRecentDatabase( std::string sPath )
	{
		std::vector<std::string> lsRecent = getRecentDatabases();
		if (!PathFileExists(sPath.c_str()))
			DebugBreak();
		
		// Remove the entry if it already existed
		for(std::vector<std::string>::iterator it = lsRecent.begin(); it != lsRecent.end(); )
			if(*it == sPath)
				it = lsRecent.erase(it), it == lsRecent.end();
			else
					++it;

		lsRecent.insert(lsRecent.begin(), sPath);
		if (lsRecent.size() > getRecentListMaxLength())
			lsRecent.resize(getRecentListMaxLength());
		
		std::string sRecentList = algorithm::join(lsRecent, std::string(";"));
		Layout::Registry::getInstance()->writeString("DependencyStudio", "RecentList", sRecentList.data());
	}

	bool Config::getAllowWrite()
	{
		long lAllowWrite = FALSE;
		Layout::Registry::getInstance()->readLong("DependencyStudio", "AllowWrite", lAllowWrite, 0);
		return lAllowWrite != 0;
	}

	unsigned int Config::getRecentListMaxLength()
	{
		long lMaxRecentCount = 0;
		Layout::Registry::getInstance()->readLong("DependencyStudio", "RecentListMaxLength", lMaxRecentCount, 10);
		return (unsigned int) lMaxRecentCount;
	}

	bool Config::getUseDumpbin()
	{
		return true;
		long lUseDumpbin = 0;
		Layout::Registry::getInstance()->readLong("DependencyStudio", "UseDumpbin", lUseDumpbin, 10);
		return lUseDumpbin != 0l;
	}

	long Config::getUserColWidth( int nIndex, long lDefault )
	{
		char vchColWidth[5000];
		vchColWidth[0] = '\0';
		Layout::Registry::getInstance()->readString("DependencyStudio", "UserColWidth", vchColWidth, 5000, "");
		
		std::vector<std::string> vsColWidths;
		algorithm::split(vsColWidths, std::string(vchColWidth), algorithm::is_any_of(","));
		
		if(vsColWidths.size() > nIndex && nIndex >= 0 && !vsColWidths[nIndex].empty())
			return lexical_cast<long>(vsColWidths[nIndex]);
			
		return lDefault;
	}

	void Config::setUserColWidth( int nIndex, unsigned long lSize )
	{
		char vchColWidth[5000];
		vchColWidth[0] = '\0';
		Layout::Registry::getInstance()->readString("DependencyStudio", "UserColWidth", vchColWidth, 5000, "");
		
		std::vector<std::string> vsColWidths;
		algorithm::split(vsColWidths, std::string(vchColWidth), algorithm::is_any_of(","));
		
		if(vsColWidths.size() > static_cast<size_t>(nIndex))
			vsColWidths[nIndex] = lexical_cast<std::string>(lSize);
		else if(vsColWidths.size() == nIndex)
			vsColWidths.push_back(lexical_cast<std::string>(lSize));
			
		Layout::Registry::getInstance()->writeString("DependencyStudio", "UserColWidth", algorithm::join(vsColWidths, std::string(",")).c_str());
	}

	std::string Config::getLogFilePath()
	{
		return "";
	}
}
