#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip> 
#include <random>
//#include "Json Classes.h"

#ifndef HTTP_DLL_1_00
#define HTTP_DLL_1_00 __declspec(dllimport)
#endif

class common_exception : public std::exception
{
public:
	common_exception (common_exception* e)
		:msg(e->msg)
	{
		delete e;
	}
	common_exception (const std::wstring& in)
		:msg(in)
	{}
	virtual ~common_exception () {}
	const std::wstring& getMessage() const {return msg;}
private:
	std::wstring msg;
};

class Common_Functions
{
public: 
	static void splitResponse (const std::wstring& Response,int& Code,std::wstring& Contents)
	{
		std::wstring startString;
		size_t l=Response.size();
		size_t n=0;
		for (;n<l;++n) 
		{
			if (Response[n]==L' ') {break;}
			startString.push_back(Response[n]);
		}
		ToNumber(Code,startString);
		if (n!=l) {
			Contents=std::wstring(Response,n+1,std::wstring::npos);
		}
	}	
	static void splitInput(const std::wstring& s,std::wstring& command,std::wstring& params) 
	{
		size_t n=0;
		size_t l=s.size();
		while (s[n]!=L' ' && n!=l)
		{
			command.push_back(s[n++]);
		}
		params.assign(s,n,s.npos);
		Common_Functions::Remove_Leading_And_Trailing_White_Spaces(params);
	}
	template <class T> static const T& getEmpty () {static T t;return t;}
	template <class T> static void ToNumber (T& out,const std::wstring& text)
	{
		//decimal base assumed, returns success/failure, num takes extracted number
		std::basic_istringstream<wchar_t> iss;
		iss.str(text);
		if ((iss >> out).fail()) {throw new common_exception(L"Could not retrieve number from string: "+text+L".");}
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
	static void string_to_wstring (const std::string& In,std::wstring& Out)
	{
		wchar_t* temp=new wchar_t[In.size()+1];
		try {
			size_t size_out=0;
			if (mbstowcs_s (&size_out,temp,In.size()+1,In.c_str(),In.size()+1)) {throw new common_exception(L"Could not convert string.");}
			else 
			{
				Out=std::basic_string<wchar_t>(temp);
				if (Out.size()!=In.size()) {throw new common_exception(L"Could not convert string.");}
			}
		} catch (std::exception* e) {
			delete[] temp;
			throw e;
		}
		delete[] temp;
	}
	static void wstring_to_string (const std::basic_string<wchar_t>& In,std::basic_string<char>& Out)
	{
		char* temp=new char[In.size()+1];
		try {
			size_t size_out=0;
			if (wcstombs_s (&size_out,temp,In.size()+1,In.c_str(),In.size()+1)) {throw new common_exception(L"Could not convert string.");}
			else
			{
				Out=std::basic_string<char>(temp);
				if (Out.size()!=In.size()) {throw new common_exception(L"Could not convert string.");}			
			} 
		} catch (std::exception* e) {
			delete[] temp;
			throw e;
		}
		delete[] temp;
	}
	//	Returns index of character AFTER closing bracket
	static size_t findBracketedSubstr (const std::wstring& In,const size_t Opens,std::wstring& Out,wchar_t Open,wchar_t Close)
	{
		size_t limit=In.size();
		if (limit<Opens+2 || In[Opens]!=Open) {throw new common_exception(L"Unexpected string.");}			

		size_t first=Opens,last=0,bracketCount=0;
		do {
			if (first==limit) {throw new common_exception(L"Unexpected string.");}
			if (In[first]==Open) {++bracketCount;}
			else if (In[first]==Close) {--bracketCount;}
			++first;
			if (bracketCount==0) {last=first;}
		} while (bracketCount>0);
		Out=In.substr(Opens,last);
		return last;
	}
	template <class T> static int getLargest (const std::vector<T>& v)
	{
		if (v.empty()) {return -1;}
		T Biggest=v.front();
		int out=0;
		size_t l=v.size();
		for (size_t n=1;n<l;++n)
		{
			if (v[n]>Biggest) {
				Biggest=v[n];
				out=static_cast<int>(n);
			}
		}
		return out;
	}
	template <class T> static int getSecondLargest (const std::vector<T>& v)
	{
		if (v.size()<2) {return -1;}
		int biggestIndex=getLargest(v);
		T Biggest=biggestIndex!=0?v.front():v[1];
		int out=biggestIndex!=0?0:1;
		size_t l=v.size();
		for (size_t n=1;n<l;++n)
		{
			if (n==biggestIndex){continue;}
			if (v[n]>Biggest) {
				Biggest=v[n];
				out=static_cast<int>(n);
			}
		}
		return out;
	}
	template <class T> static bool isMember (const std::vector<T>& v,const T& i)
	{
		return getIndex(v,i)!=v.size();
	}
	template <class T> static size_t getIndex (const std::vector<T>& v,const T& i)
	{
		size_t l=v.size();		
		for (size_t n=0;n<l;++n)
		{
			if (v[n]==i) {return n;}
		}
		return l;
	}
	static double getNormalProbability (double Mean,double StdDev,double X)
	{
		static double TwoPi=2*3.1415926535897932384626433832795028841971693994L;
		double Variance=StdDev*StdDev;
		return exp(-(((X-Mean)*(X-Mean))/(2*Variance)))
			/	sqrt(Variance * TwoPi);	
	}
	template <class Numeric> static Numeric Add (const std::vector<Numeric>& In)
	{
		Numeric out=0;
		size_t l=In.size();
		for (size_t n=0;n<l;++n)
		{
			out+=In[n];
		}
		return out;
	}
	template <class Numeric> static void Normalize (std::vector<Numeric>& In)
	{
		Numeric denom=Add(In);
		if (denom==0) {throw new common_exception(L"Divide by zero.");}
		size_t l=In.size();
		for (size_t n=0;n<l;++n)
		{
			In[n]/=denom;
		}
	}

	static std::default_random_engine Generate_DRE()
	{
		std::default_random_engine re;
		re.seed(time(NULL) % 27644437);
		return re;
	}

	static double rand_01 ()
	{
		// construct a trivial random generator engine from a time-based seed:
		static std::default_random_engine generator=Generate_DRE();
		static std::uniform_real_distribution<double> distribution (0.0,1.0);
		return distribution(generator);
	}
};