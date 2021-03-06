#define GRID_WIDTH 42
#define GRID_HEIGHT 42
#define NUM_VANS 5
#define TIME_SPREADOUT 60

#include <iostream>
#include <map>

#include "DeliveryManClient.h"
#include "GridNode.h"
#include "AStar.cpp"

int EdgeToNodesCalc(Location nodeA, Location nodeB, std::vector<std::vector<int>>* edges)
{
	// 	int x, y = 0;
	// 	x = int(((float)nodeA.second + (float)nodeB.second) * 0.5f);
	// 	y = nodeA.first + nodeB.first;
	// 
	// 	return edges[y][x];

	unsigned int edgeX, edgeY;

	edgeX = int(((float)nodeA.second + (float)nodeB.second) * 0.5f);
	edgeY = nodeA.first + nodeB.first;

	if (edgeY >= edges->size())
		edgeY = edges->size() - 1;

	if (edgeX >= edges->at(edgeY).size())
		edgeX = edges->at(edgeY).size() - 1;

	//return (float)mEdges[edgeY][edgeX];
	return edges->at(edgeY).at(edgeX);
}

Location EdgePosFromNodes(GridNode* nodeA, GridNode* nodeB)
{
	Location pos;

	pos.second = int(((float)nodeA->y + (float)nodeB->y) * 0.5f);
	pos.first = nodeA->x + nodeB->x;

	return pos;
}

std::vector<std::pair<int, int>> InitAndRunAStar(AStar<GridNode>* aStar, Location locA, Location locB)
{
	// Clear nodes previously used by this Astar object
	//aStar->ClearAllNodes();

	// Instructions to be sent to the van
	std::vector<std::pair<int, int>> instructions;

	// Initialize A* by setting start and goal states
	GridNode startNode = GridNode(locA.first, locA.second, L"Undefined");
	GridNode goalNode = GridNode(locB.first, locB.second, L"Undefined");
	aStar->SetStartAndGoalStates(startNode, goalNode);

	// Begin A* by doing the first step
	unsigned int aStarState;
	aStarState = aStar->SearchStep();

	// Run A* until we have found a route to the goal (or encouter some error)
	while (aStarState == AStar<GridNode>::SEARCHING)
	{
		aStarState = aStar->SearchStep();
	}

	// A* succeeded with the search
	if (aStarState == AStar<GridNode>::SUCCEEDED)
	{
		// Get the start node to the solution
		GridNode* node = aStar->GetSolutionStart();
		GridNode* prevNode = node;
		//std::cout << node->x << " " << node->y << std::endl;

		while (node)
		{
			node = aStar->GetSolutionNext();

			if (!node)
			{
				break;
			}

			// Get the edge between this node and the previous one
			instructions.push_back(EdgePosFromNodes(node, prevNode));

			prevNode = node;

			//std::cout << node->x << " " << node->y << std::endl;
		}

		// Assign the instructions to the van

		aStar->ClearSolutionNodes();
	}

	// A* failed with the search
	else if (aStarState == AStar<GridNode>::FAILED)
	{

	}

	return instructions;
}

void RouteVanAndSendInstructions(DM_Client* dm, VanInfo* van, AStar<GridNode>* aStar, Location locA, Location locB)
{
	// Start by clearing target van instructions
	van->instructions.clear();

	std::vector<std::pair<int, int>> curVanInstructions;
	curVanInstructions = InitAndRunAStar(aStar, locA, locB);
	//vanInstructions[targetVanIndex] = curVanInstructions;
	//targetVan->instructions.push_back(Location()); // Push back dummy instruction to avoid further instructing this van

	std::map<int, std::vector<std::pair<int, int>>> curVanInstructionsMap;
	curVanInstructionsMap[van->Number] = curVanInstructions;

	std::wstring output;

	dm->sendInstructions(curVanInstructionsMap, output);
	std::cout << "Instructions for van " << van->Number << " sent!" << std::endl;
	std::wcout << output << std::endl;
}

