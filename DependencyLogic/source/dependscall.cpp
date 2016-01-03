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

#include "stdafx.h"

#pragma hdrstop

// Local Includes
#include "../include/dependscall.h"
#include "../GlobExport/config.h"

namespace DependencyLogic
{
	DependsCall::DependsCall( std::string sModulePath )
	{
		execute(sModulePath);
	}
	
	void DependsCall::initCmdLine( std::string sModulePath )
	{
		m_sCmdLine = "depends /c /of:\""+m_sOutputFile+"\" \""+sModulePath+"\"";	
	}

	bool DependsCall::run()
	{
		STARTUPINFO si;
		::ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pi;

		if( ::CreateProcess(
				NULL,
				(LPTSTR)m_sCmdLine.data(),
				NULL,
				NULL,
				FALSE,
				NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW,
				NULL,
				NULL,
				&si,
				&pi
			)
		)
		{
			DWORD ret = WaitForSingleObject(pi.hProcess, 300000);

			if( ret == WAIT_TIMEOUT )
			{
				TerminateProcess( pi.hProcess, -1 );
				m_bSuccess = false;
			}
			else
				m_bSuccess = true;
			
			::CloseHandle(pi.hThread);
			::CloseHandle(pi.hProcess);
		}
		else
			return false;
			
		return m_bSuccess;
	}

	void DependsCall::parseResult()
	{
		enum ParseState
		{
			ParseHeader,
			ParseExportedFunctions,
			ParseImportedFunctions
		};
		
		typedef int ParseExit;
	
		FILE* pFile = fopen(m_sOutputFile.c_str(), "r");
		/*AFXASSUME(pFile);*/
		if(!pFile) return;
		
		std::string sLine = "";
		std::string sCurrentModule = "";
		ParseState nState = ParseHeader;
		int nEmptyLineCount = 0;
		
		bool bTerminate = false;
		int nTerminate = 0;
		
		try
		{
			while(!feof(pFile))
			{
				parseLine(pFile, sLine);
				
				switch( nState )
				{
					case ParseHeader:
					{
						if( sLine[0] == '[' )
						{
							readLibraryName(sLine, sCurrentModule);
							AFXASSUME( iequals(sCurrentModule, m_sModule) );
							nState = ParseExportedFunctions;
							nEmptyLineCount = 0;
						}
						break;
					}
					case ParseExportedFunctions:
					{
						if(sLine.empty())
						{
							nEmptyLineCount++;
							if( nEmptyLineCount == 3 )
							{
								nState = ParseImportedFunctions;
								nEmptyLineCount = 0;
							}
						}
						else
						{
							std::string sExportProps = trimEntryFlags(sLine);
							if( !sExportProps.empty() )
							{
								reduceToFunctionName(sLine);
								m_lsExports.push_back(sLine);
							}
						}
						break;
					}
					case ParseImportedFunctions:
					{
						if(sLine.empty())
						{
							if( bTerminate )
								throw (ParseExit) 0;
								
							nEmptyLineCount = ++nEmptyLineCount % 3;
							
							// two blank lines in a row will trigger termination
							if( nEmptyLineCount == 0 )
								bTerminate = true;
						}
						else
						{
							// parse new module
							if(nEmptyLineCount == 0)
							{
								// blank line followed by content
								bTerminate = false;
							
								// check for the module to be an import of this one.
								// it isn't, when it is indented further than 1 tab
								if( countIndents(sLine) == 1 )
									readLibraryName(sLine, sCurrentModule);
								else
									sCurrentModule = "";
							}
							else if( nEmptyLineCount == 1 && !sCurrentModule.empty() ) // parse imported functions
							{
								std::string sOptions = trimEntryFlags(sLine);
								if(!sOptions.empty())
								{
									reduceToFunctionName(sLine);
									if(!isNumeric(sLine)) // exclude imports by ordinal
										m_mapImportedModuleFunctions[sCurrentModule].push_back(sLine);
								}
							}
						}
					} // case
				} // switch
			}
		} // try
		catch(ParseExit&)
		{
		}
		
		fclose(pFile);
	}

	inline void DependsCall::parseLine( FILE*& pFile, std::string& sResult )
	{
		for( std::string ch = sResult = ""; !feof(pFile) && ch[0] != '\n'; sResult += ( (ch = fgetc(pFile))[0] != '\r' && ch[0] != '\n' ? ch : "" ) );
	}
	
	void DependsCall::readLibraryName( std::string& sLine, std::string& sName )
	{
		trimEntryFlags(sLine);
		sName = filesystem::path(sLine).filename().string();
	}
	
	/**
	 * Removes the prefixed "[...]" from the beginning of the string, and returns its content.
	 */
	std::string DependsCall::trimEntryFlags(std::string& sLine)
	{
		algorithm::trim(sLine);
		const char* pchLine = sLine.c_str();
		if(pchLine[0] == '[')
		{
			for(++pchLine; pchLine[0] != '\0' && pchLine[0] != ']'; ++pchLine);

			// Go one step further over the ']'
			return ++pchLine;
		}
		else
			return "";
	}
	
	void DependsCall::reduceToFunctionName(std::string& sLine)
	{
		algorithm::trim(sLine);
		std::vector<const char*> vsParts;
		throw std::runtime_error("Not implemented!");
		// algorithm::split(vsParts, sLine, algorithm::is_any_of("  "), algorithm::token_compress_on);
		// Would need regex_split because of "  " separator, but thats deprecated.
		sLine = vsParts.at(2);
		algorithm::trim(sLine);
	}
	
	int DependsCall::countIndents(const std::string& sLine)
	{
		int iSpaceCnt = 0;
		
		for each(const char& ch in sLine)
		{
			if( ch == ' ' )
				iSpaceCnt++;
			else
				break;
		}
				
		return iSpaceCnt/5;
	}

	DependsCall::~DependsCall()
	{

	}

	void DependsCall::execute( std::string &sModulePath )
	{
		AFXASSUME(PathFileExists(sModulePath.c_str()));

		m_sOutputFile = Config().getTempPath();
		MakeSureDirectoryPathExists(m_sOutputFile.c_str());

		m_sOutputFile += (m_sModule = filesystem::path(sModulePath).filename().string());
		m_sOutputFile += ".txt";

		initCmdLine(sModulePath);

		if( m_bSuccess = run() )
			parseResult();
	}

	bool DependsCall::isNumeric( const std::string s )
	{
		for each(const char& ch in s)
			if(ch > '9' || ch < '0')
				return false;
		
		return true;
	}
}