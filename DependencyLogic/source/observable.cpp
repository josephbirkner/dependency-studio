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

#include "../GlobExport/observable.h"
#include "../GlobExport/observer.h"

std::string regex_escape(const std::string& re) {
	const boost::regex esc("[.^$|()\\[\\]{}*+?\\\\]");
	const std::string rep("\\\\&");
	return regex_replace(re, esc, rep, boost::match_default | boost::format_sed);
}

namespace DependencyLogic
{
	Observable::~Observable()
	{
		for each(const IQueryObserver* pQuery in m_lsObservers)
			pQuery->invalidate(const_cast<const Observable*>(this));
	}

	void Observable::addObserver( const IQueryObserver* pQuery )
	{
		m_lsObservers.push_back(pQuery);
	}

	bool Observable::isApplicable( const IQueryObserver* pObserver )
	{
		if(pObserver->getSearchTerm().empty())
			return true;
		
		std::string searchTerm = regex_escape(pObserver->getSearchTerm());
		if(pObserver->getSearchWord())
			searchTerm = std::string("\\b") + searchTerm + "\\b";
		regex re(searchTerm, regex::ECMAScript | ( pObserver->getConsiderCase() ? 0 : regex::icase ) );
		return regex_search( getName(), re );
	}
}
