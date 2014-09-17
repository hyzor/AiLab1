#pragma once

#include <string> 
#include <vector>
#include <map>
#include "Common Functions.h"

#define DM_CLIENT_LOCAL

#ifndef DM_CLIENT_DLL
#define DM_CLIENT_DLL __declspec(dllimport)
#endif

class DM_Exception : public common_exception
{
public:
	DM_Exception (common_exception* e)
		:common_exception(e)
	{}
	DM_Exception (const std::wstring& in)
		:common_exception(in)
	{}
};

typedef std::pair<int,int> Location; // Y,X

struct VanInfo {
	VanInfo(int i) :Number(i),cargo(-1),location(-1,-1) {}
	int Number;
	Location location;
	std::vector<Location> instructions;
	int cargo; // -1 is no cargo
};
struct DeliveryInfo {
	int Number;
	int Time;
	Location pickUp;
	Location dropOff;
};

class DeliveryMan;
class http_pImple;
class Json_Array;
class DM_CLIENT_DLL DM_Client 
{
public:
	DM_Client (const std::wstring& Group,bool& OK);
	virtual ~DM_Client ();
	
	// NodeTypes: "hw": highway , "sub" : suburban , "bd" : business district , "cbd" : central business district 
	static const std::wstring getHighayString () {static std::wstring s=L"hw";return s;}	
	static const std::wstring getSuburbanString () {static std::wstring s=L"sub";return s;}
	static const std::wstring getBusinessDistrictString () {static std::wstring s=L"bd";return s;}	
	static const std::wstring getCentralBusinessDistrictString () {static std::wstring s=L"cbd";return s;}	

	void startGame (std::vector<std::vector<std::wstring>>&,std::wstring& output);
	void getInformation (int& time,std::vector<std::vector<int>>& Edges,std::vector<VanInfo>& Vans,std::vector<DeliveryInfo>& waitingDeliveries,std::vector<DeliveryInfo>& activeDeliveries,
		std::vector<std::pair<int,int>>& completedDeliveries,std::wstring& output);
	void sendInstructions (const std::map<int,std::vector<std::pair<int,int>>>& instructions,std::wstring& output);
private:
#ifndef DM_CLIENT_LOCAL
	http_pImple* dmEngine;
	static const std::vector<std::wstring>& getMediaType ();
	static const std::wstring& getPathBase ();
	static int getPort ();
	static const std::wstring& getExtensionBase ();
#else
	DeliveryMan* dmEngine;
#endif
	static void getLocationsVector (const Json_Array& In,std::vector<Location>& Out);
};