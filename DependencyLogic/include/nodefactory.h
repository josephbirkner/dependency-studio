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

#ifndef _DEPENDENCYCONTROL_QUERYNODEFACTORY_H_
#define _DEPENDENCYCONTROL_QUERYNODEFACTORY_H_

#ifdef DEPENDENCYLOGIC_EXPORTS
#define DEPENDENCYLOGIC_API __declspec(dllexport)
#else
#define DEPENDENCYLOGIC_API __declspec(dllimport)
#endif

#include "StdAfx.h"

#include "../GlobExport/querynode.h"

namespace DependencyLogic
{
	/**
	 * @about Manages the creation of QueryNode object subclasses for specific QuerySelectionNode IDs.
	 * @see   QuerySelectionNode
	 */
	class QueryNodeFactory
	{
		/** The created node is stored in this member variable, and returned by operator QueryNode*() */
		QueryNode* m_pNode;
		
	public:
		/**
		 * @about  Class Constructor. Create a QueryNode for a specific Observable object and a QuerySelectionNode.
		 *         And assertion will be raised if a BadModelException is caught.
		 * @param  pModel The model which the newly created QueryNode should represent
		 * @param  pNode The QuerySelectionNode that triggered the creation and describes the subtype of the
		 *         QueryNode to be created via its id.
		 * @see    QuerySelectionNode, QueryNode, BadModelException
		 */
		QueryNodeFactory(Observable* pModel, const QuerySelectionNode* pNode) : m_pNode(NULL)
		{
			try
			{
				switch(pNode->getID())
				{
					#pragma push_macro("NODE_TYPE")
					#define NODE_TYPE(id, classname) case id: m_pNode = new classname(pModel, pNode); break;

					NODE_TYPE(IDNODE_EXPORTS, QueryNodeExport)
					NODE_TYPE(IDNODE_EXPORT_SOURCE_MODULE, QueryNodeExportSourceModule)
					NODE_TYPE(IDNODE_EXPORT_REFERENCING_DEPENDENCIES, QueryNodeExportDependencies)
					NODE_TYPE(IDNODE_MODULES, QueryNodeModule)
					NODE_TYPE(IDNODE_MODULE_EXPORTS, QueryNodeModuleExports)
					NODE_TYPE(IDNODE_MODULE_OUTGOING_DEPENDENCIES, QueryNodeModuleOutgoingDependencies)
					NODE_TYPE(IDNODE_MODULE_INCOMING_DEPENDENCIES, QueryNodeModuleIncomingDependencies)
					NODE_TYPE(IDNODE_DEPENDENCIES, QueryNodeDependency)
					NODE_TYPE(IDNODE_DEPENDENCY_EXPORTS, QueryNodeDependencyExports)
					NODE_TYPE(IDNODE_DEPENDENCY_MISSING_EXPORTS, QueryNodeDependencyMissingExportErrors)
					NODE_TYPE(IDNODE_DEPENDENCY_SOURCE_MODULE, QueryNodeDependencySourceModule)
					NODE_TYPE(IDNODE_DEPENDENCY_TARGET_MODULE, QueryNodeDependencyTargetModule)
					NODE_TYPE(IDNODE_CYCLIC_CHAIN_ERRORS, QueryNodeCyclicChainError)
					NODE_TYPE(IDNODE_CYCLIC_CHAIN_ERROR_DEPENDENCIES, QueryNodeCyclicChainErrorDependencies)
					NODE_TYPE(IDNODE_MISSING_EXPORT_ERROR, QueryNodeMissingExportError)
					NODE_TYPE(IDNODE_SYSTEM, QueryNodeSystem)
					
					#pragma pop_macro("NODE_TYPE")
				}
			}
			catch(BadModelException&)
			{
				DebugBreak();
			}
		}
		
		/**
		 * @about   Retreive the created QueryNode, or Null if the QuerySelectionNode did not provide a valid id,
		 *          or the Observable type did not match the required type (BadModelException).
		 * @return  The QueryNode, or Null, if an error occured.
		 */
		operator QueryNode*()
		{
			return m_pNode;
		}
	};
}

#endif