//----------------------------------------------------------------------------

///@file
///MyXML library header file.
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
///@date Jul 2005-Mar 2006

//----------------------------------------------------------------------------

#ifndef MYXML_H
#define MYXML_H

//----------------------------------------------------------------------------

#include <stdexcept>
#include <iostream>
#include <map>
#include <boost/lexical_cast.hpp>

//----------------------------------------------------------------------------

///MyXML library for reading, processing and saving data in XML format.
namespace MyXML
	{

//----------------------------------------------------------------------------

	///Exception class for MyXML routines.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This exception is thrown when you're trying to violate some XML conditions or after erros in
	///input/output.
	class Exception: public std::runtime_error
		{
		public:
			///Constructor.
			///Passes message into base class runtime_error.
			///@param exMsg XML error message which will be available using runtime_error::what().
			Exception(const std::string& exMsg): runtime_error(exMsg)	{}
		};

//----------------------------------------------------------------------------

	class Key;

	///Multimap object to store the subkeys.
	///First element of multimap (key) is the string containing the key name. Second one stores
	///the key itself.
	///@sa Key class.
	typedef std::multimap<std::string, Key> KeysMap;
	///Iterator in keys multimap.
	///Allows to iterate throught the keys container.
	///@sa Key class.
	typedef KeysMap::iterator KeyIterator;
	///Iterator in keys multimap.
	///Allows to iterate throught the keys container without allowing to change the keys.
	///@sa Key class.
	typedef KeysMap::const_iterator KeyConstIterator;
	///Pair of keys container iterators.
	///Used to store the pair of two iterators within the keys container.
	///@sa Key class.
	///@sa keys(const std::string& keyName)
	typedef std::pair<KeyIterator, KeyIterator> KeysRange;
	///Pair of keys container iterators.
	///Used to store the pair of two const iterators within the keys container.
	///@sa Key class.
	///@sa keys(const std::string& keyName) const
	typedef std::pair<KeyConstIterator, KeyConstIterator> KeysConstRange;

	///Single XML key including all sub keys.
	///XML key is all the data between tags tag and /tag.
	///@par Definitions:
	///This is a typical XML key:
	///@verbatim
	/// <data>
	/// 	<key1 attribute1"Attribute 1 value" attribute2="Attribute 2 value">
	/// 		<subKey>Sub Key value</subKey>
	/// 	</key1>
	/// </data>
	///@endverbatim
	///Assume that Key object corresponds to key1 key. This key has 2 attributes attribute1 and
	///attribute2. It has one sub key called subKey. It doesn't have a value (key cannot have
	///simultaneously subkeys and value). But the nested key subKey has a value "Sub Key value".
	///If this was the content of XML file (proceeded with a XML header), data would be called
	///the topmost key.
	///You'll learn how to read, change and clear all those part of the key.
	///@par Example:
	///To have a basic felling how to use the class, consider the following example:
	///@code
	/// #include "MyXML/myxml.h"
	/// #include <iostream>
	/// using namespace std;
	///
	/// int main()
	/// 	{
	/// 	MyXML::Key key;
	/// 	key = "data";
	/// 	key["key1"] = "Key 1 value";
	/// 	key["key2"].attribute("attr2") = "Attribute 2 value";
	/// 	key["key3"]["subKey3-1"] = "Sub Key 3-1 value";
	/// 	key["key3"]["subKey3-2"].attribute("attr3-2") = "Attribute 3-2 value";
	/// 	key["key3"]["subKey3-2"] = "Sub Key 3-2 value";
	/// 	for(int i = 1; i <= 4; ++i)
	/// 		{
	/// 		MyXML::Key& newKey = key["key4"].insert("subKey");
	/// 		newKey.attribute("attr4") = lexical_cast<std::string>(i);
	/// 		newKey = i * 1000;
	/// 		}
	/// 	cout << key;
	/// 	}
	///@endcode
	///This code will produce the output below:
	///@verbatim
	/// <?xml version="1.0" ?>
	/// <data>
	/// 	<key1>Key 1 value</key1>
	/// 	<key2 attr2="Attribute 2 value" />
	/// 	<key3>
	/// 		<subKey3-1>Sub Key 3-1 value</subKey3-1>
	/// 		<subKey3-2 attr3-2="Attribute 3-2 value">Sub Key 3-2 value</subKey3-2>
	/// 	</key3>
	/// 	<key4>
	/// 		<subKey attr4="1">1000</subKey>
	/// 		<subKey attr4="2">2000</subKey>
	/// 		<subKey attr4="3">3000</subKey>
	/// 		<subKey attr4="4">4000</subKey>
	/// 	</key4>
	/// </data>
	///@endverbatim
	class Key
		{
		private:
			///The character to use when indenting tags.
			///Nested tags are put in single line and indented using this character.
			///@sa toStream()
			static const char INDENT_CHAR = '\t';
			///Value of a key.
			///@sa value() to see how to get and change this information.
			std::string value_;
			///List of all key attributes and its values.
			///First element of a map is the attribute name and the second one stores its value.
			///@par Meaning in topmost key.
			///The value has a special meaning in topmost key. Because topmost key cannot have a value,
			///it is used as the topmost key name.
			///@note This variable is ignored in topmost key.
			///@sa attribute() to see how to get and change this information.
			std::map<std::string, std::string> attributes;
			///All keys nested in the key.
			///First element in multimap is the key name and the second one is a Key object itself.
			///Please not that the nested keys name are stored in this multimap, not in the Key object.
			///This means when you have a particular Key object, you'll have access to its attributes,
			///value and all sub keys, but the key name is stored higher in the hierarchy.
			///@sa operator[]
			///@sa keys()
			KeysMap keys_;
			///Puts the specified key in given output stream.
			///Prints the keyName value, all the key attributes and then calls
			///streamAllSubKeys() to print all the nested keys.
			///@param os Output stream to store the key information. Typically this is a file stream.
			///@param keyName Name of the key. As mentioned in keys_ description, this information isn't
			///stored in the key, it must be given explicitly.
			///@param indent How many INDENT_CHAR chars to use at the beginning of each line to indent it.
			///@sa loadFromFile()
			///@sa streamAllSubKeys()
			///@sa INDENT_CHAR
			void toStream(std::ostream& os, const std::string& keyName, int indent) const;
			///Streams all keys stored in keys_ variable.
			///Called from toStream() puts all the nested keys in the specified stream.
			///@param os Output stream to store the keys information. Typically this is a file stream.
			///@param indent How many INDENT_CHAR chars to use at the beginning of each line to indent it.
			///@sa toStream()
			void streamAllSubKeys(std::ostream& os, int indent = 1) const;
		public:
			friend std::ostream& operator<<(std::ostream& os, const MyXML::Key& key);

			///Default constructor.
			///Empty default constructor is needed for some routines which call it explicitly or if you want
			///to create an empty key and fill it later.
			///@sa Key(const std::string& initValue)
			Key()	{}
			///Creates a new key and loads XML data from specified key.
			///This constructor will automatically load the XML data from a file
			///@param loadFromFileName XML file name to be loaded.
			///@sa loadFromFile()
			Key(const std::string& loadFromFileName)	{loadFromFile(loadFromFileName);}
			///Sets up new value to the key.
			///Thanks to this overloaded operator if you want to change the key value, you don't have
			///to call value() every time like this:
			///@code
			///MyXML::Key key;
			///key.value() = "abc";
			///@endcode
			///but simply assign the Key object as it were a string:
			///@code
			///MyXML::Key key;
			///key = "abc";
			///@endcode
			///@param value New key value.
			///@return Reference to a string containing value after change.
			///@sa value()
			///@sa operator=(int value)
			std::string& operator=(const std::string& value)	{return value_ = value;}
			///Sets up an integer value to the key.
			///This is similar to the operator=(const std::string& value), but it simplifies assigning
			///numeric data to the key - which is very common.
			///@par Example:
			///@code
			///MyXML::Key key1, key2;
			///key1 = "abc";
			///key2 = 123;		//(1)
			///@endcode
			///(1) uses this overloaded operator. The key2.value() would now be "123".
			///@param value New key integer value, which will be automatically converted into string.
			///@return Reference to a string containing value after conversion change.
			///@sa value()
			///@sa operator=(const std::string& value)
			std::string& operator=(int value)	{return value_ = boost::lexical_cast<std::string>(value);}
			///Automatically converts Key object into the string.
			///The string represents the Key::value_. This may really simplify reading the key's value.
			///Using value() method you would say:
			///@code
			/// MyXML::Key key;
			/// key = "Key value";
			/// string s = key.value();
			/// cout << key.value() << endl
			///@endcode
			///But using this operator its much simplier:
			///@code
			/// MyXML::Key key("Key value");
			/// string s = key();
			/// cout << static_cast<string>(key) << endl
			///@endcode
			///As you see you must be carefull with overloaded operator<<() for streams as it have both
			///version for std::string objects an Key object and the one for Key object has bigger
			///bigger priority in this case so the explicit cast must be used.
			///@return Const reference to a string containing key's value.
			///@note Use non-const version of this operator for writing.
			///@sa value()
			operator const std::string&() const	{return value_;}
			///Automatically converts Key object into the string.
			///The string represents the Key::value_. This may really simplify reading and writing
			///the key's value.
			///@return Reference to a string containing key's value.
			///@sa value()
			operator std::string&()	{return value_;}
			///Gives access to value_ variable.
			///Allows to read and change the keys value. I recommend to use the overloaded conversion
			///operator and operator=() although this may sometimes produce ambiguities.
			///@return Reference to a string containing key's value.
			///@sa operator=() and std::string&() for examples
			///@sa value_
			///@sa value() const
			std::string& value()	{return value_;}
			///Gives read only access to key value.
			///Allows only to read the keys value when the key is const.
			///@return Const reference to a string containing key's value, which makes impossible to
			///change it.
			///@sa value()
			const std::string& value() const	{return value_;}
			///Gives access to the key attributes.
			///Returns the value of an attribute of a specified name, allowing to read and change it.
			///@par Example:
			///@code
			/// MyXML::Key key;
			/// key = "data";
			/// MyXML::Key& subKey = key["key1"];
			/// subKey.attribute("att1") = "First value";
			/// subKey.attribute("att2") = "Second value";
			/// subKey.attribute("att3") = "Third value";
			/// string s = subKey.attribute("att3");
			/// cout << key << endl;
			/// cout << s << endl;
			///@endcode
			///And the output:
			///@verbatim
			/// <?xml version="1.0" ?>
			/// <data>
			/// 	<key1 att1="First value" att2="Second value" att3="Third value" />
			/// </data>
			///
			/// Third value
			///@endverbatim
			///@param attName Attribute name to read.
			///@return Reference to a string containing a value of a specified attribute. If the
			///attribute does not exist, it will return an empty string creating a new empty attribute.
			///@sa attributes
			///@sa attribute(const std::string& attName) const
			std::string& attribute(const std::string& attName)	{return attributes[attName];}
			///Gives access to the key attributes.
			///Returns the value of an attribute of a specified name, allowing only to read it. This
			///overloaded version of attribute() method works with const keys as well.
			///@param attName Attribute name to read.
			///@return Reference to a string containing a value of a specified attribute. If the
			///attribute does not exist, it will differently to non-const version throw an exception, as
			///it can't add an empty attribute to const object.
			///@throws Exception
			///@sa attributes
			///@sa attribute(const std::string& attName)
			const std::string& attribute(const std::string& attName) const;
			///Gives access to a specified sub key.
			///Allows to get the sub key using array-style square brackets.
			///@note If there are more than one key with a specified name, the exception will be thrown
			///because the key name is ambiguos. Consider the following example:
			///@code
			/// MyXML::Key key;
			/// key["key1"] = "Key 1 value";		//OK, one key named key1
			/// key.insert("key2");
			/// key.insert("key2");
			/// cout << key["key2"].value();		//WRONG: more than one key with name key2 - exception!!!
			///@endcode
			///There is another feature - in the second line we use key1 without earilier adding it.
			///That's right - if the key with a given name does not exist, an empty key will be added
			///automatically. But if you want to add more than one key with same name, use insert()
			///instead.
			///@par
			///Another example shows how to use this operator to create quasi N-dimensional arrays from
			///XML data:
			///@code
			/// MyXML::Key key;
			/// key = "data";
			/// key["key"]["nestedKey"]["evenMoreNestedKey1"].attribute("nest") = "3 (a)";
			/// key["key"]["nestedKey"]["evenMoreNestedKey2"].attribute("nest") = "3 (b)";
			/// cout << key;
			///@endcode
			///This will produce following XML hierarchy:
			///@verbatim
			/// <?xml version="1.0" ?>
			/// <data>
			/// 	<key>
			/// 		<nestedKey>
			/// 			<evenMoreNestedKey1 nest="3 (a)" />
			/// 			<evenMoreNestedKey2 nest="3 (b)" />
			/// 		</nestedKey>
			/// 	</key>
			/// </data>
			///@endverbatim
			///@param keyName Name of a key which you want to access. If he key does not exist, it will
			///be created. If there are more than one key with this name, exception will be thrown.
			///@return Reference to a key with the specified name.
			///@throw Exception
			///@sa operator[](const std::string& keyName) const
			///@sa insert()
			Key& operator[](const std::string& keyName);
			///Gives access to a specified sub key.
			///Allows to get the sub key using array-style square brackets. Because it operates on
			///const objects, it can't add new sub keys - so calling the method with unexisting key name
			///will end up with exception thrown.
			///@param keyName Name of a key which you want to access. If he key does not exist or there
			///are more than one key with this name, exception will be thrown.
			///@return Reference to a key with the specified name.
			///@return Const reference to a key with the specified name.
			///@throw Exception
			///@sa operator[](const std::string& keyName) for examples
			///@sa insert()
			const Key& operator[](const std::string& keyName) const;
			///Inserts a new sub key with the specified name.
			///Creates a new empty key with a specified name. Although for creating keys with unique
			///names calling operator[]()is much simplier, it can't solve the problem of adding multiple
			///keys with the same name.
			///@par Example:
			///This examples shows how to add and read keys with the same names.
			///@code
			/// MyXML::Key key;
			/// key = "data";
			/// for(int x = 1; x <= 20; ++x)
			/// 	{
			/// 	MyXML::Key& item = key.insert("item");
			/// 	item.attribute("id") = lexical_cast<std::string>(x);
			/// 	item = "Item data number: " + lexical_cast<std::string>(x);
			/// 	}
			/// MyXML::KeysRange range = key.keys("item");
			/// for(MyXML::KeyIterator i = range.first; i != range.second; ++i)
			/// 	cout << i->second.attribute("id") << ":\t" << i->second.value() << endl;
			///@endcode
			///@param newKeyName Name of a key you want to create. Might be the same as the one which
			///already exists.
			///@return Reference to a newly created key.
			///@sa operator[]()
			///@sa keys()
			Key& insert(const std::string& newKeyName)
				{return keys_.insert(make_pair(newKeyName, Key()))->second;}
			///Removes specified sub keys.
			///@param removeKeyName Name of key(s) which you want to remove.
			///@return Number of keys which were removed. Of course 0 means that no keys with the
			///given name were found and removed.
			///@sa insert()
			///@sa clear()
			///@sa count()
			int remove(const std::string& removeKeyName)	{return keys_.erase(removeKeyName);}
			///Clears the key.
			///Clearing means removing the value, all sub keys and attributes. The key becomes empty
			///just as it was after creating it.
			///@sa remove()
			///@sa insert()
			///@sa count
			void clear();
			///Returns all sub keys with specified name.
			///You cannot use operator[]() to read the key if there are more than one key with the
			///specified name. You must instead call keys() method. It returns a pair of iterators
			///which limits all the keys with the specified key name.
			///@par Example:
			///See @ref insert() for complete keys() example.
			///@param keyName Name of key(s) which you want to read.
			///@return Pair of iterators limiting all the keys with the specified name. First element
			///is the first key with a specified name, second is the key <b>after</b> the last key with
			///a specified name.
			///@sa operator[]()
			///@sa keys(const std::string& keyName)
			KeysRange keys(const std::string& keyName)	{return keys_.equal_range(keyName);}
			///Returns all sub keys with specified name.
			///This overloaded method of keys() operates on const Key objects and it returns
			///pair of const iterators.
			///@par Example:
			///See @ref insert() for complete keys() example.
			///@param keyName Name of key(s) which you want to read.
			///@return Pair of const iterators limiting all the keys with the specified name. First element
			///is the first key with a specified name, second is the key <b>after</b> the last key with
			///a specified name.
			///@sa operator[]()
			///@sa keys(const std::string& keyName) const
			///@sa count()
			KeysConstRange keys(const std::string& keyName) const	{return keys_.equal_range(keyName);}
			///Returns the count of all sub keys with a specified name.
			///@param keyName name of sub keys to count.
			///@return Number of sub keys with the specified name. If no keys with that name exist,
			///function returns 0.
			///@sa insert()
			///@sa keys()
			///@sa count()
			int count(const std::string& keyName) const	{return keys_.count(keyName);}
			///Save the whole key to a specified XML file.
			///Creates file with a specified name (if it's not possible throws an exception) and
			///puts whole key (including sub keys) data proceeded with XML header into it using
			///overloaded operator<<(std::ostream& os, const MyXML::Key& root).
			///@param fileName Name of file to save the XML data
			///@throw Exception
			///@sa operator<<(std::ostream& os, const MyXML::Key& root)
			///@sa loadFromFile()
			void saveToFile(const std::string& fileName) const;
			///Loads XML file into key.
			///Opens file with a specified name (if it's not possible throws an exception) and
			///read all the XML data (including sub keys) into key using overloaded
			///operator>>(std::ostream& os, const MyXML::Key& root).
			///@param fileName Name of file to load the XML data
			///@throw Exception
			///@warning Loading function does not perform any syntax check, see
			///operator>>(std::ostream& os, const MyXML::Key& root) for more details.
			///@sa operator>>(std::ostream& os, const MyXML::Key& root)
			///@sa saveToFile()
			void loadFromFile(const std::string& fileName);
		};		//class Key

//----------------------------------------------------------------------------

	///Overloaded operator<<() for Key object output.
	///Used mostly to save XML Ket into file or print it onto the screen while testing.
	///@sa Key::saveToFile()
	std::ostream& operator<<(std::ostream& os, const MyXML::Key& root);

	///Overloaded operator<<() for Key object input.
	///Used mostly to load XML data from file into Key object.
	///@warning While testing the class I've decided to throw away all XML syntax checking sacrificing
	///safety for speed. If you're using only proper XML files (i.e. made using the library with
	///correct key names and values) everything will be OK. But if you read corrupted XML file
	///or using features which are not available in my library, the <b>library behaviour is
	///undefined</b>.
	///@sa Key::saveToFile()
	std::istream& operator>>(std::istream& is, MyXML::Key& root);

//----------------------------------------------------------------------------

	///Reading key value giving the default one.
	///This tool simplifies reading some XML data where the specified key does not exist. Normally the
	///key method value() would return an empty string. Using this function you might give any default
	///value you want (with any type you want) and if the key does not exist, the function will
	///return this default value. Additionally the key will be set to the default value too.<br>
	///Of course if the key exists, the will just return its value.
	///@param key XML key which value we want to read.
	///@param defaults Default value of a key (must be ability to convert it to and from a string).
	///@return Key value or default value if key doesn't have any.
	///@sa Key::value()
	template<typename T>
	const T readKeyDef(MyXML::Key& key, const T& defaults)
		{
		if(key.value().empty())
			{
			key = boost::lexical_cast<std::string>(defaults);
			return defaults;
			}
		else
			return boost::lexical_cast<T, std::string>(key);
		}

	///Reading key attribute value giving the default one.
	///This tool simplifies reading some XML data where the specified key attribute does not exist.
	///Normally the key method attribute() would return an empty string. Using this function you
	///might give any default attribute value you want (with any type you want) and if the key
	///attribute does not exist, the function will return this default value. Additionally the key
	///attribute will be set to the default value too.<br>
	///Of course if the key attribute exists, the will just return its value.
	///@param key XML key which attribute's value we want to read.
	///@param attrName Attribute name we want to read.
	///@param defaults Default value of an attribute  (must be ability to convert it to and from
	///a string).
	///@return Attribute value or default value if key attribute doesn't have any.
	///@sa Key::attribute()
	template<typename T>
	const T readAttrDef(MyXML::Key& key, const std::string& attrName, const T& defaults)
		{
		std::string& attValue = key.attribute(attrName);
		if(attValue.empty())
			{
			attValue = boost::lexical_cast<std::string>(defaults);
			return defaults;
			}
		else
			return boost::lexical_cast<T>(attValue);
		}

//----------------------------------------------------------------------------

	}		//namespace MyXML

//----------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------
