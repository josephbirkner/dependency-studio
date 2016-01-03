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
#include <vector>

#pragma hdrstop

#include "../include/dumpbincall.h"

namespace DependencyLogic
{
	DumpbinCall::DumpbinCall( std::string sModulePath ) : DependsCall(), m_nType(TypeDll)
	{
		execute(sModulePath);
	}

	void DumpbinCall::initCmdLine( std::string sModulePath )
	{
		if( iequals(filesystem::path(sModulePath).extension().string(), ".exe") )
			m_nType = TypeExe;
			
		m_sCmdLine = "dumpbin "+sModulePath+" /exports /imports /out:"+m_sOutputFile;
	}

	void DumpbinCall::parseResult()
	{
		FILE* pResult = fopen(m_sOutputFile.c_str(), "r");
		
		AFXASSUME(pResult);
		if(!pResult)
			return;
		
		std::string sLine;
		int nEmptyLineCount = 0;
		std::string sCurrentModule;
		
		enum ParseState {ParseExports, ParseImports, ParseDelayLoadImports} nParseState = ParseExports;
		
		while(!feof(pResult))
		{
			parseLine(pResult, sLine);
			algorithm::trim(sLine);
			
			if(sLine.empty())
			{
				// switch ParseState if necessary
				switch(nEmptyLineCount)
				{
					case 3:
						if(m_nType != TypeExe)
							break;
					case 7:
						nParseState = ParseImports;
					case 2:
						if(nParseState == ParseImports || nParseState == ParseDelayLoadImports)
						{
							sCurrentModule = "";
							nEmptyLineCount = 0;
						}
				}
				nEmptyLineCount++;
			}
			else
			{
				switch(nParseState)
				{
					case ParseExports:
						if(nEmptyLineCount == 6)
						{ 
							std::vector<std::string> sParts;
							algorithm::split(sParts, sLine, algorithm::is_any_of(" "), algorithm::token_compress_on);
							if(sParts.size() > 5 && sParts[4] != "=") // This case applies for forwarded exports
								m_lsExports.push_back(sParts[2]);
							else if(sParts.size() > 3)
								m_lsExports.push_back(sParts[3]);
						}
						break;
					case ParseImports:
					case ParseDelayLoadImports:
						if(nEmptyLineCount == 1 && sCurrentModule.empty()) // parse module name
						{
							if( iequals(filesystem::path(sLine).extension().string(), ".dll") )
								sCurrentModule = filesystem::path(sLine).filename().string();
							else
							{
								// In that case, we just hit the line "Section contains the following delay load imports:"
								nEmptyLineCount = 0;
								nParseState = ParseDelayLoadImports;
							}
						}
						else if(nEmptyLineCount > 1 && !sCurrentModule.empty())
						{
							std::vector<std::string> sParts;
							algorithm::split(sParts, sLine, algorithm::is_any_of(" "), algorithm::token_compress_on);
							if(sParts.size() > 1 && !isNumeric(sParts[1]))
								m_mapImportedModuleFunctions[sCurrentModule].push_back(sParts[1]);
							else if(nParseState == ParseDelayLoadImports && sParts.size() > 2)
								m_mapImportedModuleFunctions[sCurrentModule].push_back(sParts[2]);
						}
				} // switch
			} // if(sLine.empty()) else
		} // while
		
		fclose(pResult);
	}
	
}
