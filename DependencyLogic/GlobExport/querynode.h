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

#ifndef _DEPENDENCYCONTROL_QUERYNODE_H_
#define _DEPENDENCYCONTROL_QUERYNODE_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

#include "observable.h"
#include "module.h"
#include "export.h"
#include "queryselectionnode.h"
#include "queryview.h"
#include "cyclic.h"

namespace DependencyLogic
{
	class QueryNodeFactory;
	class Export;

	/**
	 * @about Represents a Node in the result graph of a Query, ultimately displaying
	 *        information about or parenting a specific class of related objects of the
	 *        Observable object it represents.
	 *        
	 *        The particular role of a QueryNode is described by the QuerySelectionNode it has
	 *        been created with.
	 *        
	 *        The QueryNode class is an abstract superclass. The instancable subclasses of
	 *        QueryNode inherit from QueryNode_T. The QueryNode_T class is a template, that
	 *        allows the QueryNode to only represent a specific subclass of Observable objects.
	 *        
	 *        A QueryNode can be created using the QueryNodeFactory, which is only available to
	 *        the QuerySelectionNode::select() functions.
	 * @see   QuerySelectionNode, QueryNode_T, Observable, QueryNodeFactory, Query
	 */
	class DEPENDENCYLOGIC_API QueryNode
	{
		public:
			/**
			 * @about  The QueryNode destructor. Calls delete on all subnodes.
			 * @see    addChild()
			 */
			virtual ~QueryNode();
			
			/**
			 * @about  The QueryNode constructor. A QueryNode has to be created as belonging to a specific QuerySelectionNode.
			 * @param  pNode The QuerySelectionNode that triggered the creation of this QueryNode.
			 * @see    QuerySelectionNode::select
			 */
			QueryNode(const QuerySelectionNode* pNode);
			
			/**
			 * @about  Creates the nodes subnodes. Calls the void QuerySelectionNode::select(QueryNode&, bool))
			 *         function to create child nodes of this QueryNode, issuing this for the QueryNode parameter, and
			 *         bApplyQuery for the boolean parameter.
			 *         
			 *         Subclasses may introduce different implementations
			 * @param  pNode The node that is supposed to be the parent of the new child QueryNodes.
			 *         Allowed to be Null if the new QueryNodes are to be appended to this QueryNode.
			 * @param  bApplyQuery forwarded to the QuerySelectionNode::select function call.
			 * @return void
			 * @see    QuerySelectionNode::select(QueryNode&, bool))
			 */
			virtual bool expand(QueryNode* pNode, bool bApplyQuery);
			
			/**
			 * @about  Appends this node as root or child element to the treeitems displayed in
			 *         a specific QueryView. The treeitems title will be set according to the getTitle() method.
			 *         The icon will be set according to the icon resource of this nodes QuerySelectionNode.
			 *         The m_hTreeItem member will get the newly created treeitem assigned to it.
			 * @param  treeView QueryView that will contain the newly created treeitem.
			 * @param  The QueryNode that holds the parent item, or Null, if the new item is supposed to
			 *         be a root item.
			 * @return void
			 * @see    QuerySelectionNode::getItemImage, getTitle, QueryView, Query::display
			 */
			void display(QueryView& treeView, QueryNode* pParent);
			
			/**
			 * @about  Recursively remove the QueryNode from a QueryView.
			 * @param  treeView The QueryView this QueryNode as previously been displayed in.
			 * @return void
			 * @see    display()
			 */
			void unload(QueryView& treeView);
			
			/**
			 * @about  Set the Observable object this node represents.
			 * @param  The object to be represented.
			 * @return void
			 * @see    QueryNode_T::setModel, getModel
			 */
			virtual void setModel(Observable*) = 0;
			
			/**
			 * @about  Get the Observable object this node represents.
			 * @return The object this node represents. Should not return Null.
			 * @see    QueryNode_T::getModel, setModel
			 */
			virtual Observable* getModel() const = 0;
			
			/**
			 * @about  Removes the Observable object from the QueryNode, thus making it unselectable and read-only.
			 * @return void
			 * @see    Query::invalidate()
			 */
			void invalidate();
			
