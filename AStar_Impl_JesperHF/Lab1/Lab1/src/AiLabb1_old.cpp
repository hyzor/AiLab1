// AiLabb1.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <map>
#include <vector>
#include "DeliveryManClient.h"
#pragma comment (lib, "DeliveryManClient")
#include <iostream>
#include <tchar.h>

std::map<int,std::vector<std::pair<int,int>>> aStar(std::vector<std::vector<int>> Edges, std::vector<VanInfo> Vans, std::vector<DeliveryInfo> waitingDeliveries)
{
	//Magic happens...
	//Implementera en A*-algorithm

	std::map<int, std::vector<std::pair<int, int>>> instruction;
	return instruction;
}

int _tmain(int argc, _TCHAR* argv[])
{
	const std::wstring group(L"Skynet");
	bool ok;
	DM_Client dm(group, ok);

	std::vector<std::vector<std::wstring>> nodes;
	std::wstring output;
	dm.startGame(nodes, output);
	std::wcout << output << std::endl;

	int time;												//	Current time (in minutes). The game starts at 6:00=360 and ends at 24:00 = 1440
	std::vector<std::vector<int>> Edges;					//	The current traffic conditions on all edges: The number of minutes a van will take to travel over the edge.
	std::vector<VanInfo> Vans;								//	The van information for each van (see above)
	std::vector<DeliveryInfo> waitingDeliveries;			//	The delivery information for each waiting (not yet picked up) delivery (see above).
	std::vector<DeliveryInfo> activeDeliveries;				//	The delivery information for each active (currently on a van) delivery (see above).
	std::vector<std::pair<int,int>> completedDeliveries;	//	The delivery number and time taken for each completed delivery.


	while(true)
	{
		dm.getInformation(time, Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);
		std::wcout << output << std::endl;

		//Algorithm: (derp: ska rutt läggas om vid varje tidssteg eller ska bilarna slutföra ursprungligt planerad rutt?)
		//Kika på paket som ska hämtas (waitingDeliveries)
		//På nåt sätt bedöma vilken bil som ska ska ta hand om vilket paket...

		for (int i = 0; i < waitingDeliveries.size(); i++) //Tillfällig lösning
		{
			if (i < Vans.size())
			{
				if (Vans[i].cargo == -1)
				{
					//Vans[i].cargo = waitingDeliveries[i].Number;

					// Find 
				}
			}
		}

		

		//Tillämpa A* på varje bil-paket-uppsättning
		std::map<int,std::vector<std::pair<int,int>>> instructions = aStar(Edges, Vans, waitingDeliveries);
		
		dm.sendInstructions(instructions, output);
		std::wcout << output << std::endl;
	}

	return 0;
}

