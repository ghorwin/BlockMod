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

#include "BM_Block.h"

#include <QXmlStreamWriter>
#include <QStringList>
#include <QDebug>

#include "BM_XMLHelpers.h"
#include "BM_Globals.h"

namespace BLOCKMOD {

Block::Block(const QString & name) :
	m_name(name),
	m_pos(0,0),
	m_connectionHelperBlock(false)
{
}

Block::Block(const QString & name, double x, double y) :
	m_name(name),
	m_pos(x,y),
	m_connectionHelperBlock(false)
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
	// special handling for "invisible" blocks

	if (m_name == Globals::InvisibleLabel) {
		QPointF startPoint(socket->m_pos);
		startPoint += m_pos;
		return QLineF(startPoint, startPoint);
	}
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



