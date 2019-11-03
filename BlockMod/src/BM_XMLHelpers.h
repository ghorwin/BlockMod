/*	BSD 3-Clause License

	This file is part of the BlockMod Library.

	Copyright (c) 2019, Andreas Nicolai
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	   contributors may be used to endorse or promote products derived from
	   this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef BM_XMLHelpersH
#define BM_XMLHelpersH


class QXmlStreamWriter;
class QString;

#include <QPointF>
#include <QList>
#include <QXmlStreamReader>

#include <vector>
#include <stdexcept>

namespace BLOCKMOD {

/*! Helper function for XML readers. Reads unknown XML elements recursively.
	Use like:
	\code
	while (!reader.atEnd()) {
		reader.readNext();
		// process elements and attributes
		if (reader.isStartElement()) {
			QString ename = reader.name().toString();
			if (reader.name() == "MyElement")
			{
				readMyElement(reader);
			}
			else {
				readUnknownElement(reader);
			}
		}
	}
	\endcode
*/
void readUnknownElement(QXmlStreamReader & reader);

/*! Continues to read elements until and EndElement has been read.*/
void readUntilEndElement(QXmlStreamReader & reader);

/*! Tries to read a double value attribute from the current element.
	If the conversion to the double number fails, an error is raised in the reader unless the
	argument 'optional' is passed as true.
	If the XML stream is already in error, the function simply returns.
*/
void readDoubleAttribute(QXmlStreamReader & reader, const QString & name, double & val, bool optional=false);

/*! Tries to read an integer value attribute from the current element.
	If the conversion to the integer fails, an error is raised in the reader unless the
	argument 'optional' is passed as true.
	If the XML stream is already in error, the function simply returns.
*/
void readIntAttribute(QXmlStreamReader & reader, const QString & name, int & val, bool optional=false);

/*! Tries to read a boolean (integer) value attribute from the current element.
	If the conversion to the boolean fails, an error is raised in the reader unless the
	argument 'optional' is passed as true.
	If the XML stream is already in error, the function simply returns.
*/
void readBoolAttribute(QXmlStreamReader & reader, const QString & name, bool & val, bool optional=false);

/*! Tries to read a text element written with QXmlStreamWriter::writeTextElement().
	Call this function after you've already read the opening tag of the element:

	If the XML stream is already in error, the function simply returns an empty string.
*/
QString readTextElement(QXmlStreamReader & reader);

/*! Tries to read an double value tag with "name" attribute from the current element.
	If the conversion to the double fails, an error is raised in the reader.
	If the XML stream is already in error, the function simply returns.
*/
void readNamedDouble(QXmlStreamReader & reader, QString & name, double & val);

/*! Tries to read an integer value tag with "name" attribute from the current element.
	If the conversion to the integer fails, an error is raised in the reader.
	If the XML stream is already in error, the function simply returns.
*/
void readNamedInteger(QXmlStreamReader & reader, QString & name, int & val);

/*! Tries to read a string/text element with "name" attribute from the current element.
	If the XML stream is already in error, the function simply returns.
*/
void readNamedString(QXmlStreamReader & reader, QString & name, QString & val);

// templated function that works with all types of lists
template <typename T>
void readList(QXmlStreamReader & reader, QList<T> & typeList) {
	// then read all the subsections
	int count = 0;
	while (!reader.atEnd() && !reader.hasError()) {
		reader.readNext();
		// process elements and attributes
		if (reader.isStartElement()) {
			++count;
			// start reading the type
			T tmp;
			tmp.readXML(reader);
			if (reader.hasError())
				break;
			typeList.append(tmp);
		}
		else if (reader.isEndElement()) {
			break; // done with type list
		}
	}
}

// templated function that works with all types of lists
template <typename T>
void readList(QXmlStreamReader & reader, std::list<T> & typeList) {
	// then read all the subsections
	int count = 0;
	while (!reader.atEnd() && !reader.hasError()) {
		reader.readNext();
		// process elements and attributes
		if (reader.isStartElement()) {
			++count;
			// start reading the type
			T tmp;
			tmp.readXML(reader);
			if (reader.hasError())
				break;
			typeList.push_back(tmp);
		}
		else if (reader.isEndElement()) {
			break; // done with type list
		}
	}
}

/*! Encodes a point representation. */
QString encodePoint(const QPointF & p);

/*! Decodes a point representation. */
QPointF decodePoint(const QString & pStr);

} // namespace BLOCKMOD

#endif // BM_XMLHelpersH
