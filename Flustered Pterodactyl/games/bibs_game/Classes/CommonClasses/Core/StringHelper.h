#pragma once
#ifndef StringHelper_h
#define StringHelper_h

#include <string>
#include <vector>
#include <sstream>
using std::string;
using std::vector;

namespace StringHelper
{
	const string DefaultDelimiters = "\r\t\n ";

	string& trimr(string& source, const string& delimiters = DefaultDelimiters);
	string& triml(string& source, const string& delimiters = DefaultDelimiters);
	string& trim(string& source, const string& delimiters = DefaultDelimiters);
	inline string& trimr_char(string& source, const char delimiter = ' ');
	inline string& triml_char(string& source, const char delimiter = ' ');

	int ToInt32(const string& source);
	float ToFloat(const string& source);
    void Replace(string& value, string const& search, string const& replace);
    
	template <class T>
	inline string ToString(const T& t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
    
    template<typename T>
    bool ConvertToT(const std::string& str, T& result)
    {
        std::istringstream iss(str);
        T obj;
        
        iss >> std::ws >> obj >> std::ws;
        
        if(!iss.eof())
            return false;
        result = obj;
        return true;
    }

	string format(const char* fmt, ...);
	string format_v(const char* fmt, va_list arglist);
	string::size_type Tokenize(const string& str, vector<string>& tokens, 
		const string& delimiters = " ", int maxCount = -1);
	void ToLower(string& source);
	string ToLower(const string& source);

	//разделить строку str на подстроки, не превышающей длину length
	template<typename TCollection>
	void SplitByLength(TCollection& result, const string& str, unsigned int length)
	{
		for (unsigned int i = 0; i < str.size(); i += length)
		{
			unsigned int currentLength = (i + length > str.size()) ? str.size() - i : length;
			result.push_back(str.substr(i, currentLength));
		}
	}

	template<typename TCollection>
	void Join(string& result, TCollection& strings, const string& delimiter)
	{
		bool isAddDelimiter = false;

		//NOTE: gcc не понимает for_each_iterator(strings) :'(
		for (auto it = strings.begin(), itEnd = strings.end(); it != itEnd; ++it)
		{
			string& s = *it;

			if (!isAddDelimiter) isAddDelimiter = true;
			else result += delimiter;

			result += s;
		}
	}

	string FromWStringToString(const std::wstring& str);
	std::wstring FromStringToWString(const string& str);	
}

typedef std::stringstream StringBuilder;

#define MAKE_STRING(tokens) \
    static_cast<std::ostringstream&>(          \
        std::ostringstream().flush() << tokens \
    ).str()

#endif
