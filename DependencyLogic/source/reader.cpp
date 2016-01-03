/*     __                            __                               __          __
  ____/ /___  ____  ____  ____  ____/ /___  ____  _______  __  ______/ /_  ______/ /_____
 / __, / __ \/ __ \/ __ \/ ,_ \/ __, / __ \/ ,_ \/ ___/ / / / / ___\, / / / / __, /_/ __ \
/ /_/ / /_/ / /_/ / /_/ / / / / /_/ / /_/ / / / / /__/ /_/ / _\__, / / /_/ / /_/ / / /_/ /
\____/ ,___/ ,___/ ,___/\/ /_/\____/ ,___/\/ /_/\___/\__, /  \____/\/\___,_\____/\/\____/
     \____/\/    \____/            \____/               \/
               __  ____  ______     ____  ____________              __    __
              / /_/ / / / / __ \   /__, \/ __ \, /__, \          __/ /   /_/
             / ,_, / / / / /_/ /  ____/ / / / / /___/ /         /_/ /_  / /
            / / / / /_/ / ,___/  / ,___/ /_/ / / ,___/         / / ,_ \/ /
            \/  \/\____/\/       \____/\____/_/\____/ /\   ___/ / /_/ /_/
                                                      '/   \___/\____/
*/

#include "StdAfx.h"

#pragma hdrstop

#define SWITCH(_type, _caseclass) {_type __caseclass=_caseclass; if(0){
#define CASE(__case) } else if(__caseclass == __case) {
#define DEFAULT } else {
#define END_SWITCH }}

#include "../GlobExport/reader.h"
#include "../GlobExport/subject.h"
#include "../GlobExport/config.h"
#include "../GlobExport/module.h"
#include "../GlobExport/export.h"

namespace DependencyLogic
{
	Reader::Reader( std::string sXmlPath ) : m_pSystem(NULL), m_pModule(NULL), m_pArchive(NULL)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		
		m_dlgProgress.ShowModal(NULL);
		m_dlgProgress.SetTitle(CFMsg(IDS_LOADDB_PROGRESSDLGTITLE, sXmlPath.c_str()));
		
		m_dlgProgress.SetLineText( 1, CFMsg(IDS_LOADDB_PARSINGDOCUMENT), false );
		m_dlgProgress.SetCancelMessage(CFMsg(IDS_LOADDB_NOCANCEL));
		
		m_pArchive = new XMLArchive();
		m_pArchive->setDelegate(this);
		
		if( m_pArchive->loadFromFile(sXmlPath) )
		{
			m_pSystem = m_pArchive->getObject<System>("system", &m_nResult);
			AFXASSUME(m_nResult == Archiving::Found);
			
			Config().addRecentDatabase(sXmlPath);
		}
		else
			MessageBox(NULL, CFMsg(IDS_MISSING_FILE_OR_PARSER_ERROR, sXmlPath.c_str()), CFMsg(IDS_BADDB), MB_ICONWARNING|MB_OK);
			
		m_dlgProgress.EndDialog();
		delete m_pArchive;
	}

	IArchivableObject* Reader::handleInstance( const IArchivableObject* pObject )
	{
		SWITCH(std::string, pObject->getClassName())
		
			CASE("System")
			{
				m_pSystem = (System*) pObject;
				
				/* this is a little bit dirty. the system communicates the total
				 * module/dependency count to the reader (setDependenyCount/setModuleCount),
				 * so the reader can track the load progress.
				 */
				m_pSystem->setReader(this);
			}
			CASE("Module")
			{
				( (Subject*) pObject )->setSystem(m_pSystem);
				m_pModule = (Module*) pObject;
				
				m_dlgProgress.SetLineText( 1, CFMsg(IDS_LOADDB_LOADINGMODULES, ++m_nModules.second, m_nModules.first), false );
				m_dlgProgress.UpdateProgress( (DWORD) m_nModules.second, (DWORD) m_nModules.first);
			}
			CASE("Export")
			{
				( (Subject*) pObject )->setSystem(m_pSystem);
				( (Export*) pObject )->setModule(m_pModule);
			}
			CASE("Dependency")
			{
				( (Subject*) pObject )->setSystem(m_pSystem);
				m_dlgProgress.SetLineText( 2, CFMsg(IDS_LOADDB_LOADINGDEPENDENCIES, ++m_nDependencies.second, m_nDependencies.first), false );
				m_dlgProgress.UpdateProgress( (DWORD) m_nDependencies.second, (DWORD) m_nDependencies.first);
			}
				
		END_SWITCH
			
		return IArchiveDelegate::handleInstance(pObject);
	}

	bool Reader::afterDeserializeObject( IArchivableObject *pObject )
	{
		if(pObject->getClassName() == "Dependency")
			return ( (Dependency*) pObject )->getSourceModule() != NULL;
			
		return true;
	}

}