int main()
{
	const std::wstring group(L"Skynet");
	bool ok;
	DM_Client dm(group, ok);

	std::vector<std::vector<std::wstring>> nodes;
	std::wstring output;
	dm.startGame(nodes, output);
	std::wcout << output << std::endl;

	std::vector<GridNode*> gridNodes;

	std::vector<std::vector<GridNode*>> gridNodes_2D;

	// Populate our node vector
	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		gridNodes_2D.push_back(std::vector<GridNode*>());

		for (unsigned int j = 0; j < nodes[i].size(); ++j)
		{
			GridNode* gridNode = new GridNode(i, j, nodes[i][j]);
			gridNodes.push_back(gridNode);

			gridNodes_2D[i].push_back(gridNode);
		}
	}

	int time;												//	Current time (in minutes). The game starts at 6:00=360 and ends at 24:00 = 1440
	std::vector<std::vector<int>>* Edges = new std::vector<std::vector<int>>();					//	The current traffic conditions on all edges: The number of minutes a van will take to travel over the edge.
	std::vector<VanInfo> Vans;								//	The van information for each van (see above)
	std::vector<DeliveryInfo> waitingDeliveries;			//	The delivery information for each waiting (not yet picked up) delivery (see above).
	std::vector<DeliveryInfo> activeDeliveries;				//	The delivery information for each active (currently on a van) delivery (see above).
	std::vector<std::pair<int, int>> completedDeliveries;	//	The delivery number and time taken for each completed delivery.

	std::map<int, int> vanDeliveryMap; // Mapping between a delivery number and van ID
	std::map<int, int> curVanDelivery; // Current delivery assigned to a van

	unsigned int prevCompletedDeliveriesNum = 0;

	dm.getInformation(time, *Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);

	// There currently are no assigned deliveries to any van
	for (unsigned int i = 0; i < Vans.size(); ++i)
	{
		curVanDelivery[Vans[i].Number] = -1;
	}

	std::vector<AStar<GridNode>*> aStar;
	for (int i = 0; i < NUM_VANS; ++i)
	{
		aStar.push_back(new AStar<GridNode>());
	}

	GridNode::SetEdges(Edges);
	GridNode::SetGridDimensions(GRID_WIDTH, GRID_HEIGHT);

	// Initial spread-out locations
	Location vanInitLocs[NUM_VANS];
	vanInitLocs[0] = Location(10, 30);
	vanInitLocs[1] = Location(20, 20);
	vanInitLocs[2] = Location(30, 10);
	vanInitLocs[3] = Location(20, 30);
	vanInitLocs[4] = Location(30, 20);

	for (unsigned int i = 0; i < Vans.size(); ++i)
	{
		RouteVanAndSendInstructions(&dm, &Vans[i], aStar[Vans[i].Number], Vans[i].location, vanInitLocs[i]);
	}

	// Game loop
	while(true)
	{
		// Clear previous information
		Vans.clear();
		Edges->clear();
		waitingDeliveries.clear();
		activeDeliveries.clear();
		completedDeliveries.clear();

		dm.getInformation(time, *Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);

		// All the deliveries has been completed (or time is up), exit loop
		if (completedDeliveries.size() == 20 || time >= 1440)
		{
			break;
		}

		// Try to find more optimal routes for all the vans every 40 in-game seconds
		if (time % 40 == 0)
		{
			int test = 0;
			for (unsigned int i = 0; i < Vans.size(); ++i)
			{
				VanInfo* curVan = &Vans[i];

				if (curVanDelivery[curVan->Number] != -1)
				{
					DeliveryInfo* curDelivery = nullptr;

					// It is an waiting delivery
					if (curVan->cargo == -1)
					{
						for (unsigned int j = 0; j < waitingDeliveries.size(); ++j)
						{
							if (waitingDeliveries[j].Number == curVan->cargo)
							{
								curDelivery = &waitingDeliveries[j];
								break;
							}
						}

						if (curDelivery)
						{
							RouteVanAndSendInstructions(&dm, curVan, aStar[curVan->Number], curVan->location, curDelivery->pickUp);
						}
					}

					// It is a active delivery
					else
					{
						for (unsigned int j = 0; j < activeDeliveries.size(); ++j)
						{
							if (activeDeliveries[j].Number == curVan->cargo)
							{
								curDelivery = &activeDeliveries[j];
								break;
							}
						}

						if (curDelivery)
						{
							RouteVanAndSendInstructions(&dm, curVan, aStar[curVan->Number], curVan->location, curDelivery->dropOff);
						}
					}


				}
			}
		}

		// There are no deliveries to be done the during the first 60 seconds, try to spread out the vans
// 		if (time < TIME_SPREADOUT)
// 		{
// 			// Skip all subsequent code
// 			continue;
// 		}

		// Manage all the waiting deliveries
		for (unsigned int i = 0; i < waitingDeliveries.size(); ++i)
		{
			DeliveryInfo* curDelivery = &waitingDeliveries[i];

			// The delivery was found to have been assigned to a van
			if (vanDeliveryMap.count(curDelivery->Number) == 1 && vanDeliveryMap[curDelivery->Number] != -1)
			{
				continue;
			}

			// We have to find a suitable van for this delivery (find the closest van)
			VanInfo* targetVan = nullptr;
			int targetVanIndex = -1;

			for (unsigned int j = 0; j < Vans.size(); ++j)
			{
				VanInfo* curVan = &Vans[j];

				// If the van doesn't already have cargo and is not on a route to pick up cargo (the van is available)
				if (curVan->cargo == -1 && curVan->instructions.size() == 0 && curVanDelivery[curVan->Number] == -1)
				{
					if (targetVan == nullptr)
					{
						// The waiting delivery currently does not have any van targeted to it, set target to this van
						targetVan = curVan;
						targetVanIndex = j;
						continue;
					}

					// Otherwise, we calculate the distance from this van to the waiting delivery location,
					// and compare it with the previous targeted van distance
					if (targetVan != curVan)
					{
						if (gridNodes_2D[curVan->location.first][curVan->location.second]->GoalDistanceEstimate(*gridNodes_2D[curDelivery->pickUp.first][curDelivery->pickUp.second])
							< gridNodes_2D[targetVan->location.first][targetVan->location.second]->GoalDistanceEstimate(*gridNodes_2D[curDelivery->pickUp.first][curDelivery->pickUp.second]))
						{
							// Current van is closer to the pick-up location than the previous target van, set new target van
							targetVan = curVan;
							targetVanIndex = j;
						}
					}
				}
			}

			// We now have found the closest available van, route it to this pick-up location
			if (targetVan)
			{
				// Also tell the application that this van is instructed to this package
				vanDeliveryMap[waitingDeliveries[i].Number] = targetVan->Number;
				curVanDelivery[targetVan->Number] = waitingDeliveries[i].Number;
			}
		}

		// All the waiting deliveries has been processed, now we assign routes to the vans
		// Manage all the vans
		for (unsigned int i = 0; i < Vans.size(); ++i)
		{
			VanInfo* curVan = &Vans[i];

			// This van has cargo and is ordered to deliver it but no instructions assigned, find path to delivery drop-off location
			if (curVan->cargo != -1 && curVan->instructions.size() == 0 && curVanDelivery[curVan->Number] == curVan->cargo)
			{
				Location deliveryDropOffLoc;
				deliveryDropOffLoc.first = deliveryDropOffLoc.second = -1;

				// Find the delivery this van is assigned to
				for (unsigned int j = 0; j < activeDeliveries.size(); ++j)
				{
					if (curVan->cargo == activeDeliveries[j].Number)
					{
						// The delivery was found, find the location it is supposed to be dropped off at
						deliveryDropOffLoc = activeDeliveries[j].dropOff;
						break;
					}
				}

				// If the delivery drop off location is valid (and does exist), proceed to A*-route the van to this location
				if (deliveryDropOffLoc.first != -1 && deliveryDropOffLoc.second != -1)
				{
					RouteVanAndSendInstructions(&dm, curVan, aStar[curVan->Number], curVan->location, deliveryDropOffLoc);
				}
			}

			// The van has no cargo and no instructions, but a delivery assigned to it
			if (curVan->cargo == -1 && curVan->instructions.size() == 0 && curVanDelivery[curVan->Number] != -1)
			{
				DeliveryInfo* curDelivery = nullptr;
				// Find the delivery object
				for (unsigned int j = 0; j < waitingDeliveries.size(); ++j)
				{
					if (waitingDeliveries[j].Number == curVanDelivery[curVan->Number])
					{
						curDelivery = &waitingDeliveries[j];
						break;
					}
				}

				if (curDelivery)
				{
					RouteVanAndSendInstructions(&dm, curVan, aStar[curVan->Number], curVan->location, curDelivery->pickUp);
					vanDeliveryMap[curDelivery->Number] = curVan->Number;
				}

				// Delivery was not found, invalid delivery
				else
				{
					if (vanDeliveryMap[curVanDelivery[curVan->Number]] == curVan->Number)
						vanDeliveryMap[curVanDelivery[curVan->Number]] = -1;

					curVanDelivery[curVan->Number] = -1;
				}
			}

			// The van accidentally picked up a cargo on its way to pick up another cargo
			// Re-route to drop off this cargo instead
			if (curVan->cargo != -1 && curVanDelivery[curVan->Number] != curVan->cargo)
			{
				// Unassign the current delivery job first
				for (unsigned int j = 0; j < waitingDeliveries.size(); ++j)
				{
					if (waitingDeliveries[j].Number == curVanDelivery[curVan->Number])
					{
						vanDeliveryMap[waitingDeliveries[j].Number] = -1;
						break;
					}
				}

				DeliveryInfo* activeDelivery = nullptr;

				for (unsigned int j = 0; j < activeDeliveries.size(); ++j)
				{
					if (activeDeliveries[j].Number == curVan->cargo)
					{
						activeDelivery = &activeDeliveries[j];
						break;
					}
				}

				if (activeDelivery)
				{
					RouteVanAndSendInstructions(&dm, curVan, aStar[curVan->Number], curVan->location, activeDelivery->dropOff);
					vanDeliveryMap[activeDelivery->Number] = curVan->Number;
					curVanDelivery[curVan->Number] = activeDelivery->Number;
				}
			}

			// The van has cargo and no instructions, and the assigned delivery to it is different than its cargo
			if (curVan->cargo != -1 && curVan->instructions.size() == 0 && curVan->cargo != curVanDelivery[curVan->Number])
			{
				// Drop the current delivery job first
				for (unsigned int j = 0; j < waitingDeliveries.size(); ++j)
				{
					if (waitingDeliveries[j].Number == curVanDelivery[curVan->Number])
					{
						vanDeliveryMap[waitingDeliveries[j].Number] = -1;
						break;
					}
				}

				DeliveryInfo* activeDelivery = nullptr;

				for (unsigned int j = 0; j < activeDeliveries.size(); ++j)
				{
					if (activeDeliveries[j].Number == curVan->cargo)
					{
						activeDelivery = &activeDeliveries[j];
						break;
					}
				}

				if (activeDelivery)
				{
					RouteVanAndSendInstructions(&dm, curVan, aStar[curVan->Number], curVan->location, activeDelivery->dropOff);
					vanDeliveryMap[activeDelivery->Number] = curVan->Number;
					curVanDelivery[curVan->Number] = activeDelivery->Number;
				}
			}
		}

		// If a delivery has been completed, unassign the delivery for the van
		// One or more deliveries has been completed
		if (prevCompletedDeliveriesNum < completedDeliveries.size())
		{
			unsigned int completedNum = completedDeliveries.size() - prevCompletedDeliveriesNum;

			for (unsigned int i = completedDeliveries.size() - completedNum; i < completedDeliveries.size(); ++i)
			{
				// Find which van completed this delivery
				for (unsigned int j = 0; j < Vans.size(); ++j)
				{
					if (curVanDelivery[Vans[j].Number] == completedDeliveries[i].first)
					{
						curVanDelivery[Vans[j].Number] = -1;
						break;
					}
				}
			}
		}

		prevCompletedDeliveriesNum = completedDeliveries.size();

		for (unsigned int i = 0; i < Vans.size(); ++i)
		{
			std::cout << "Van " << Vans[i].Number << ": " << "x: " << Vans[i].location.first << " y: " 
				<< Vans[i].location.second << " cargo: " << Vans[i].cargo 
				<< " instructions: " << Vans[i].instructions.size() << 
				" delivery: " << curVanDelivery[Vans[i].Number] << std::endl;
		}

		std::cout << "Completed deliveries: " << completedDeliveries.size() << std::endl;
		std::cout << "Waiting deliveries: " << waitingDeliveries.size() << std::endl;
		std::cout << "Active deliveries: " << activeDeliveries.size() << std::endl;
	}

	// Clean-up
	for (unsigned int i = 0; i < gridNodes.size(); ++i)
	{
		delete gridNodes[i];
	}

	gridNodes.clear();

	for (unsigned int i = 0; i < aStar.size(); ++i)
	{
		delete aStar[i];
	}

	aStar.clear();

	return 0;
}