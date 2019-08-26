#include "BM_Block.h"

#include <QXmlStreamWriter>
#include <QStringList>
#include <QDebug>

#include "BM_XMLHelpers.h"
#include "BM_Globals.h"

namespace BLOCKMOD {

Block::Block(const QString & name) :
	m_name(name),
	m_pos(0,0)
{
}

Block::Block(const QString & name, double x, double y) :
	m_name(name),
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


QLineF Block::socketStartLine(const Socket * socket) const {
	// first determine the direction: top, left, right, bottom
	QPointF otherPoint = socket->m_pos;
	switch (socket->direction()) {
		case Socket::Left	: otherPoint += QPointF(-2*Globals::GridSpacing, 0); break;
		case Socket::Right	: otherPoint += QPointF(+2*Globals::GridSpacing, 0); break;
		case Socket::Top	: otherPoint += QPointF(0, -2*Globals::GridSpacing); break;
		case Socket::Bottom	: otherPoint += QPointF(0, +2*Globals::GridSpacing); break;
	}
	QPointF startPoint(socket->m_pos);
	// shift both points by block position
	startPoint += m_pos;
	otherPoint += m_pos;
	return QLineF(startPoint, otherPoint);
}


} // namespace BLOCKMOD



