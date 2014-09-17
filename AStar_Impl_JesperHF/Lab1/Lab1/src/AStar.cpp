#include "AStar.h"

AStar<class UserState>::Node::Node()
{
	mParent = 0;
	mChild = 0;
	g = h = f = 0.0f;
}

AStar<class UserState>::AStar()
{
	mClosedNodeList.clear();
	mOpenNodeList.clear();

	mStartNode = 0;
	mGoalNode = 0;
}

AStar<class UserState>::~AStar()
{
	for (int i = 0; i < mClosedNodeList.size(); ++i)
	{
		if (mClosedNodeList[i])
			delete mClosedNodeList[i];
	}
	mClosedNodeList.clear();

	for (int i = 0; i < mOpenNodeList.size(); ++i)
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

void AStar<class UserState>::SetStartAndGoalStates(UserState& start, UserState& goal)
{
	mStartNode = new Node();
}