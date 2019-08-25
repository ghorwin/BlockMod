#include "BM_Connector.h"

#include <QXmlStreamWriter>
#include <QStringList>
#include <QDebug>

#include "BM_XMLHelpers.h"

namespace BLOCKMOD {

void Connector::readXML(QXmlStreamReader & reader) {
	Q_ASSERT(reader.isStartElement());
	// read attributes of Connector element
	m_name = reader.attributes().value("name").toString();
	// read child tags
	while (!reader.atEnd() && !reader.hasError()) {
		reader.readNext();
		if (reader.isStartElement()) {
			QString ename = reader.name().toString();
			if (ename == "Source") {
				m_sourceSocket = readTextElement(reader);
			}
			else if (ename == "Target") {
				m_targetSocket = readTextElement(reader);
			}
			else if (ename == "Polygon") {
				QString pos = readTextElement(reader);
				QStringList p = pos.split(";");
				for (int i=0; i<p.count(); ++i) {
					m_points.append( decodePoint(p[i]) );
				}
			}
			else {
				// unknown element, skip it and all its child elements
				reader.raiseError(QString("Found unknown element '%1' in Connector tag.").arg(ename));
				return;
			}
		}
		else if (reader.isEndElement()) {
			QString ename = reader.name().toString();
			if (ename == "Connector")
				break;// done with XML tag
		}
	}
}


void Connector::writeXML(QXmlStreamWriter & writer) const {
	writer.writeStartElement("Connector");
	writer.writeAttribute("name", m_name);
	if (!m_sourceSocket.isEmpty())
		writer.writeTextElement("Source", m_sourceSocket);
	if (!m_targetSocket.isEmpty())
		writer.writeTextElement("Target", m_targetSocket);
	if (!m_points.isEmpty()) {
		// encode polygon
		QString poly;
		for (int i=0; i<m_points.count(); ++i) {
			poly += encodePoint(m_points[i]) + ";";
		}
		if (!poly.isEmpty())
			poly.chop(1); // remove trailing ;
		writer.writeTextElement("Polygon", poly);
	}

	writer.writeEndElement();
}

} // namespace BLOCKMOD


