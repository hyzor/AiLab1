#include "GridNode.h"

std::vector<std::vector<int>>* GridNode::mEdges = nullptr;
int GridNode::mGridWidth = 0;
int GridNode::mGridHeight = 0;

GridNode::GridNode()
{
	x = y = 0;
	type = L"Undefined";
}

GridNode::GridNode(int _x, int _y, std::wstring _type)
{
	x = _x;
	y = _y;
	type = _type;
}

GridNode::~GridNode(){}

float GridNode::GoalDistanceEstimate(GridNode& goalNode)
{
	// Heuristic estimate to goal
	float xDist = float((float)x - (float)goalNode.x);
	float yDist = float((float)y - (float)goalNode.y);

	return std::abs(xDist) + std::abs(yDist);

	//return xDist + yDist;
}

bool GridNode::IsGoalNode(GridNode& goalNode)
{
	if ((x == goalNode.x) && (y == goalNode.y))
	{
		return true;
	}

	return false;
}

bool GridNode::GetSuccessors(AStar<GridNode>* aStar, GridNode* parentNode)
{
	int parentX = -1;
	int parentY = -1;

	if (parentNode)
	{
		parentX = parentNode->x;
		parentY = parentNode->y;
	}

	GridNode newNode;

	// Find and add each possible move (successor)

	// Left node
	if ((parentX != x - 1) && (parentY != y))
	{
		if (IsValidGridPos(x - 1, y))
		{
			newNode = GridNode(x - 1, y, L"Undefined");
			aStar->AddSuccessor(newNode);
		}
	}

	// Top node
	if ((parentX != x) && (parentY != y - 1))
	{
		if (IsValidGridPos(x, y - 1))
		{
			newNode = GridNode(x, y - 1, L"Undefined");
			aStar->AddSuccessor(newNode);
		}
	}

	// Right node
	if ((parentX != x + 1) && (parentY != y))
	{
		if (IsValidGridPos(x + 1, y))
		{
			newNode = GridNode(x + 1, y, L"Undefined");
			aStar->AddSuccessor(newNode);
		}
	}

	// Bottom node
	if ((parentX != x) && (parentY != y + 1))
	{
		if (IsValidGridPos(x, y + 1))
		{
			newNode = GridNode(x, y + 1, L"Undefined");
			aStar->AddSuccessor(newNode);
		}
	}

	return true;
}

// float GridNode::GetCost(GridNode& successors, std::vector<std::vector<int>>& edges)
// {
// 	// Get the cost of moving to successor
// 	// In this case, the cost is the edge value between the two nodes
// 	int edgeX, edgeY;
// 	edgeX = int(((float)x + (float)successors.x) * 0.5f);
// 	edgeY = y + successors.y;
// 
// 	return (float)edges[edgeY][edgeX];
// }

float GridNode::GetCost(GridNode& successors)
{
	unsigned int edgeX, edgeY;
// 	edgeX = int(((float)x + (float)successors.x) * 0.5f);
// 	edgeY = y + successors.y;

	edgeX = unsigned int(((float)y + (float)successors.y) * 0.5f);
	edgeY = x + successors.x;

	if (edgeY >= mEdges->size())
		edgeY = mEdges->size() - 1;

	if (edgeX >= mEdges->at(edgeY).size())
		edgeX = mEdges->at(edgeY).size() - 1;

	float cost = (float)mEdges->at(edgeY).at(edgeX);

	//return (float)mEdges[edgeY][edgeX];
	return cost;
}

bool GridNode::IsSameState(GridNode& rhs)
{
	if ((x == rhs.x) && (y == rhs.y))
		return true;

	return false;
}

void GridNode::SetEdges(std::vector<std::vector<int>>* edges)
{
	mEdges = edges;
}

void GridNode::SetGridDimensions(unsigned int gridWidth, unsigned int gridHeight)
{
	mGridWidth = gridWidth;
	mGridHeight = gridHeight;
}

bool GridNode::IsValidGridPos(int x, int y)
{
	if (x < 0 || x > mGridWidth - 1)
		return false;

	if (y < 0 || y > mGridHeight - 1)
		return false;

	return true;
}
