#include "StdAfx.h"

#pragma hdrstop

#include "../GlobExport/errorlist.h"

namespace DependencyLogic
{
	ErrorList::ErrorList()
	{
	}

	ErrorList::~ErrorList()
	{
	}

	ErrorList& ErrorList::operator=( ErrorList& other)
	{
		m_lsErrors.clear();
		*this += other;
		return *this;
	}

	ErrorList& ErrorList::operator+=( ErrorList& other)
	{
		for each(const ErrorListEntry& entry in other.m_lsErrors)
			insert(entry);
			
		return *this;
	}
	
	ErrorList& ErrorList::operator-=( ErrorList& other)
	{
		for each(const ErrorListEntry& entry in other.m_lsErrors)
		{
			std::set<ErrorListEntry>::iterator itEntry = m_lsErrors.find(entry);
			if(itEntry != m_lsErrors.end())
				m_lsErrors.erase(itEntry);
		}
		
		return *this;
	}

	std::string ErrorList::getString()
	{
		std::string sRet;
		for each(const ErrorListEntry& entry in m_lsErrors)
			sRet += (std::string) entry;
			
		return sRet;
	}

	int ErrorList::getCount()
	{
		return m_lsErrors.size();
	}

	void ErrorList::insert( std::string sEntry )
	{
		m_lsErrors.insert(sEntry);
	}
}
