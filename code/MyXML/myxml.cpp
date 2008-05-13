//----------------------------------------------------------------------------

///@file
///MyXML library implementation file.
///
///@par License:
///@verbatim
///MyXML - My XML library for parsing and manipulating XML files.
///Copyright (C) 2005-06 Tomasz Nurkiewicz
///For full license text see license.txt.
///
///This program is free software; you can redistribute it and/or modify it under the terms of
///the GNU General Public License as published by the Free Software Foundation;
///either version 2 of the License, or (at your option) any later version.
///
///This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
///without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
///See the GNU General Public License for more details.
///
///You should have received a copy of the GNU General Public License along with this program;
///if not, write to the Free Software Foundation, Inc.,
///59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///@endverbatim
///
///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
///@date Sep 2005

//----------------------------------------------------------------------------

#include <fstream>
#include <utility>
#include "myxml.h"
using std::endl;
using std::string;

//----------------------------------------------------------------------------

void MyXML::Key::toStream(std::ostream& os, const std::string& keyName, int indent) const
	{
	if(value_.empty() && keys_.empty() && attributes.empty())
		return;
	os << string(indent, INDENT_CHAR) << '<' << keyName;
	for(std::map<string, string>::const_iterator i = attributes.begin(); i != attributes.end(); ++i)
		if(!i->second.empty())		//print only non-empty attributes
			os << ' ' << i->first << "=\"" << i->second << '"';
	if(!value_.empty() || !keys_.empty())
		{
		os << '>';
		if(!keys_.empty())
			{
			streamAllSubKeys(os, indent + 1);
			os << string(indent, INDENT_CHAR);
			}
		else
			if(!value_.empty())
				os << value_;
		os << "</" << keyName;
		}
	else
		os << " /";
	os << '>' << endl;
	}

void MyXML::Key::streamAllSubKeys(std::ostream& os, int indent) const
	{
	os << endl;
	for(KeysMap::const_iterator i = keys_.begin(); i != keys_.end(); ++i)
		i->second.toStream(os, i->first, indent);
	}

MyXML::Key& MyXML::Key::operator[](const std::string& keyName)
	{
	switch(keys_.count(keyName))
		{
		//no data at keyName saved, create empty key keyName
		case 0: return keys_.insert(make_pair(keyName, Key()))->second;
		//expected situation: only one unambigous item with key keyName
		case 1: return keys_.find(keyName)->second;
		//more than one items with key keyName - throw an exception
		default: throw MyXML::Exception('\'' + keyName + "' key name ambiguous");
		}
	}

const MyXML::Key& MyXML::Key::operator[](const std::string& keyName) const
	{
	switch(keys_.count(keyName))
		{
		//no data at keyName saved, create empty key keyName
		case 0: throw MyXML::Exception('\'' + keyName + "' key name not found");
		//expected situation: only one unambigous item with key keyName
		case 1: return keys_.find(keyName)->second;
		//more than one items with key keyName - throw an exception
		default: throw MyXML::Exception('\'' + keyName + "' key name ambiguous");
		}
	}

const std::string& MyXML::Key::attribute(const std::string& attName) const
	{
	if(attributes.count(attName) == 0)
		throw MyXML::Exception('\'' + attName + "' attribute not found");
	return attributes.find(attName)->second;
	}

void MyXML::Key::clear()
	{
	value_.clear();
	attributes.clear();
	keys_.clear();
	}

void MyXML::Key::saveToFile(const std::string& fileName) const
	{
	std::ofstream xmlFile(fileName.c_str());
	if(!xmlFile)
		throw Exception("Can't create file \"" + fileName + '"');
	xmlFile << *this;
	}

void MyXML::Key::loadFromFile(const std::string& fileName)
	{
	std::ifstream xmlFile(fileName.c_str());
	if(!xmlFile)
		throw Exception("Can't open file \"" + fileName + '"');
	try
		{
		xmlFile >> *this;
		}
	catch(const MyXML::Exception& e)
		{
		throw MyXML::Exception("XML file '" + fileName + "' exception:\n" + e.what());
		}
	}

//----------------------------------------------------------------------------