			/**
			 * @about  Add a child to the list of children this QueryNode has.
			 *         Called by QuerySelectionNode::select(Observable*, QueryNode&, bool).
			 * @param  pChild The Child to be appended to m_lsChildren
			 * @return void
			 * @see    QuerySelectionNode::select(Observable*, QueryNode&, bool)
			 */
			void addChild(QueryNode* pChild);
			
			/**
			 * @about  Get the treeitem of this QueryNode, if display has been called.
			 * @return The treeitem, or null, if display() hasnt been called, or unload() has been called.
			 * @see    display(), unload()
			 */
			HTREEITEM getTreeItem() {return m_hTreeItem;}
			
			/**
			 * @about  Get this nodes title. Returns the QuerySelectionNodes title by default.
			 *         Subclasses might introduce a different implementation.
			 * @return The string that will become the treeitems title. Should be in human-readable form.
			 * @see    display()
			 */
			virtual std::string getTitle() const;
			
			void writeToFile( FILE* pFile, int iDepth );
			
		protected:
			/** Functor derived from executable_unary to extract Observable objects from an <std::string, Module*> map. */
			struct fctModuleMapElem : public executable_unary<const std::pair<std::string, Module*>&, Observable*> {
				Observable* operator() (const std::pair<std::string, Module*>& element) {return element.second;}
			} m_fctModuleMapElem;

			/** Functor derived from executable_unary to extract Observable objects from an <std::string, Export*> map. */
			struct fctExportMapElem : public executable_unary<const std::pair<std::string, Export*>&, Observable*> {
				Observable* operator() (const std::pair<std::string, Export*>& element) {return element.second;}
			} m_fctExportMapElem;
			
			/** Functor derived from executable_unary to extract Observable objects from an ExportRef list. */
			struct fctExportRefListElem : public executable_unary<const ExportRef&, Observable*> {
				Observable* operator() (const ExportRef& element) {return const_cast<ExportRef*>( (const Export*) element ? NULL : &element );}
			} m_fctIncompleteExportRefListElem;
			
			/** The QuerySelectionNode this QueryNode has been created with */
			mutable const QuerySelectionNode* m_pSelectionNode;
			
			/** Says whether this node is currently selected in the TreeView. */
			bool m_bSelected;
			
			/** The treeitem within a QueryView that shows this QueryNode. Set in display() */
			HTREEITEM m_hTreeItem;
			
			/** The child nodes of this QueryNode. */
			std::list<QueryNode*> m_lsChildren;
	};
	
	/**
	 * @about Exception thrown when trying to set an Observable object on a QueryNode_T
	 *        that is not of that QueryNode_T's template class.
	 * @see   QueryNode_T::setModel
	 */
	typedef std::pair<std::string, std::string> BadModelException;
	
	/**
	 * @about Represents a QueryNode subclass, that only accepts a special class of Observable objects.
	 * @param T_ModelType The template parameter describing the Observable subclass this QueryNode_T can be used for.
	 * @see   QueryNodeModule, QueryNodeExport, QueryNodeDependency, QueryNodeSystem, QueryNodeMissingExportError, QueryNodeCyclicChainError
	 */
	template<class T_ModelType> class QueryNode_T : public QueryNode
	{
		public:
			/**
			 * @about  Create a QueryNode_T, issuing an Observable object that must be of T_ModelType,
			 *         and the QuerySelectionNode that triggered the creation (will be forwarded to the QueryNode ctor.
			 *         
			 *         Calls setModel() with the Observable object issued.
			 * @param  pModel The Observable object of the new QueryNode
			 * @param  pNode  The QuerySelectionNode that triggered the creation of this node.
			 * @see    getModel(), setModel()
			 */
			QueryNode_T(Observable* pModel, const QuerySelectionNode* pNode) : QueryNode(pNode), m_pModel(NULL) {setModel(pModel);}
			
			/**
			 * @about  Delete a specific QueryNode. Will call invalidate()
			 * @see    invalidate()
			 */
			virtual ~QueryNode_T() {invalidate();}
			
		private:
			/** The T_ModelType instance this QueryNode_T represents. Must be a subtype of Observable. */
			T_ModelType* m_pModel;
			
			/** Returns the name of the T_ModelType class. */
			virtual std::string getClassName() = 0;
			
		public:
			/**
			 * @about  Covariant overload of QueryNode::getModel(). Returns the
			 *         observable this QueryNode represents. Must have been set using setModel().
			 * @return The T_ModelType pointer, or Null, if the QueryNode has been invalidated.
			 * @see    ivalidate(), setModel()
			 */
			virtual T_ModelType* getModel() const {return m_pModel;}
			
			/**
			 * @about  Set the Observable object this QueryNode represents.
			 *         A BadModelException will be thrown, if the issued Observable object
			 *         is not of T_ModelType.
			 *         Must not be used to invalidate the QueryNode.
			 * @param  pModel The Observable pointer m_pModel is to be set to.
			 * @return void
			 * @see    BadModelException
			 */
			virtual void setModel(__notnull Observable* pModel) {
				if( ( m_pModel = dynamic_cast<T_ModelType*>(pModel) ) == NULL )	
					throw BadModelException(pModel->getClassName(), getClassName());
			}
	};
	
