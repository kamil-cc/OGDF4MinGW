/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-13 17:57:11 +0100 (Di, 13 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Contains the struct declarations XmlAttributeObject, XmlTagObject
 * and the class DinoXmlParser.
 * 
 * \author Dino Ahr
 * 
 * \par License:
 * This file is part of the Open Graph Drawing Framework (OGDF).
 * Copyright (C) 2005-2007
 * 
 * \par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 or 3 as published by the Free Software Foundation
 * and appearing in the files LICENSE_GPL_v2.txt and
 * LICENSE_GPL_v3.txt included in the packaging of this file.
 *
 * \par
 * In addition, as a special exception, you have permission to link
 * this software with the libraries of the COIN-OR Osi project
 * (http://www.coin-or.org/projects/Osi.xml), all libraries required
 * by Osi, and all LP-solver libraries directly supported by the
 * COIN-OR Osi project, and distribute executables, as long as
 * you follow the requirements of the GNU General Public License
 * in regard to all of the software in the executable aside from these
 * third-party libraries.
 * 
 * \par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * \par
 * You should have received a copy of the GNU General Public 
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 * 
 * \see  http://www.gnu.org/copyleft/gpl.html
 ***************************************************************/

#ifdef _MSC_VER
#pragma once
#endif

#ifndef OGDF_DINO_XML_PARSER_H
#define OGDF_DINO_XML_PARSER_H

#include <ogdf/basic/String.h>
#include <ogdf/basic/Hashing.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/fileformats/DinoXmlScanner.h>



namespace ogdf {

	//---------------------------------------------------------
	// H a s h e d S t r i n g
	//---------------------------------------------------------

	typedef HashElement<String,int> HashedString;

	//---------------------------------------------------------
	// X m l A t t r i b u t e O b j e c t
	//---------------------------------------------------------
	/** This struct represents an attribute associated to a tag.
	 */
	struct XmlAttributeObject {
		
		/** Contains the name of the attribute, i.e.
		 *  for <A attr1="value1"> ... </A> it contains "attr1"
		 */
		HashedString *m_pAttributeName;
		
		/** Contains the value assigned to this attribute without qoutes, i.e.
		 *  for <A attr1="value1"> ... </A> it contains "value1" and not "\"value1\"".
		 */
		HashedString *m_pAttributeValue;

		/** Pointer to the next attribute and 0 if this is the only attribute. */
		XmlAttributeObject *m_pNextAttribute;

		/** Constructor */
		XmlAttributeObject(HashedString *name, HashedString *value) :
			m_pAttributeName(name),
			m_pAttributeValue(value),
			m_pNextAttribute(0)
			{};

		/** Destructor; will be performed in destroyParseTree(). */
		~XmlAttributeObject(){};

		// Overloaded new and delete operators
        OGDF_NEW_DELETE

	}; // struct XmlAttributeObject

	//---------------------------------------------------------
	// X m l T a g O b j e c t
	//---------------------------------------------------------
	/** This struct represents a node in the XML parse tree.
	 */
	struct XmlTagObject {

		/** The identifier of the tag, 
		 *i.e. for <A> the identifier is "A"
		 */
		HashedString *m_pTagName;

		/** Pointer to the first attribute; 
		 *  if there is more than one attribute these are linked by 
		 * m_pNextAttribute in struct XmlAttributeObject
		 */
		XmlAttributeObject *m_pFirstAttribute;

		/** Contains the characters inbetween the start tag and the end tag,
		 *  i.e. for <A attr1=... attr2=...> lala </A> it contains " lala "
		 */
		HashedString *m_pTagValue;

		/** Contains the pointer to the first son tag object,
		 *  i.e. for <A> <B> ... </B> <C> ... </C> </A> it contains a pointer
		 *  to the object representing B
		 *  The other children of A are reachable via m_pBrother of the first son,
		 *  i.e. the variable m_pBrother of the object representing B contains a 
		 * pointer to the object representing C
		 */
		XmlTagObject *m_pFirstSon;
			
		/** Contains the pointer to a brother tag object or 0 if this 
		 *  object is the only child
		 */
		XmlTagObject *m_pBrother;

		/** Constructor */
		XmlTagObject(HashedString *name) :
			m_pTagName(name),
			m_pFirstAttribute(0),
			m_pTagValue(0),
			m_pFirstSon(0),
			m_pBrother(0)
			{};

		/** Destructor; will be performed in destroyParseTree(). */
		~XmlTagObject(){};

		// Overloaded new and delete operators
        OGDF_NEW_DELETE

	}; // struct XmlTagObject

	//---------------------------------------------------------
	// D i n o X m l P a r s e r 
	//---------------------------------------------------------
	/** This class parses the XML input file and builds up a 
	 *  parse tree with linked elements XMLTagObject and 
	 *  XMLAttributeObject. The class DinoXmlScanner is used to
	 *  get the token for the parse process.
	 */
  class DinoXmlParser {

	  friend ostream &operator<<(ostream&, const DinoXmlParser &);

	private:

		/** Pointer to the root element of the parse tree. */
		XmlTagObject *m_pRootTag;

		/** Pointer to the scanner. */
		DinoXmlScanner *m_pScanner;

		/** Hash table for storing names of TagObjects and 
		 *  AttributeObjects in an efficient manner.
		 *  The key element is String.
		 *  The info element is int.
		 */
		Hashing<String,int> m_hashTable; 

		/** The info element of the hash table is simply an integer
		 * number which is incremented for each new element (starting at 0).
		 * The value m_hashTableInfoIndex - 1 is the last used index.
		 */
		int m_hashTableInfoIndex;

		/** Recursion depth of parse(). */
		int m_recursionDepth;

	public:
		
		/** Constructor.
		 *  Inside the constructor the scanner is generated.
		 */
		DinoXmlParser(const char *fileName);

		/** Destructor; destroys the parse tree. */
		~DinoXmlParser();

		/** Creates a new hash element and inserts it into the hash table.
		 */
		void addNewHashElement(const String &key, int info){
			OGDF_ASSERT(info >= m_hashTableInfoIndex)
			m_hashTable.fastInsert(key, info);
			m_hashTableInfoIndex = info + 1;
		}

		/** Creates the parse tree and anchors it in m_pRootTag. 
		 *  TODO: Should return a value to indicate if success.
		 */
		void createParseTree();

		/** Allows (non modifying) access to the parse tree. */
		const XmlTagObject &getRootTag() const {
			return *m_pRootTag;
		}

		/** Traverses the parseTree starting at startTag using the path
		 * description in path, which contains the infoIndices of the tags
		 * which have to be traversed.
		 * If the XmlTagObject associated to the last infoIndex in the path is 
		 * found, it is returned via targetTag and the return value is true
		 * If the XmlTagObject is not found the return value is false.
		 */
		bool traversePath(const XmlTagObject &startTag, 
						  const Array<int> &infoIndexPath,
			    		  const XmlTagObject *&targetTag) const;

		/** Searches for a specific son (identified by sonInfoIndex) 
		 *  of father.
		 *  Returns the son via the referenced pointer son.
		 *  Returns true if son is found.
		 *  Returns false otherwise, son is set to NULL.
		 */
		bool findSonXmlTagObject(const XmlTagObject &father, 
			 				     int sonInfoIndex,
								 const XmlTagObject *&son) const;

		/** Searches for a specific brother (identified by brotherInfoIndex)
		 *  of current.
		 *  Returns the brother via the referenced pointer brother.
		 *  Returns true if brother is found.
		 *  Returns false otherwise, brother is set to NULL.
		 */
		bool findBrotherXmlTagObject(const XmlTagObject &currentTag, 
									 int brotherInfoIndex,
									 const XmlTagObject *&brother) const;

		/** Searches for a specific attribute (identified by attributeInfoIndex)
		 *  of current.
		 *  Returns the attribute via the referenced pointer attribute.
		 *  Returns true if attribute is found.
		 *  Returns false otherwise, attribute is set to NULL.
		 */
		bool findXmlAttributeObject(const XmlTagObject &currentTag, 
								    int attributeInfoIndex,
								    const XmlAttributeObject *&attribute) const;
		
		/** Returns line number of the most recently read line of 
		 *  the input file.
		 */
		inline int getInputFileLineCounter() const {
			return m_pScanner->getInputFileLineCounter();
		}

		/** Prints the content of the hash table to os. */
		void printHashTable(ostream &os);

	private:

		/** Destroys the parse tree appended to root. */
		void destroyParseTree(XmlTagObject *root);

		/** Parses the token stream provided by the scanner until a complete 
		 *  XmlTagObject is identified which will be returned. 
		 *  This function is likely to be called recursively 
		 * due to the recursive structure of XML documents.
		 */
		XmlTagObject* parse();

		/** Append attributeObject to list of attributes of tagObject. */
		void appendAttributeObject(XmlTagObject *tagObject,
								   XmlAttributeObject *attributeObject);
		
		/** Appends sonTagObject to the list of sons of currentTagObject. */
		void appendSonTagObject(XmlTagObject *currentTagObject, 
								XmlTagObject *sonTagObject);

		/** Returns the hash element for the given string. 
		 *  If the key str is not contained in the table yet, it is
		 *  inserted together with a new info index and the new 
		 *  hash element is returned.
		 *  If the key str exists, the associated hash element is returned.
		 */
		HashedString *hashString(const String &str);

		/** Prints the given XmlTagObject and its children recursively.
		 *  The parameter indent is used as indentation value.
		 */
		void printXmlTagObjectTree(ostream &os, const XmlTagObject &rootObject, 
								   int indent = 0) const;

		/** Little helper that prints nOfSpaces space characters. */
		void printSpaces(ostream &os, int nOfSpaces) const;

	}; // class DinoXmlParser

	/** Output operator for DinoXmlParser. */
	ostream &operator<<(ostream &os, const DinoXmlParser &parser);

} // end namespace ogdf

#endif
