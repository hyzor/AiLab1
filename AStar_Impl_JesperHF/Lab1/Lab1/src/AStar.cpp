#ifndef ASTAR_CPP_
#define ASTAR_CPP_

#include "AStar.h"

template <class UserState>
AStar<UserState>::Node::Node()
{
	mParent = nullptr;
	mChild = nullptr;
	g = h = f = 0.0f;
}

template <class UserState>
AStar<UserState>::AStar()
{
	mClosedNodeList.clear();
	mOpenNodeList.clear();

	mStartNode = nullptr;
	mGoalNode = nullptr;
	mCurrentSolutionNode = nullptr;

	mCurrentState = NOT_INITIALIZED;
}

template <class UserState>
AStar<UserState>::~AStar()
{
	for (unsigned int i = 0; i < mClosedNodeList.size(); ++i)
	{
		if (mClosedNodeList[i])
			delete mClosedNodeList[i];
	}
	mClosedNodeList.clear();

	for (unsigned int i = 0; i < mOpenNodeList.size(); ++i)
	{
		if (mOpenNodeList[i])
			delete mOpenNodeList[i];
	}
	mOpenNodeList.clear();

	if (mStartNode)
		delete mStartNode;

	if (mGoalNode)
		delete mGoalNode;
}

template <class UserState>
void AStar<UserState>::SetStartAndGoalStates(UserState& start, UserState& goal)
{
	mStartNode = new Node();
	mGoalNode = new Node();

	assert((mStartNode != NULL && mGoalNode != NULL));

	mStartNode->mUserState = start;
	mGoalNode->mUserState = goal;

	mCurrentState = SEARCHING;

	// Initialize start node properties
	mStartNode->g = 0;
	mStartNode->h = mStartNode->mUserState.GoalDistanceEstimate(mGoalNode->mUserState);
	mStartNode->f = mStartNode->g + mStartNode->h;
	mStartNode->mParent = 0;

	// Push start node to open list
	mOpenNodeList.push_back(mStartNode);

	push_heap(mOpenNodeList.begin(), mOpenNodeList.end(), HeapCompare_f());
}

template <class UserState>
unsigned int AStar<UserState>::SearchStep()
{
	// Make sure we have initialized A* and its not invalid
	assert ((mCurrentState > NOT_INITIALIZED) && (mCurrentState < INVALID));

	// If the A* algorithm has succeeded or failed, just return this status
	if ((mCurrentState == SUCCEEDED) || (mCurrentState == FAILED))
	{
		return mCurrentState;
	}

	// If the open node list is empty, A* has failed
	if (mOpenNodeList.empty())
	{
		ClearAllNodes();
		mCurrentState = FAILED;
		return mCurrentState;
	}

	// Pop the node with the lowest f
	Node* node = mOpenNodeList.front();
	pop_heap(mOpenNodeList.begin(), mOpenNodeList.end(), HeapCompare_f());
	mOpenNodeList.pop_back();

	// Check if this node is actually the goal node
	if (node->mUserState.IsGoalNode(mGoalNode->mUserState))
	{
		mGoalNode->mParent = node->mParent;
		mGoalNode->g = node->g;

		// If this node is also the start node...
		if (!node->mUserState.IsSameState(mStartNode->mUserState))
		{
			delete node; // FreeNode(node)

			Node* childNode = mGoalNode;
			Node* parentNode = mGoalNode->mParent;

			while (childNode != mStartNode)
			{
				parentNode->mChild = childNode;
				childNode = parentNode;
				parentNode = parentNode->mParent;
			}
		}

		ClearUnusedNodes();

		mCurrentState = SUCCEEDED;

		return mCurrentState; // Success
	}

	// The node is not a goal node
	else
	{
		mCurNodeSuccessors.clear();

		bool hasSuccessors = node->mUserState.GetSuccessors(this, node->mParent ? &node->mParent->mUserState : NULL);

		// Error, successors wasn't found
		if (!hasSuccessors)
		{
			typename std::vector<Node*>::iterator successor;

			for (successor = mCurNodeSuccessors.begin(); successor != mCurNodeSuccessors.end(); ++successor)
			{
				delete (*successor); // FreeNode(*successor)
			}

			mCurNodeSuccessors.clear();

			ClearAllNodes();

			mCurrentState = OUT_OF_MEMORY;

			return mCurrentState; // Out of memory
		}

		// Continue with handling each successor to this current node
		for (typename std::vector<Node*>::iterator successor = mCurNodeSuccessors.begin(); successor != mCurNodeSuccessors.end(); ++successor)
		{
			// Find the g value of this successor
			float g_new = node->g + node->mUserState.GetCost((*successor)->mUserState);

			// Now we find out if the node is on the open or closed list

			// Find if it is in the open list
			typename std::vector<Node*>::iterator openListResult;
			for (openListResult = mOpenNodeList.begin(); openListResult != mOpenNodeList.end(); ++openListResult)
			{
				if ((*openListResult)->mUserState.IsSameState((*successor)->mUserState))
				{
					// Node was found in the open node list, break
					break;
				}
			}

			// It was found in the open node list
			if (openListResult != mOpenNodeList.end())
			{
				// If the g value is cheaper than the new g value, skip the subsequent code
				if ((*openListResult)->g <= g_new)
				{
					delete (*successor); // FreeNode(*successor)
					continue;
				}
			}

			// Find if it is in the closed list
			typename std::vector<Node*>::iterator closedListResult;
			for (closedListResult = mClosedNodeList.begin(); closedListResult != mClosedNodeList.end(); ++closedListResult)
			{
				if ((*closedListResult)->mUserState.IsSameState((*successor)->mUserState))
				{
					// Node was found in the closed node list, break
					break;
				}
			}

			// It was found in the closed node list
			if (closedListResult != mClosedNodeList.end())
			{
				if ((*closedListResult)->g <= g_new)
				{
					delete (*successor); // FreeNode((*successor))
					continue;
				}
			}

			// We now have found the best node with this particular state so far
			(*successor)->mParent = node;
			(*successor)->g = g_new;
			(*successor)->h = (*successor)->mUserState.GoalDistanceEstimate(mGoalNode->mUserState);
			(*successor)->f = (*successor)->g + (*successor)->h;

			// Remove successors from closed list (if the node was from this list)
			if (closedListResult != mClosedNodeList.end())
			{
				delete (*closedListResult); // FreeNode((*closedListResult))
				mClosedNodeList.erase(closedListResult);
			}

			// Remove successors from open list (if the node was from this list)
			// And rebuild the open node list heap
			if (openListResult != mOpenNodeList.end())
			{
				delete (*openListResult); // FreeNode((*openListResult))
				mOpenNodeList.erase(openListResult);

				make_heap(mOpenNodeList.begin(), mOpenNodeList.end(), HeapCompare_f());
			}

			// Push this successor node to the open node list
			mOpenNodeList.push_back((*successor));

			push_heap(mOpenNodeList.begin(), mOpenNodeList.end(), HeapCompare_f());
		}

		// Push node to closed list (we have already expanded it)
		mClosedNodeList.push_back(node);
	}

	return mCurrentState;
}