	/////////////////////////////////////////////
	//
	#pragma push_macro("NODE_CLASS")
	#define NODE_CLASS(classname, superclass) \
		class DEPENDENCYLOGIC_API classname : public superclass \
		{ \
			friend class QueryNodeFactory; \
			\
			public: \
				classname(Observable* pModel, const QuerySelectionNode* pSelNode) : superclass(pModel, pSelNode) {;} \
				virtual std::string getClassName() {return #classname;} \
				virtual bool expand(QueryNode* pNode, bool bApplyQuery) // the SUBJECT_CLASS macro extends the definition
				 
	#pragma push_macro("SUBJECT_CLASS")
	#define SUBJECT_CLASS(classname, superclass) NODE_CLASS(classname, superclass) \
		{return QueryNode::expand(pNode, bApplyQuery);} \
		virtual std::string getTitle() const {return getModel()->getName();}
				
	#pragma push_macro("END_SUBJECT_CLASS")
	#define END_SUBJECT_CLASS };

	#pragma push_macro("END_CLASS")
	#define END_CLASS ;};
	//
	////////////////////////////////////////////
	
	NODE_CLASS(QueryNodeSystem, QueryNode_T<System>) END_CLASS
	
	SUBJECT_CLASS(QueryNodeModule, QueryNode_T<Module>) END_SUBJECT_CLASS
	SUBJECT_CLASS(QueryNodeExport, QueryNode_T<Export>) END_SUBJECT_CLASS
	SUBJECT_CLASS(QueryNodeDependency, QueryNode_T<Dependency>) END_SUBJECT_CLASS
	SUBJECT_CLASS(QueryNodeMissingExportError, QueryNode_T<ExportRef>) END_SUBJECT_CLASS
	SUBJECT_CLASS(QueryNodeCyclicChainError, QueryNode_T<CyclicReference>) END_SUBJECT_CLASS
	
	NODE_CLASS(QueryNodeModuleIncomingDependencies, QueryNode_T<Module>) END_CLASS
	NODE_CLASS(QueryNodeModuleOutgoingDependencies, QueryNode_T<Module>) END_CLASS
	NODE_CLASS(QueryNodeModuleExports, QueryNode_T<Module>) END_CLASS
	
	NODE_CLASS(QueryNodeCyclicChainErrorDependencies, QueryNode_T<CyclicReference>) END_CLASS
	NODE_CLASS(QueryNodeExportSourceModule, QueryNode_T<Export>) END_CLASS
	NODE_CLASS(QueryNodeExportDependencies, QueryNode_T<Export>) END_CLASS
	NODE_CLASS(QueryNodeDependencyTargetModule, QueryNode_T<Dependency>) END_CLASS
	NODE_CLASS(QueryNodeDependencySourceModule, QueryNode_T<Dependency>) END_CLASS
	NODE_CLASS(QueryNodeDependencyExports, QueryNode_T<Dependency>) END_CLASS
	NODE_CLASS(QueryNodeDependencyMissingExportErrors, QueryNode_T<Dependency>) END_CLASS
	
	#pragma pop_macro("NODE_CLASS")
	#pragma pop_macro("END_CLASS")
	#pragma pop_macro("SUBJECT_CLASS")
	#pragma pop_macro("END_SUBJECT_CLASS")
}

#endif