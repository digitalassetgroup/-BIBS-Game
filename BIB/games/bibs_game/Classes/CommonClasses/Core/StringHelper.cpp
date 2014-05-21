#include "StringHelper.h"

#include <locale>
#include <iostream>
#include <sstream>

#include "stdio.h"
#include <stdarg.h>
#include <stdlib.h>
#include <cctype>
#include "CollectionHelper.h"

using std::string;

string& StringHelper::trim(string& source, const string& delimiters)
{
	return trimr(triml(source, delimiters), delimiters);
}

string& StringHelper::trimr_char(string& source, const char delimiter)
{
	return source.erase(source.find_last_not_of(delimiter) + 1);
}

string& StringHelper::triml_char(string& source, const char delimiter)
{
	return source.erase(0, source.find_first_not_of(delimiter));
}

string& StringHelper::trimr(string& source, const string& delimiters)
{
	source.erase(source.find_last_not_of(delimiters) + 1);
	return source;
}

string& StringHelper::triml(string& source, const string& delimiters)
{
	source.erase(0, source.find_first_not_of(delimiters));
	return source;
}

string StringHelper::format_v(const char* fmt, va_list arglist)
{
	// Get formatted string length adding one for NULL
	size_t len = vsnprintf(NULL, 0, fmt, arglist) + 1;

	// Create a char vector to hold the formatted string.
	vector<char> buffer(len, '\0');
//	int nWritten = vsnprintf_s(&buffer[0], buffer.size(), len, fmt, marker);    
    int nWritten = vsnprintf(&buffer[0], buffer.size(), fmt, arglist);    
	if (nWritten > 0)
	{
		return &buffer[0];
	}

	return "";
}

//TODO: сделать версию, когда результат передается в string&
string StringHelper::format(const char* fmt, ...)
{ 
	if (fmt == NULL)
		return "";

	va_list marker;
	va_start(marker, fmt);  // initialize variable arguments
	string result = format_v(fmt, marker);	
	va_end(marker);   // Reset variable arguments

	return result; 
}

//sample:
//vector<string> tokens;
//string str("Split me up! Word1 Word2 Word3.");
//Tokenize(str, tokens);

//maxCount - максимальное число токенов, которое может быть добавлено в tokens; -1 - не установлено макс. число
//return - символ строки str, на котором остановились
string::size_type StringHelper::Tokenize(const string& str, vector<string>& tokens, const string& delimiters, int maxCount)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

	int i = 0;
    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));

		if (++i == maxCount)
			break;

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }

	return pos;
}

int StringHelper::ToInt32(const string& source)
{
	return atoi(source.c_str());
}

float StringHelper::ToFloat(const string& source)
{
	return (float)atof(source.c_str());
}

void StringHelper::ToLower(string& source)
{
	for (unsigned int i = 0; i < source.length(); ++i)
	{
		source[i] = tolower(source[i]);
	}
}

string StringHelper::ToLower(const string& source)
{
	string res;
	res.resize(source.length());
	for (unsigned int i = 0; i < source.length(); ++i)
	{
		res[i] = tolower(source[i]);
	}

	return res;
}

void StringHelper::Replace(string& value, string const& search, string const& replace)
{
    std::string::size_type  next;
    
    for(next = value.find(search);        // Try and find the first match
        next != std::string::npos;        // next is npos if nothing was found
        next = value.find(search,next)    // search for the next match starting after
        // the last match that was found.
        )
    {
        // Inside the loop. So we found a match.
        value.replace(next,search.length(),replace);   // Do the replacement.
        next += replace.length();                      // Move to just after the replace
        // This is the point were we start
        // the next search from. 
    }
}

string StringHelper::FromWStringToString(const std::wstring& str)
{
	std::ostringstream stm;
	const std::ctype<char>& ctfacet = std::use_facet<std::ctype<char>>(stm.getloc());
	for (size_t i = 0; i < str.size(); ++i)
		stm << ctfacet.narrow(str[i], 0);
	return stm.str();
}

std::wstring StringHelper::FromStringToWString(const std::string& str)
{
	std::wostringstream wstm;
	const std::ctype<wchar_t>& ctfacet = std::use_facet<std::ctype<wchar_t> >( wstm.getloc() );
	for (size_t i=0; i<str.size(); ++i)
		wstm << ctfacet.widen(str[i]);
	return wstm.str();
}


#if UNIT_TEST == 1
	static bool unittest()
	{
		string s = "  asdasd  \n\t";
		string sRes = StringHelper::trim(s);  //asdasd

		string s1 = StringHelper::format("_%s_%d_asd_%f", "324", 1, 2.566f);  //_324_1_asd_2.566000

		string s2 = "aaaa\n"
					"bbbb\n"
					"cccc";
		vector<string> tokens;
		StringHelper::Tokenize(s2, tokens, "\n");

		StringBuilder s3; s3 << 11 << "234234";
		string s4 = s3.str();

		return true;
	}

	static bool isR = unittest();
#endif
