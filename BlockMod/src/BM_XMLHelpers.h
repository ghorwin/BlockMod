#ifndef BM_XMLHelpersH
#define BM_XMLHelpersH


class QXmlStreamWriter;
class QString;

#include <QPointF>
#include <QList>
#include <QXmlStreamReader>
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

/*! Encodes a point representation. */
QString encodePoint(const QPointF & p);

/*! Decodes a point representation. */
QPointF decodePoint(const QString & pStr);

} // namespace BLOCKMOD

#endif // BM_XMLHelpersH
