#define GRID_WIDTH 42
#define GRID_HEIGHT 42

#ifndef ASTAR_H_
#define ASTAR_H_

#include <vector>

template <class UserState> class AStar
{
public:
	class Node
	{
	public:
		Node();

	private:
		Node* mParent;
		Node* mChild;

		float g; // Movement cost (Cost of this node and its predecessors)
		float h; // Heuristic estimate of distance to goal
		float f; // g + h

		UserState mUserState;
	};

public:
	AStar();
	~AStar();

	//void FindPath(const int startX, const int startY, const int goalX, const int goalY);

	void SetStartAndGoalStates(UserState& start, UserState& end);

	//static const int mGridWidth = 42;
	//static const int mGridHeight = 42;

	//int mClosedNodes[GRID_WIDTH]; // Closed nodes (already tried out)
	//int mOpenNodes[GRID_HEIGHT]; // Open nodes (to be tried out)

	std::vector<Node*> mClosedNodeList; // List of closed nodes (already tried out)
	std::vector<Node*> mOpenNodeList; // List of open nodes (to be tried out)

	std::vector<Node*> mCurNodeSuccessors; // List of node successors to the current node

	Node* mStartNode; // Start node
	Node* mGoalNode; // Goal node

private:

};

#endif