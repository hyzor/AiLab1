#ifndef ASTAR_H_
#define ASTAR_H_

#include <vector>
#include <assert.h>
#include <algorithm>

template <class T> class AStarState;

template <class UserState> class AStar
{
public:
	enum SearchStates
	{
		NOT_INITIALIZED = 0,
		SEARCHING = 1,
		SUCCEEDED = 2,
		FAILED = 3,
		OUT_OF_MEMORY = 4,
		INVALID = 5
	};

public:
	class Node
	{
	public:
		Node();

		Node* mParent;
		Node* mChild;

		float g; // Movement cost (Cost of this node and its predecessors)
		float h; // Heuristic estimate of distance to goal
		float f; // g + h

		UserState mUserState;
	};

public:
	class HeapCompare_f
	{
	public:
		bool operator() (const Node* x, const Node* y) const
		{
			return x->f > y->f;
		}
	};

public:
	AStar();
	~AStar();

	//void FindPath(const int startX, const int startY, const int goalX, const int goalY);

	void SetStartAndGoalStates(UserState& start, UserState& end);

	unsigned int SearchStep();

	void ClearUnusedNodes();
	void ClearAllNodes();

	bool AddSuccessor(UserState& state);

	UserState* GetSolutionStart();
	UserState* GetSolutionNext();

	//static const int mGridWidth = 42;
	//static const int mGridHeight = 42;

	//int mClosedNodes[GRID_WIDTH]; // Closed nodes (already tried out)
	//int mOpenNodes[GRID_HEIGHT]; // Open nodes (to be tried out)

private:
	std::vector<Node*> mClosedNodeList; // List of closed nodes (already tried out)
	std::vector<Node*> mOpenNodeList; // List of open nodes (to be tried out)

	std::vector<Node*> mCurNodeSuccessors; // List of node successors to the current node

	Node* mStartNode; // Start node
	Node* mGoalNode; // Goal node

	Node* mCurrentSolutionNode;

	// Current state
	SearchStates mCurrentState;
};

template <class T> class AStarState
{
public:
	virtual ~AStarState() {}
	virtual float GoalDistanceEstimate(T& nodeGoal) = 0; // Heuristic computational function estimating the cost to goal node
	virtual bool IsGoalNode(T& nodeGoal) = 0;
	virtual bool GetSuccessors(AStar<T>* aStarSearch, T* parentNode) = 0; // Get all successors to this node
	virtual float GetCost(T& successor) = 0; // Cost from this node to successor node
	virtual bool IsSameState(T& rhs) = 0; // Compare two nodes if they are the same state
};

#endif