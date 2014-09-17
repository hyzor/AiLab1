#ifndef JSON_HEADER_ONLY
#define JSON_HEADER_ONLY

#include <sstream>
#include <vector>
#include <map>
#include <iomanip> 

class Json_Parse_Exception : public std::exception 
{	
public:
	Json_Parse_Exception (const std::wstring& in)
		:msg(in)
	{}
	const std::wstring& getMessage() const {return msg;}
private:
	std::wstring msg;
};
class Common_Functions
{
public: 
	template <class T> static const T& getEmpty () {static T t;return t;}
	template <class T> static void ToNumber (T& out,const std::wstring& text)
	{
		//decimal base assumed, returns success/failure, num takes extracted number
		std::basic_istringstream<wchar_t> iss;
		iss.str(text);
		if ((iss >> out).fail()) {throw new Json_Parse_Exception(L"Could not retrieve number from string: "+text+L".");}
	}
	static std::wstring ToString (int num)
	{
		std::basic_ostringstream<wchar_t> oss;
		oss << std::setprecision(6) << num;
		return oss.str();
	}
	static std::wstring ToString (unsigned long num)
	{
		std::basic_ostringstream<wchar_t> oss;
		oss << std::setprecision(6) << num;
		return oss.str();
	}
	static std::wstring ToString (size_t num)
	{
		std::basic_ostringstream<wchar_t> oss;
		oss << std::setprecision(6) << num;
		return oss.str();
	}
	static std::wstring ToString (double num)
	{
		std::basic_ostringstream<wchar_t> oss;
		oss << std::setprecision(6) << num;
		return oss.str();
	}
	static const std::wstring& ToString (bool b)
	{
		static std::wstring t=L"true";
		static std::wstring f=L"false";
		return b?t:f;
	}
	static void Seperate_Strings (std::wstring In,std::vector<std::wstring>& Out,wchar_t t,bool Has_Last)
	{	
		std::basic_stringstream<wchar_t> ss(In);
		std::wstring next;
		bool Cont=true;
		while (Cont)
		{
			while (ss.peek()==L' '){ss.get();}//Remove leading whitespace
			getline(ss,next,t);
			if ((Cont=(!ss.eof())) || Has_Last)
			{
				if (!next.empty()){while (next.back()==L' '){next.pop_back();}}//Remove trailing whitespace
				if (!next.empty()) {Out.push_back(next);}
			}
		}
	}
	static void Remove_Leading_And_Trailing_White_Spaces(std::wstring& In)	
	{
		Remove_Trailing_White_Spaces(In);
		Remove_Leading_White_Spaces(In);
	}
	static void Remove_Leading_White_Spaces(std::wstring& In)	
	{
		size_t l=In.size();
		for (size_t n=0;n<l;++n)
		{
			if (!isspace(In[n])){In.swap(std::wstring(In,n,l-n));return;}
		}
		In.clear();
	}
	static void Remove_Trailing_White_Spaces(std::wstring& In)	
	{   
		while (!In.empty()&&isspace(In.back())){In.pop_back();}
	}
};


class Json_Map;
class Json_Array;
class Json_Base
{
protected:
	//	Notice pass by value
	static void Fetch_As_String (std::wstring In,std::wstring& Out);
	static int Fetch_As_Integer (std::wstring In);
	static double Fetch_As_Double (std::wstring In);
	static bool Fetch_As_Boolean (std::wstring In);
	static void Fetch_As_Json_Array (std::wstring In,Json_Array& Out);
	static void Fetch_As_Json_Map (std::wstring In,Json_Map& Out);
	static void Validate_And_Remove_Ends(std::wstring&,wchar_t,wchar_t);
	static void Possible_Validate_And_Remove_Ends(std::wstring&,wchar_t,wchar_t);
	static bool Fetch_Next_Key(std::wstring& in,std::wstring& out);
	static bool Fetch_Next_Value(std::wstring& in,std::wstring& out);
public:
	//	Returns if key/value was present
	bool Fetch_And_Remove_Internal_Json_Map (std::wstring& Str,const std::wstring& Key,Json_Map& Out);
};
class Json_Array : public Json_Base
{
	friend Json_Base;
public:
	Json_Array () {}
	Json_Array (const std::wstring&,bool& OK);

	typedef std::vector<std::wstring>::iterator iterator;
	typedef std::vector<std::wstring>::const_iterator const_iterator;

	iterator begin () {return Contents.begin();}
	const_iterator begin () const {return Contents.begin();}
	iterator end () {return Contents.end();}
	const_iterator end () const {return Contents.end();}
	
	void Fetch_As_Vector (std::vector<std::wstring>& Out) const;
	void Fetch_As_Vector (std::vector<double>& Out) const;
	void Fetch_As_Vector (std::vector<int>& Out) const;

	static void Fetch_As_String (const const_iterator& it__,std::wstring& Out);
	static int Fetch_As_Integer (const const_iterator& it__);
	static double Fetch_As_Double (const const_iterator& it__);
	static bool Fetch_As_Boolean (const const_iterator& it__);
	static void Fetch_As_Json_Array (const const_iterator& it__,Json_Array& Out);
	static void Fetch_As_Json_Map (const const_iterator& it__,Json_Map& Out);
private:
	std::vector<std::wstring> Contents;
};
class Json_Map : public Json_Base
{
	friend Json_Base;
public:
	Json_Map () {}
	Json_Map(const std::wstring&,bool& OK);

	typedef std::map<std::wstring,std::wstring>::iterator iterator;
	typedef std::map<std::wstring,std::wstring>::const_iterator const_iterator;

	iterator begin () {return Contents.begin();}
	const_iterator begin () const {return Contents.begin();}
	iterator end () {return Contents.end();}
	const_iterator end () const {return Contents.end();}
	
	size_t size () const {return Contents.size();}

	bool has (const std::wstring& key) const;
	void check (const std::wstring& key) const; // throws missingInputException on failure

	void Fetch_As_String (const std::wstring& In,std::wstring& Out) const;
	int Fetch_As_Integer (const std::wstring& In) const;
	double Fetch_As_Double (const std::wstring& In) const;
	bool Fetch_As_Boolean (const std::wstring& In) const;
	void Fetch_As_Json_Array (const std::wstring& In,Json_Array& Out) const;
	void Fetch_As_Json_Map (const std::wstring& In,Json_Map& Out) const;
	
	static void Fetch_As_String (const const_iterator& it__,std::wstring& Out);
	static int Fetch_As_Integer (const const_iterator& it__);
	static double Fetch_As_Double (const const_iterator& it__);
	static bool Fetch_As_Boolean (const const_iterator& it__);
	static void Fetch_As_Json_Array (const const_iterator& it__,Json_Array& Out);
	static void Fetch_As_Json_Map (const const_iterator& it__,Json_Map& Out);

	static int Fetch_Key_As_Integer (const const_iterator& it__);
	static void Fetch_Key_As_Json_Array (const const_iterator& it__,Json_Array& Out);
private:
	std::map<std::wstring,std::wstring> Contents;	//	Keys are strings, not Json strings, but can be cast
};


#endif