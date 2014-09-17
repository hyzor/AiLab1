
#include "Json Library.h"
#include <sstream>
using namespace std;


int Json_Base::Fetch_As_Integer (wstring temp)
{
	int out;
	Common_Functions::ToNumber(out,temp);
	return out;
}
double Json_Base::Fetch_As_Double (wstring temp)
{
	double out;
	Common_Functions::ToNumber(out,temp);
	return out;
}
bool Json_Base::Fetch_As_Boolean (wstring temp)
{
	Common_Functions::Remove_Leading_And_Trailing_White_Spaces(temp);
	if (temp==L"true") {return true;}
	else if (temp==L"false") {return false;}
	throw new Json_Parse_Exception(L"Invalid input: "+temp+L".");
}
void Json_Base::Fetch_As_Json_Array (std::wstring temp,Json_Array& Out)
{
	Validate_And_Remove_Ends(temp,L'[',L']');
	wstring Value;
	while (Fetch_Next_Value(temp,Value))
	{
		Out.Contents.push_back(Value);
		Value.clear();
	}
}
void Json_Base::Validate_And_Remove_Ends(std::wstring& in,wchar_t front,wchar_t back)
{
	Common_Functions::Remove_Leading_And_Trailing_White_Spaces(in);
	if (in.size()<2||in.front()!=front||in.back()!=back) {
		throw new Json_Parse_Exception(L"Invalid input: "+in+L".");
	} else {
		in.pop_back();
		in=in.substr(1);
	}
}
void Json_Base::Possible_Validate_And_Remove_Ends(std::wstring& in,wchar_t front,wchar_t back)
{
	Common_Functions::Remove_Leading_And_Trailing_White_Spaces(in);
	if (in.empty()) {throw new Json_Parse_Exception(L"Invalid input: "+in+L".");}
	if (in.front()==front || in.back()==back) {Validate_And_Remove_Ends(in,front,back);}
}
bool Json_Base::Fetch_Next_Key(std::wstring& in,std::wstring& out)
{
	Common_Functions::Remove_Leading_And_Trailing_White_Spaces(in);
	if (in.empty()) {return false;}
	size_t i=0;
	size_t l=in.size();
	while (i<l && in[i]!=L':') {++i;}
	if (i==0||i==l) {throw new Json_Parse_Exception(L"Invalid input: "+in+L".");}
	out=in.substr(0,i++);
	//	Keys must be strings or constants. Constants come to be treated as strings...
	Possible_Validate_And_Remove_Ends(out,L'"',L'"');
	if (i==l) {throw new Json_Parse_Exception(L"Invalid input: "+in+L".");}
	in=in.substr(i,in.npos);
	return true;
}
bool Json_Base::Fetch_Next_Value(std::wstring& in,std::wstring& out)
{
	Common_Functions::Remove_Leading_And_Trailing_White_Spaces(in);
	if (in.empty()) {return false;}
	size_t openSqrBrackets=0,openCurlyBrackets=0;
	bool openQuotes=false;
	bool escape=false;
	size_t i=0;
	size_t l=in.size();
	while (i<l && (openSqrBrackets+openCurlyBrackets+openQuotes!=0 || in[i]!=L',')) 
	{
		if (escape) {escape=false;}
		else {
			if (in[i]==L'\\') {escape=true;}
			else if (in[i]==L'{') {++openCurlyBrackets;}
			else if (in[i]==L'}') {--openCurlyBrackets;}
			else if (in[i]==L'[') {++openSqrBrackets;}
			else if (in[i]==L']') {--openSqrBrackets;}
			else if (in[i]==L'"') {openQuotes=!openQuotes;}
		}
		++i;
	}
	if (openSqrBrackets+openCurlyBrackets+openQuotes!=0) {throw new Json_Parse_Exception(L"Invalid input: "+in+L".");}
	out=in.substr(0,i);
	in=in.substr(i+(i!=l),in.npos);
	return true;
}
void Json_Base::Fetch_As_Json_Map (std::wstring temp,Json_Map& Out)
{
	Validate_And_Remove_Ends(temp,L'{',L'}');
	wstring Key,Value;
	while (Fetch_Next_Key(temp,Key) && Fetch_Next_Value(temp,Value))
	{
		Out.Contents[Key]=Value;
		Key.clear();
		Value.clear();
	}
}
Json_Array::Json_Array (const std::wstring& In,bool& OK)
{
	try {
		Json_Base::Fetch_As_Json_Array(In,*this);
		OK=true;
	} catch (Json_Parse_Exception* e) {
		delete e;
		OK=false;
	}
}
void Json_Base::Fetch_As_String (wstring temp,std::wstring& Out)
{
	Common_Functions::Remove_Leading_And_Trailing_White_Spaces(temp);
	if (temp.size()<2||temp.front()!=L'\"'||temp.back()!=L'\"') {
		throw new Json_Parse_Exception(L"Invalid input: "+temp+L".");
	}
	temp.pop_back();
	Out=temp.substr(1);
}
void Json_Array::Fetch_As_String (const const_iterator& it__,std::wstring& Out)
{
	Json_Base::Fetch_As_String(*it__,Out);
}
int Json_Array::Fetch_As_Integer (const const_iterator& it__)
{
	return Json_Base::Fetch_As_Integer(*it__);
}
double Json_Array::Fetch_As_Double (const const_iterator& it__)
{
	return Json_Base::Fetch_As_Double(*it__);
}
bool Json_Array::Fetch_As_Boolean (const const_iterator& it__)
{
	return Json_Base::Fetch_As_Boolean(*it__);
}
void Json_Array::Fetch_As_Json_Array (const const_iterator& it__,Json_Array& Out)
{
	Json_Base::Fetch_As_Json_Array(*it__,Out);
}
void Json_Array::Fetch_As_Json_Map (const const_iterator& it__,Json_Map& Out)
{
	Json_Base::Fetch_As_Json_Map(*it__,Out);
}
void Json_Array::Fetch_As_Vector (std::vector<std::wstring>& Out) const
{
	const_iterator l=end();
	for (const_iterator i=begin();i!=l;++i)
	{
		Out.push_back(wstring());
		Json_Base::Fetch_As_String(*i,Out.back());
	}
}
void Json_Array::Fetch_As_Vector (std::vector<int>& Out) const
{
	const_iterator l=end();
	for (const_iterator i=begin();i!=l;++i)
	{
		Out.push_back(Json_Base::Fetch_As_Integer(*i));
	}
}
void Json_Array::Fetch_As_Vector (std::vector<double>& Out) const
{
	const_iterator l=end();
	for (const_iterator i=begin();i!=l;++i)
	{
		Out.push_back(Json_Base::Fetch_As_Double(*i));
	}
}

