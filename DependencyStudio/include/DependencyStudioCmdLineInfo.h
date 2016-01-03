#include "StdAfx.h"
#pragma hdrstop

class CDependencyStudioCmdLineInfo : public CCommandLineInfo
{
public:
	virtual void ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast)
	{
		if(m_sXmlPath.empty())
			m_sXmlPath = pszParam;
		else if(!m_sCommand.empty())
			m_lsArguments.push_back(pszParam);
		else
			m_sCommand = pszParam;	
	}
	
	std::string& getXmlPath() {return m_sXmlPath;}
	
	std::string& getCommand() {return m_sCommand;}
	
	std::list<std::string>& getArgs(){return m_lsArguments;}
	
private:
	std::string m_sXmlPath;
	std::string m_sCommand;
	std::list<std::string> m_lsArguments;
	
	bool m_bError;
};
