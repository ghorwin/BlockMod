#include "BM_Block.h"

#include <QXmlStreamWriter>
#include <QStringList>
#include <QDebug>

#include "BM_XMLHelpers.h"

namespace BLOCKMOD {

Block::Block(const QString & name) :
	Entity(name),
	m_pos(0,0)
{
}

Block::Block(const QString & name, double x, double y) :
	Entity(name),
	m_pos(x,y)
{

}

void Block::readXML(QXmlStreamReader & reader) {
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
			else if (ename == "Size") {
				QString sizeStr = readTextElement(reader);
				QPointF pos = decodePoint(sizeStr);
				m_size.setWidth( pos.x() );
				m_size.setHeight( pos.y() );
			}
			else if (ename == "Sockets") {
				readList(reader, m_sockets);
			}
			else {
				// unknown element, skip it and all its child elements
				reader.raiseError(QString("Found unknown element '%1' in Block tag.").arg(ename));
				return;
			}
		}
		else if (reader.isEndElement()) {
			QString ename = reader.name().toString();
			if (ename == "Block")
				break;// done with XML tag
		}
	}
}


void Block::writeXML(QXmlStreamWriter & writer) const {
	writer.writeStartElement("Block");
	writer.writeAttribute("name", m_name);
	writer.writeTextElement("Position", encodePoint(m_pos));
	writer.writeTextElement("Size", encodePoint(QPointF(m_size.width(),m_size.height())) );
	if (!m_sockets.isEmpty()) {
		writer.writeComment("Sockets");
		writer.writeStartElement("Sockets");
		for (int i=0; i<m_sockets.count(); ++i)
			m_sockets[i].writeXML(writer);

		writer.writeEndElement(); // Sockets
	}

	writer.writeEndElement();
}


} // namespace BLOCKMOD