///Internal MyXML class used in loading process.
///This class associates with input stream and produces a lexem stream. Lexem is a singel
///XML element like symbol, identifier or string data.
///@sa operator>>(std::istream& is, MyXML::Key& root)
class XMLStream
	{
	private:
		///ID number of lexem containing symbols
		///@sa Lexem
		static const int SYMBOL = 1;
		///ID number of lexem containing identifier.
		///@sa Lexem
		///Identifier is a key or attribute name
		static const int IDENTIFIER = 2;
		///ID number of lexem containing data.
		///Data is a string (which might contain white spaces and special characters) corresponding
		///to key or attribute values.
		///@sa Lexem
		static const int DATA = 3;

		///Lexem data.
		///First element of pair is the lexem ID number and the second one is the lexem itself.
		typedef std::pair<int, string> Lexem;
		///Input string to read from.
		///It must be a text file containing valid XML data.
		///@sa nextLexem()
		std::istream& is;
		///Expect data in next lexem flag.
		///This flag is set when there is possibility that the next lexem will be a data string. This
		///tells nextLexem() method to interpret white spaces different (not as the end of lexem).
		///@sa nextLexem()
		bool expectData;
		///Private constructor.
		///This is at least strange to make constructor private, but there is one function -
		///operator>>(std::istream& is, MyXML::Key& root) - which is a friend of XMLStream and
		///can create an instance if this class. This makes impossible to use XMLStream anywhere except
		///operator>>(std::istream& is, MyXML::Key& root)
		///@param iis Input stream (typically input file) to read XML lexems.
		///@sa operator>>(std::istream& is, MyXML::Key& root)
		XMLStream(std::istream& iis);
		///Returns the next XML lexem.
		///@return Lexem object containing next lexem in stream.
		const Lexem nextLexem();
		///Is the given char a valid XML symbol?
		///@param c Character to check.
		///@return True if a given character is one of the XML valid symbols.
		///@sa nextLexem()
		static bool validChar(char c)
			{return (c == '>') || (c == '"') || (c == '<') || (c == '/') || (c == '=') || (c == '?');}
		///Loads XML data into Key object using lexem stream.
		///Because this function is called recursively, it can read the whole file when it's run
		///for topmost key.
		///@param root Initial key to load the XML data (the topmost key).
		///@sa operator>>(std::istream& is, MyXML::Key& root)
		void load(MyXML::Key& root);
	public:
		friend std::istream& MyXML::operator>>(std::istream& is, MyXML::Key& root);
	};

//----------------------------------------------------------------------------

XMLStream::XMLStream(std::istream& iis): is(iis), expectData(false)
	{
	nextLexem();		//skip '<'
	nextLexem();		//skip '?'
	while(nextLexem() != make_pair(SYMBOL, string(1, '?')))
		;//look after '?>' closing key
	nextLexem();		//skip '>'
	}

const XMLStream::Lexem XMLStream::nextLexem()
	{
	char c;
	is >> c;
	if(expectData)
		{
		if((c == '<') || (c == '"'))
			{
			expectData = false;
			return make_pair(SYMBOL, string(1, c));
			}
		string text;
		do
			{
			text += c;
			is.get(c);
			}
		while((c != '"') && (c != '<'));		//complete the string alnum words
		if(!isspace(c))
			is.seekg(-1, std::ios::cur);		//move back one non alnum character
		return make_pair(DATA, text);
		}
	else
		{
		expectData = (c == '>') || (c == '"');
		if(validChar(c))
			return make_pair(SYMBOL, string(1, c));		//found known symbol
		string text;
		do
			{
			text += c;
			is.get(c);
			}
		while(!(isspace(c) || validChar(c)));		//complete the whole identifier word
		if(!isspace(c))
			is.seekg(-1, std::ios::cur);		//move back one non alnum character
		return make_pair(IDENTIFIER, text);
		}
	}

void XMLStream::load(MyXML::Key& root)
	{
	Lexem lexem = nextLexem();
	while(lexem.first == IDENTIFIER)		//attribute name
		{
		string attName = lexem.second;
		nextLexem();		//skip '='
		nextLexem();		//skip '"'
		if((lexem = nextLexem()) != make_pair(SYMBOL, string(1, '"')))	//check if not empty value key=""
			{
			root.attribute(attName) = lexem.second;		//attribute value
			nextLexem();		//skip '"'		//closing quotation
			}
		lexem = nextLexem();		//read next lexem - should be identifier, '>' or '/'
		}
	if(lexem == make_pair(SYMBOL, string(1, '/')))
		{		//found '/': key like: <key att="val"... />
		nextLexem();		//skip '>'
		return;
		}
	lexem = nextLexem();		//read lexem after closing brace '>'
	if(lexem.first == SYMBOL)		//found some data: <key>data1 data2...
		{
		lexem = nextLexem();
		while(lexem != make_pair(SYMBOL, string(1, '/')))		//continue until find "</"
			{		//load all nested keys recursively
			load(root.insert(lexem.second));
			nextLexem();		//skip '<'		//beginning of the next key
			lexem = nextLexem();
			}
		}
	else
		{
		root = lexem.second;		//save data
		nextLexem();		//skip '<'
		nextLexem();		//skip '/'
		}
	nextLexem();		//skip identifier
	nextLexem();		//skip '>'
	}

//----------------------------------------------------------------------------

std::ostream& MyXML::operator<<(std::ostream& os, const MyXML::Key& root)
	{
	os << "<?xml version=\"1.0\" ?>" << endl;
	os << '<' << root.value_ << '>';
	root.streamAllSubKeys(os);
	os << "</" << root.value_ << '>' << endl;
	return os;
	}

std::istream& MyXML::operator>>(std::istream& is, MyXML::Key& root)
	{
	XMLStream stream(is);		//checks automatically XML header
	stream.nextLexem();		//skip '<'
	root = stream.nextLexem().second;		//save the topmost key name
	stream.nextLexem();		//skip '>'
	stream.nextLexem();		//skip '<'
	XMLStream::Lexem lexem = stream.nextLexem();
	while(lexem != make_pair(XMLStream::SYMBOL, string(1, '/')))		//continue until find "</"
		{		//load all almost top keys
		stream.load(root.insert(lexem.second));
		stream.nextLexem();		//skip '<'		//beginning of the next key
		lexem = stream.nextLexem();
		}
	stream.nextLexem();		//skip identifier
	stream.nextLexem();		//skip '>'
	return is;;
	}

//----------------------------------------------------------------------------
