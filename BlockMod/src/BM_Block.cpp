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

#include <cmath>

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


void Block::findSocketInsertPosition(bool inletSocket, int & x, int & y) const {
	// create list of socket positions
	unsigned int rowCount = (unsigned int)std::floor(m_size.height() / (double)Globals::GridSpacing + 0.5);
	unsigned int colCount = (unsigned int)std::floor(m_size.width() / (double)Globals::GridSpacing + 0.5);

	std::vector<int> verticalSockets(rowCount-1, 0);
	std::vector<int> horizontalSockets(colCount-1, 0);

	QList<const Socket*> inletSockets = filterSockets(true);
	if (inletSocket) {
		for (const Socket* s : inletSockets) {
			// determine position of the socket, and mark slot as taken
			if (s->m_pos.y() == 0.0) {
				// located at top
				unsigned int colrowIdx = (unsigned int)(std::floor(s->m_pos.x() / (double)Globals::GridSpacing + 0.5));
				// if 0 or > colCount-1, ignore
				if (colrowIdx > 0 && colrowIdx < colCount-1)
					horizontalSockets[colrowIdx] = 1;
			}
			else {
				unsigned int rowIdx = (unsigned int)(std::floor(s->m_pos.y() / (double)Globals::GridSpacing + 0.5));
				if (rowIdx > 0 && rowIdx < rowCount-1)
					verticalSockets[rowIdx] = 1;
			}
		}
		// first look for empty slot at left side
		for (size_t i=0; i<verticalSockets.size(); ++i) {
			if (verticalSockets[i] == 0) {
				x=0;
				y=i;
				return;
			}
		}
		for (size_t i=0; i<horizontalSockets.size(); ++i) {
			if (horizontalSockets[i] == 0) {
				y=0;
				x=i;
				return;
			}
		}
		// duplicate sockets, select top-right slot
		x=colCount-1;
		y=0;
		return;
	}
}


void Block::unusedSocketSpots(QList<int> & leftSockets, QList<int> & topSockets, QList<int> & rightSockets, QList<int> & bottomSockets) {
	// create list of socket positions
	int rowCount = (int)std::floor(m_size.height() / (double)Globals::GridSpacing + 0.5);
	int colCount = (int)std::floor(m_size.width() / (double)Globals::GridSpacing + 0.5);

	leftSockets.clear();
	rightSockets.clear();
	for (int i=0; i<rowCount; ++i) {
		leftSockets.push_back(0);
		rightSockets.push_back(0);
	}

	topSockets.clear();
	bottomSockets.clear();
	for (int i=0; i<colCount; ++i) {
		topSockets.push_back(0);
		bottomSockets.push_back(0);
	}

	// now process all sockets
	for (const Socket & s : m_sockets) {
		int colIdx = (int)std::floor(s.m_pos.x() / (double)Globals::GridSpacing + 0.5);
		int rowIdx = (int)std::floor(s.m_pos.y() / (double)Globals::GridSpacing + 0.5);

		// left side?
		if ((int)s.m_pos.x() == 0) {
			if (rowIdx > 0 && rowIdx < rowCount)
				++leftSockets[rowIdx];
		}

		// right side?
		if ((int)s.m_pos.x() == (int)m_size.width()) {
			if (rowIdx > 0 && rowIdx < rowCount)
				++rightSockets[rowIdx];
		}

		// top size
		if ((int)s.m_pos.y() == 0) {
			if (colIdx > 0 && colIdx < colCount)
				++topSockets[colIdx];
		}

		// bottom size
		if ((int)s.m_pos.y() == (int)m_size.height()) {
			if (colIdx > 0 && colIdx < colCount)
				++bottomSockets[colIdx];
		}
	}
}


void Block::autoUpdateSockets(const QStringList & inletSockets, const QStringList & outletSockets) {
	// now remove no-longer existing sockets and add and position new sockets

	// first remove all sockets from block that are not in the list of inlet/outlet sockets
	// this ensures that afterwards we only have valid
	QList<BLOCKMOD::Socket> remainingSockets;
	for (const BLOCKMOD::Socket & s : m_sockets) {
		if (s.m_inlet) {
			if (inletSockets.contains(s.m_name)) {
				remainingSockets.append(s);
			}
		}
		else {
			if (outletSockets.contains(s.m_name)) {
				remainingSockets.append(s);
			}
		}
	}
	m_sockets.swap(remainingSockets);

	// get lists of free socket positions
	QList<int> leftSockets, rightSockets, topSockets, bottomSockets;

	unusedSocketSpots(leftSockets, topSockets, rightSockets, bottomSockets);

	QStringList sockets = inletSockets;
	sockets += outletSockets;
	// now create sockets for each not yet existing variable
	for (int sidx=0; sidx < sockets.count(); ++sidx) {
		const QString & s = sockets[sidx];
		// while searching for sockets we do not need to distinguish between inlet/outlet sockets,
		// because in the step before we have filtered out all sockets that did not have a maching type
		int socketIdx;
		for (socketIdx=0; socketIdx < m_sockets.count(); ++socketIdx) {
			if (m_sockets[socketIdx].m_name == s)
				break;
		}
		// already defined?
		if (socketIdx != m_sockets.count())
			continue;
		// create and position a new socket
		Socket newSocket;
		newSocket.m_name = s;
		bool inlet = sidx < inletSockets.count();
		newSocket.m_inlet = inlet;
		// now find free slot, first search on left side
		bool found = false;
		for (int i=1; i<leftSockets.count(); ++i) {
			if (inlet) {
				if (leftSockets[i] == 0) {
					// take this spot
					leftSockets[i] = 1;
					newSocket.m_pos.setX(0);
					newSocket.m_pos.setY(i*Globals::GridSpacing);
					found = true;
					break;
				}
			}
			else {
				if (rightSockets[i] == 0) {
					// take this spot
					rightSockets[i] = 1;
					newSocket.m_pos.setX(m_size.width());
					newSocket.m_pos.setY(i*Globals::GridSpacing);
					found = true;
					break;
				}
			}
		}
		// no spot left, use right side
		if (!found) {
			for (int i=0; i<topSockets.count(); ++i) {
				if (inlet) {
					if (topSockets[i] == 0) {
						// take this spot
						topSockets[i] = 1;
						newSocket.m_orientation = Qt::Vertical;
						newSocket.m_pos.setY(0);
						newSocket.m_pos.setX(i*Globals::GridSpacing);
						found = true;
						break;
					}
				}
				else {
					if (bottomSockets[i] == 0) {
						// take this spot
						bottomSockets[i] = 1;
						newSocket.m_orientation = Qt::Vertical;
						newSocket.m_pos.setY(m_size.height());
						newSocket.m_pos.setX(i*Globals::GridSpacing);
						found = true;
						break;
					}
				}
			}
		}
		// no spot left, use top-right spot
		if (!found) {
			if (inlet) {
				newSocket.m_pos.setY(0);
				newSocket.m_pos.setX(m_size.width() - Globals::GridSpacing);
			}
			else {
				newSocket.m_pos.setY(m_size.height());
				newSocket.m_pos.setX(m_size.width() - Globals::GridSpacing);
			}
		}
		// finally, add new socket
		m_sockets.append(newSocket);
	}

}


QList<const Socket*> Block::filterSockets(bool inletSocket) const {
	QList<const Socket*> socketList;
	for (const Socket & s : m_sockets) {
		if (inletSocket && s.m_inlet)
			socketList.append(&s);
		else if (!inletSocket && !s.m_inlet)
			socketList.append(&s);
	}
	return socketList;
}

} // namespace BLOCKMOD



