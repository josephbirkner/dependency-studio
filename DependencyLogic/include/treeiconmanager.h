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

#ifndef _DEPENDENCYCONTROL_TREEICONMGR_H_
#define _DEPENDENCYCONTROL_TREEICONMGR_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

// PCH/STL/Windows/RogueWave
#include "StdAfx.h"

using namespace Archiving;

namespace DependencyLogic
{
	class Module;

	/**
	 * @about Manages an image list for a TreeView, enabling to
	 *        set the icon of a treeitem by a resource id.
	 *        
	 *        When instancing it, it must be assigned to a variable.
	 *        That variable has to be put into the ctor as a reference,
	 *        and will automatically be set to Null in the dtor.
	 *        
	 *        The ManagedTreeIcons class implements a refcounting mechanism,
	 *        deleting itself when the refcount reaches zero. Using
	 *        that mechanism is optional.
	 * @see   QueryView, QuerySelectionNode
	 */
	class ManagedTreeIcons
	{
	public:
		/**
		 * @about  Constructs a new ManagedTreeIcons instance for a specific CTreeCtrl
		 *         and assigns it to a specific variable.
		 * @param  pCtrl The MFC Tree Control this ManagedTreeIcons instance should manage
		 *         the image list for.
		 */
		ManagedTreeIcons(CTreeCtrl* pCtrl, ManagedTreeIcons*& refVar);
		~ManagedTreeIcons();
		
		/**
		 * @about  Set the bitmap for a specific HTREEITEM of the tree control
		 *         managed by this ManagedTreeIcons instance.
		 *         
		 *         It is important that you call AFX_MANAGE_STATE in your program
		 *         before making a call to this function, so that the
		 *         resource is properly recognized.
		 * @param  hItem The tree item that is supposed to retreive a new icon.
		 * @param  nResID Resource identifier of the bitmap that should become the new icon.
		 * @param  bGrey Set to true to set a greyscale version of the bitmap on the treeitem.
		 * @return void
		 * @see    QuerySelectionNode::getIconImage()
		 */
		void setItemBitmap(HTREEITEM hItem, INT nResID, bool bGrey);
		
		/**
		 * @about  Add a referencing pointer to this ManagedTreeIcons reference list
		 * @param  p The pointer to be added. A pointer can not be added twice.
		 * @return void
		 * @see    removeRef()
		 */
		void addRef(void* p);
		
		/**
		 * @about  Remove a referencing pointer from the ManagedTreeIcons instance.
		 *         If the refcount reaches zero, the TreeCtrl will delet itself and set the
		 *         reference var to Null.
		 * @param  p The pointer to be removed. It must have been added before to have an impact on the refcount.
		 *         After it has been removed, it can not be removed again, except if you add it again before.
		 * @return void
		 * @see    addRef()
		 */
		void removeRef(void* p);

	protected:
		/** The tree control this ManagedTreeIcons instance is managing the image list for. */
		CTreeCtrl* m_pTreeCtrl;
		
		/** The managed image list */
		CImageList m_ilBitmaps;
		
		/** Map that points from a resource id to a bool telling whether that resource id
		 * has already been loaded, paired with the index of the resource id in the image list.
		 */
		std::map<INT, std::pair<bool, int>> m_mapResIDImageListIndex;
		
		/** Map that points from a pointer to a flag that tells whether that pointer is still referencing the manager. */
		std::map<void*, bool> m_mapPointerRefStatus;
		
		/** The refcount. */
		long m_lRefs;
		
		/** The variable that holds this managed tree icons instance */
		ManagedTreeIcons*& m_refVar;
	};
}

#endif