Json_Map::Json_Map (const std::wstring& In,bool& OK) 
{
	try {
		Json_Base::Fetch_As_Json_Map(In,*this);
		OK=true;
	} catch (Json_Parse_Exception* e) {
		delete e;
		OK=false;
	}
}
bool Json_Map::has (const std::wstring& item) const 
{
	return Contents.find(item)!=Contents.end();
}
void Json_Map::check (const std::wstring& item) const // throws missingException on fail
{
	if (!has(item))
	{
		throw new Json_Parse_Exception(L"Missing input: "+item+L".");
	}
}
void Json_Map::Fetch_As_String (const std::wstring& key,std::wstring& Out) const
{	
	std::map<std::wstring,std::wstring>::const_iterator it__=Contents.find(key);
	if (it__==Contents.end()) {
		throw new Json_Parse_Exception(L"Missing input: "+key+L".");
	}
	Json_Base::Fetch_As_String(it__->second,Out);
}
int Json_Map::Fetch_As_Integer (const std::wstring& key) const
{
	std::map<std::wstring,std::wstring>::const_iterator it__=Contents.find(key);
	if (it__==Contents.end()) {
		throw new Json_Parse_Exception(L"Missing input: "+key+L".");
	}
	return Json_Base::Fetch_As_Integer(it__->second);
}
double Json_Map::Fetch_As_Double (const std::wstring& key) const
{
	std::map<std::wstring,std::wstring>::const_iterator it__=Contents.find(key);
	if (it__==Contents.end()) {
		throw new Json_Parse_Exception(L"Missing input: "+key+L".");
	}
	return Json_Base::Fetch_As_Double(it__->second);
}
bool Json_Map::Fetch_As_Boolean (const std::wstring& key) const
{
	std::map<std::wstring,std::wstring>::const_iterator it__=Contents.find(key);
	if (it__==Contents.end()) {
		throw new Json_Parse_Exception(L"Missing input: "+key+L".");
	}
	return Json_Base::Fetch_As_Boolean(it__->second);
}
void Json_Map::Fetch_As_Json_Array (const std::wstring& key,Json_Array& Out) const
{
	std::map<std::wstring,std::wstring>::const_iterator it__=Contents.find(key);
	if (it__==Contents.end()) {
		throw new Json_Parse_Exception(L"Missing input: "+key+L".");
	}
	Json_Base::Fetch_As_Json_Array(it__->second,Out);
}
void Json_Map::Fetch_As_Json_Map (const std::wstring& key,Json_Map& Out) const
{
	std::map<std::wstring,std::wstring>::const_iterator it__=Contents.find(key);
	if (it__==Contents.end()) {
		throw new Json_Parse_Exception(L"Missing input: "+key+L".");
	}
	Json_Base::Fetch_As_Json_Map(it__->second,Out);
}
void Json_Map::Fetch_As_String (const const_iterator& it__,std::wstring& Out)
{
	Json_Base::Fetch_As_String(it__->second,Out);
}
int Json_Map::Fetch_As_Integer (const const_iterator& it__)
{
	return Json_Base::Fetch_As_Integer(it__->second);
}
int Json_Map::Fetch_Key_As_Integer (const const_iterator& it__)
{
	return Json_Base::Fetch_As_Integer(it__->first);
}
double Json_Map::Fetch_As_Double (const const_iterator& it__)
{
	return Json_Base::Fetch_As_Double(it__->second);
}
bool Json_Map::Fetch_As_Boolean (const const_iterator& it__)
{
	return Json_Base::Fetch_As_Boolean(it__->second);
}
void Json_Map::Fetch_As_Json_Array (const const_iterator& it__,Json_Array& Out)
{
	Json_Base::Fetch_As_Json_Array(it__->second,Out);
}
void Json_Map::Fetch_Key_As_Json_Array (const const_iterator& it__,Json_Array& Out)
{
	Json_Base::Fetch_As_Json_Array(it__->first,Out);
}
void Json_Map::Fetch_As_Json_Map (const const_iterator& it__,Json_Map& Out)
{
	Json_Base::Fetch_As_Json_Map(it__->second,Out);
}
