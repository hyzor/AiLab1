#ifndef GRIDNODE_H_
#define GRIDNODE_H_

#include "AStar.cpp"

class GridNode
{
public:
	GridNode();
	GridNode(int _x, int _y, std::wstring _type);
	~GridNode();

	float GoalDistanceEstimate(GridNode& goalNode);
	bool IsGoalNode(GridNode& goalNode);
	bool GetSuccessors(AStar<GridNode>* aStar, GridNode* parentNode);
	float GetCost(GridNode& successors);
	//float GetCost(GridNode& successors, std::vector<std::vector<int>>& edges);
	bool IsSameState(GridNode& rhs);

	static void SetEdges(std::vector<std::vector<int>>* edges);
	static void SetGridDimensions(unsigned int gridWidth, unsigned int gridHeight);

	static bool IsValidGridPos(int x, int y);

	int x;
	int y;
	std::wstring type;

	static std::vector<std::vector<int>>* mEdges;
	static int mGridWidth, mGridHeight;
};

#endif