template <class UserState>
void AStar<UserState>::ClearAllNodes()
{
	typename std::vector<Node*>::iterator openIter = mOpenNodeList.begin();

	while (openIter != mOpenNodeList.end())
	{
		Node* node = (*openIter);
		delete node; // FreeNode(node)
		++openIter;
	}

	mOpenNodeList.clear();

	typename std::vector<Node*>::iterator closedIter;

	for (closedIter = mClosedNodeList.begin(); closedIter != mClosedNodeList.end(); ++closedIter)
	{
		Node* node = (*closedIter);
		delete node; // FreeNode(node)
	}

	mClosedNodeList.clear();

	// Delete goal node
	delete mGoalNode; // FreeNode(mGoalNode)
}


template <class UserState>
void AStar<UserState>::ClearUnusedNodes()
{
	// Iterate open list and clear unused nodes
	typename std::vector<Node*>::iterator openIter = mOpenNodeList.begin();

	while (openIter != mOpenNodeList.end())
	{
		Node* node = (*openIter);

		if (!node->mChild)
		{
			delete node;
			node = NULL;
		}

		openIter++;
	}

	mOpenNodeList.clear();

	// Iterate closed list and clear unused nodes
	typename std::vector<Node*>::iterator closedIter;

	for (closedIter = mClosedNodeList.begin(); closedIter != mClosedNodeList.end(); ++closedIter)
	{
		Node* node = (*closedIter);

		if (!node->mChild)
		{
			delete node;
			node = NULL;
		}
	}

	mClosedNodeList.clear();
}


template <class UserState>
bool AStar<UserState>::AddSuccessor(UserState& state)
{
	Node* node = new Node(); // AllocateNode()

	if (node)
	{
		node->mUserState = state;
		mCurNodeSuccessors.push_back(node);
		return true;
	}

	return false;
}


template <class UserState>
UserState* AStar<UserState>::GetSolutionStart()
{
	mCurrentSolutionNode = mStartNode;

	if (mStartNode)
	{
		return &mStartNode->mUserState;
	}
	else
	{
		return nullptr;
	}
}


template <class UserState>
UserState* AStar<UserState>::GetSolutionNext()
{
	if (mCurrentSolutionNode)
	{
		if (mCurrentSolutionNode->mChild)
		{
			Node* childNode = mCurrentSolutionNode->mChild;
			mCurrentSolutionNode = mCurrentSolutionNode->mChild;

			return &childNode->mUserState;
		}
	}

	return nullptr;
}


#endif