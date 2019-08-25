#include "BM_Socket.h"

#include <QXmlStreamWriter>
#include <QStringList>

#include "BM_XMLHelpers.h"

namespace BLOCKMOD {

Socket::Socket(const QString & name) :
	Entity(name),
	m_pos(0,0),
	m_orientation(Qt::Horizontal)
{
}


void Socket::readXML(QXmlStreamReader & reader) {
	Q_ASSERT(reader.isStartElement());
	// read attributes of Block element
	m_name = reader.attributes().value("name").toString();
	// read child tags
	while (!reader.atEnd() && !reader.hasError()) {
		reader.readNext();
		if (reader.isStartElement()) {
			QString ename = reader.name().toString();
			if (ename == "Position") {
				QString pos = readTextElement(reader);
				m_pos = decodePoint(pos);
			}
			else if (ename == "Orientation") {
				QString orient = readTextElement(reader);
				if (orient == "Horizontal")
					m_orientation = Qt::Horizontal;
				else
					m_orientation = Qt::Vertical;
			}
			else if (ename == "Inlet") {
				QString flag = readTextElement(reader);
				m_inlet = (flag == "true");
			}
			else {
				// unknown element, skip it and all its child elements
				reader.raiseError(QString("Found unknown element '%1' in Socket tag.").arg(ename));
				return;
			}
		}
		else if (reader.isEndElement()) {
			QString ename = reader.name().toString();
			if (ename == "Socket")
				break;// done with XML tag
		}
	}
}


void Socket::writeXML(QXmlStreamWriter & writer) const {
	writer.writeStartElement("Socket");
	writer.writeAttribute("name", m_name);
	writer.writeTextElement("Position", encodePoint(m_pos));
	writer.writeTextElement("Orientation", m_orientation == Qt::Horizontal ? "Horizontal" : "Vertical");
	writer.writeTextElement("Inlet", m_inlet ? "true" : "false");
	writer.writeEndElement();
}

} // namespace BLOCKMOD

