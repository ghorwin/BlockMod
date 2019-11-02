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

#include "BM_Network.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QSet>
#include <QDebug>

#include <stdexcept>
#include <iostream>
#include <cmath>

#include "BM_Block.h"
#include "BM_Connector.h"
#include "BM_XMLHelpers.h"
#include "BM_Globals.h"

namespace BLOCKMOD {

Network::Network()
{
}


void Network::swap(Network & other) {
	other.m_blocks.swap(m_blocks);
	other.m_connectors.swap(m_connectors);
}


void Network::readXML(const QString & fname) {

	QFile xmlFile(fname);
	if (!xmlFile.open(QIODevice::ReadOnly | QFile::Text))
		throw std::runtime_error("Cannot read file.");

	QXmlStreamReader reader(&xmlFile);

	// we start reading the XML
	while (!reader.atEnd() && !reader.hasError()) {
		reader.readNext();
		if (reader.isStartElement()) {
			// first element tag must be the BlockMod
			QString ename = reader.name().toString();
			//QString ver = reader.attributes().value("Version").toString();
			if (ename != "BlockMod" ) {
				reader.raiseError("Expected BlockMod XML tag.");
				break;
			}

			// keep reading until next start element is either Blocks, Sockets or Connectors
			while (!reader.atEnd() && !reader.hasError()) {
				reader.readNext();
				if (reader.isStartElement()) {
					QString sectionName = reader.name().toString();
					if (sectionName == "Blocks")
						readList(reader, m_blocks);
					else if (sectionName == "Connectors")
						readList(reader, m_connectors);
					else {
						reader.raiseError( QString("Unknown tag '%1'.").arg(sectionName));
						break;
					}
				}
				else if (reader.isEndElement()) {
					QString sectionName = reader.name().toString();
					Q_ASSERT(sectionName == "BlockMod");
				}
			}
		}
	}

	if (reader.hasError()) {
		throw std::runtime_error( reader.errorString().toStdString() );
	}
}


void Network::writeXML(const QString & fname) const {

	QFile xmlFile(fname);
	if (!xmlFile.open(QIODevice::WriteOnly | QFile::Truncate | QFile::Text))
		throw std::runtime_error("Cannot create output file.");
	QXmlStreamWriter stream(&xmlFile);
	stream.setAutoFormatting(true);
	stream.setAutoFormattingIndent(-1);
	stream.writeStartDocument();

	stream.writeStartElement("BlockMod");

	if (!m_blocks.empty()) {
		stream.writeComment("Blocks");
		stream.writeStartElement("Blocks");
		for (const Block & b : m_blocks)
			b.writeXML(stream);

		stream.writeEndElement(); // Blocks
	}

	if (!m_connectors.empty()) {
		stream.writeComment("Connectors");
		stream.writeStartElement("Connectors");
		for (const Connector & c : m_connectors)
			c.writeXML(stream);

		stream.writeEndElement(); // Connectors
	}


	stream.writeEndElement(); // BlockMod

	stream.writeEndDocument();
}



void Network::checkNames() const {
	QSet<QString> blockNames;
	for (const Block & b : m_blocks) {
		QString bName = b.m_name;
		if (bName.indexOf(".") != -1)
			throw std::runtime_error("Invalid Block ID name '"+bName.toStdString()+"'");
		if (blockNames.contains(bName))
			throw std::runtime_error("Duplicate Block ID name '"+bName.toStdString()+"'");
		blockNames.insert(bName);
//		qDebug() << bName;
		// now all sockets
		QSet<QString> socketNames;
		for (const Socket & s : b.m_sockets) {
			QString sName = s.m_name;
			// check if such a name already exists
			if (socketNames.contains(sName))
				throw std::runtime_error("Duplicate Socket ID name '"+sName.toStdString()+"' within block '"+bName.toStdString()+"'");
			socketNames.insert(sName);
//			qDebug() << sName;
		}
	}
	// check all connections for valid socket names
	QSet<QString> connectedSockets;
	for (const Connector & con : m_connectors) {
		// first check, that indeed the source/target connectors are valid
		const Block * b1, * b2;
		const Socket * s1, * s2;
		try {
			lookupBlockAndSocket(con.m_sourceSocket, b1, s1);
		} catch (...) {
			throw std::runtime_error("Invalid source socket identifyer '"+con.m_sourceSocket.toStdString()+"'.");
		}
		try {
			lookupBlockAndSocket(con.m_targetSocket, b2, s2);
		} catch (...) {
			throw std::runtime_error("Invalid target socket identifyer '"+con.m_targetSocket.toStdString()+"'.");
		}
		if (s1->m_inlet)
			throw std::runtime_error("Invalid source socket '"+con.m_sourceSocket.toStdString()+"'(must be an outlet socket).");
		if (!s2->m_inlet)
			throw std::runtime_error("Invalid target socket '"+con.m_targetSocket.toStdString()+"' (must be an inlet socket).");
		if (connectedSockets.contains(con.m_targetSocket))
			throw std::runtime_error("Target socket '"+con.m_targetSocket.toStdString()+"' connected twice!");
		connectedSockets.insert(con.m_targetSocket);
	}
}


void Network::adjustConnectors() {
	for (Connector & c : m_connectors) {
		try {
			adjustConnector(c);
		}
		catch (std::runtime_error & e) {
			std::cerr << "Error adjusting connector " << c.m_name.toStdString() << std::endl;
			throw e;
		}
	}
}


void Network::adjustConnector(Connector & con) {
	// split socket name into block and socket
	const Socket * socket;
	const Block * block;
	lookupBlockAndSocket(con.m_sourceSocket, block, socket);
	// get start coordinates: first point is the socket's center, second point is the connection point outside the socket
	QLineF startLine = block->socketStartLine(socket);
	lookupBlockAndSocket(con.m_targetSocket, block, socket);
	// get start coordinates: first point is the socket's center, second point is the connection point outside the socket
	QLineF endLine = block->socketStartLine(socket);

	// compute dx and dy between connection points
	double dx = endLine.p2().x() - startLine.p2().x();
	double dy = endLine.p2().y() - startLine.p2().y();

	// now subtract the distance already covered by existing segments
	for (int i=0;i<con.m_segments.count(); ++i) {
		if (con.m_segments[i].m_direction == Qt::Horizontal)
			dx -= con.m_segments[i].m_offset;
		else
			dy -= con.m_segments[i].m_offset;
	}

	// remaining distance must be distributed to segments
	if (!Globals::nearZero(dy)) {
		// now search for first connector segment that is vertical
		int i;
		for (i=0;i<con.m_segments.count(); ++i) {
			if (con.m_segments[i].m_direction == Qt::Vertical) {
				con.m_segments[i].m_offset += dy;
				break;
			}
		}
		if (i == con.m_segments.count()) {
			// add a new segment with proper size
			Connector::Segment s;
			s.m_direction = Qt::Vertical;
			s.m_offset = dy;
			con.m_segments.append(s);
		}
	}
	if (!Globals::nearZero(dx)) {
		// now search for first connector segment that is horizontal
		int i;
		for (i=0;i<con.m_segments.count(); ++i) {
			if (con.m_segments[i].m_direction == Qt::Horizontal) {
				con.m_segments[i].m_offset += dx;
				break;
			}
		}
		if (i == con.m_segments.count()) {
			// add a new segment with proper size
			Connector::Segment s;
			s.m_direction = Qt::Horizontal;
			s.m_offset = dx;
			con.m_segments.append(s);
		}
	}
}


void Network::lookupBlockAndSocket(const QString & flatName, const Block *& block, const Socket * &socket) const {
	QString blockName, socketName;
	splitFlatName(flatName, blockName, socketName);
	// search block by name
	auto blockIt = std::find_if(m_blocks.begin(), m_blocks.end(),
								[&] (const Block& b) { return b.m_name == blockName; } );
	if (blockIt == m_blocks.end())
		throw std::runtime_error("Invalid flat name.");

	const Block & b = *blockIt;
	block = &b;

	// search socket by name
	auto socketIt = std::find_if(b.m_sockets.constBegin(), b.m_sockets.constEnd(),
								[&] (const Socket& s) { return s.m_name == socketName; } );
	if (socketIt == b.m_sockets.constEnd())
		throw std::runtime_error("Invalid flat name.");

	const Socket & s = *socketIt;
	socket = &s;
}


void Network::removeBlock(unsigned int blockIdx) {
	Q_ASSERT(blockIdx < static_cast<unsigned int>(m_blocks.size()));

	// get block ID name
	auto bit = m_blocks.begin(); std::advance(bit, blockIdx);
	QString bName = bit->m_name;

	// erase all connectors that refer to this block
	auto cit = m_connectors.begin();
	while (cit != m_connectors.end()) {
		QString blockName, socketName;
		splitFlatName(cit->m_sourceSocket, blockName, socketName);
		if (blockName == bName) {
			auto cit2 = cit;
			++cit2;
			m_connectors.erase(cit);
			cit = cit2;
			continue;
		}
		splitFlatName(cit->m_targetSocket, blockName, socketName);
		if (blockName == bName) {
			auto cit2 = cit;
			++cit2;
			m_connectors.erase(cit);
			cit = cit2;
			continue;
		}
		++cit;
	}
	m_blocks.erase(bit);
}


void Network::renameBlock(unsigned int blockIdx, const QString &newName) {
	auto bit = m_blocks.begin(); std::advance(bit, blockIdx);
	QString oldName = bit->m_name;
	bit->m_name = newName;

	for (Connector & c : m_connectors) {
		QString blockName, socketName;
		splitFlatName(c.m_sourceSocket, blockName, socketName);
		if (blockName == oldName) {
			c.m_sourceSocket = newName + "." + socketName;
		}
		splitFlatName(c.m_targetSocket, blockName, socketName);
		if (blockName == oldName) {
			c.m_targetSocket = newName + "." + socketName;
		}
	}
}


void Network::splitFlatName(const QString & flatVariableName, QString & blockName, QString & socketName) {
	int pos = flatVariableName.indexOf('.');
	if (pos == -1)
		throw std::runtime_error("Bad flat name, missing . character");
	blockName = flatVariableName.left(pos).trimmed();
	socketName = flatVariableName.right(flatVariableName.length() - pos - 1).trimmed();
}


} // namespace BLOCKMOD
