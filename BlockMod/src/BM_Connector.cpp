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
			else if (ename == "Segments") {
				readList(reader, m_segments);
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
	if (!m_segments.isEmpty()) {
		writer.writeComment("Connector segments (between start and end lines)");
		writer.writeStartElement("Segments");
		for (int i=0; i<m_segments.count(); ++i)
			m_segments[i].writeXML(writer);

		writer.writeEndElement(); // Segments
	}

	writer.writeEndElement();
}


void Connector::Segment::readXML(QXmlStreamReader & reader) {
	Q_ASSERT(reader.isStartElement());
	// read attributes of Segment element
	// read child tags
	while (!reader.atEnd() && !reader.hasError()) {
		reader.readNext();
		if (reader.isStartElement()) {
			QString ename = reader.name().toString();
			if (ename == "Orientation") {
				QString orient = readTextElement(reader);
				if (orient == "Horizontal")
					m_direction = Qt::Horizontal;
				else
					m_direction = Qt::Vertical;
			}
			else if (ename == "Offset") {
				QString offsetStr = readTextElement(reader);
				bool ok;
				m_offset = offsetStr.toDouble(&ok);
				if (!ok) {
					// unknown element, skip it and all its child elements
					reader.raiseError(QString("Invalid offset value '%1' in Segment element.").arg(offsetStr));
					return;
				}
			}
			else {
				// unknown element, skip it and all its child elements
				reader.raiseError(QString("Found unknown element '%1' in Segment element.").arg(ename));
				return;
			}
		}
		else if (reader.isEndElement()) {
			QString ename = reader.name().toString();
			if (ename == "Segment")
				break;// done with XML tag
		}
	}
}


void Connector::Segment::writeXML(QXmlStreamWriter & writer) const {
	writer.writeStartElement("Segment");
	writer.writeTextElement("Orientation", m_direction == Qt::Horizontal ? "Horizontal" : "Vertical");
	writer.writeTextElement("Offset", QString("%1").arg(m_offset));
	writer.writeEndElement();
}


} // namespace BLOCKMOD


