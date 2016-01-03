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

#ifndef _DEPENDENCYCONTROL_QUERYVIEW_H_
#define _DEPENDENCYCONTROL_QUERYVIEW_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

#include "../include/treeiconmanager.h"
#include "../include/ColumnTreeCtrl.h"

namespace DependencyLogic
{
	class Query;
	class QueryNode;

	/**
	 * @about CColumnTreeCtrl subclass specifically used to display the resulting QueryNode objects of a Query.
	 *        Use SubclassDlgItem or DDX_Control to instance.
	 * @see   Query, setQuery
	 */
	class DEPENDENCYLOGIC_API QueryView : public CColumnTreeCtrl
	{
		DECLARE_DYNAMIC(QueryView)
		
		public:
			QueryView();
			~QueryView();
			
			/**
			 * @about  Get this Query this QueryNode is displaying.
			 * @return The Query object, or Null, if this view is currently not displaying anything.
			 * @see    setQuery()
			 */
			const Query* getQuery() {return m_pQuery;}
			
			/**
			 * @about  Set the Query object this control will display by your call to showResults().
			 *         If a Query is already set on this QueryView, then Query::unload will be called
			 *         on that Query before m_pQuery is set to the new value.
			 * @param  pQuery The new query, or Null, if only the old one is to be unloaded.
			 * @return void
			 * @see    getQuery, Query::unload
			 */
			void setQuery(const Query* pQuery);
			
			/**
			 * @about  Wraps a call to m_pIcons->setItemBitmap.
			 * @return void
			 * @see    ManagedTreeIcons::setItemBitmap.
			 */
			inline void setItemBitmap(HTREEITEM hItem, INT nResID, bool bGrey);
		
			DECLARE_MESSAGE_MAP()
			
		protected:
			/**
			 * @about  PreSubclassWindow overload where this QueryViews treeicon manager will be created.
			 */
			virtual void PreSubclassWindow();
			
			/**
			 * @about  OnHeaderItemChanging where the UserColWidth will be written
			 */
			afx_msg void OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult);

		private:
			/** The Query currently set to be displayed in the QueryView. */
			const Query* m_pQuery;
			
			/** This QueryViews ManagedTreeIcons, created in OnCreate. */
			ManagedTreeIcons* m_pIcons;
			
			CBitmap m_bmpHupLogo;
	};
}

#endif
