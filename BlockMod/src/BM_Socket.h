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

#ifndef BM_SocketH
#define BM_SocketH

#include <QList>
#include <QPointF>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace BLOCKMOD {

/*! Stores properties of a Socket.

	* type: in/out
	* orientation: north, east, west, south

	Sockets are owned by parent block and do not exist in separate socket list.
*/
class Socket {
public:
	/*! Default C'tor. */
	Socket() :
		m_pos(0,0),
		m_orientation(Qt::Horizontal),
		m_inlet(false)
	{
	}

	/*! C'tor, creating named default socket. */
	Socket(const QString & name) :
		m_name(name),
		m_pos(0,0),
		m_orientation(Qt::Horizontal),
		m_inlet(false)
	{
	}

	/*! C'tor, initializes all members. */
	Socket(const QString & name, const QPointF & pos, Qt::Orientation orientation, bool inlet) :
		m_name(name),
		m_pos(pos),
		m_orientation(orientation),
		m_inlet(inlet)
	{
	}

	/*! Reads content of the block from XML stream. */
	void readXML(QXmlStreamReader & reader);

	/*! Dumps out content of block to stream writer. */
	void writeXML(QXmlStreamWriter & writer) const;

	/*! Socket direction indicates into which direction a socket is pointing. */
	enum SocketDirection {
		Left,
		Right,
		Top,
		Bottom
	};

	/*! Determine direction of socket based on position and orientation properties. */
	SocketDirection direction() const {
		if (m_orientation == Qt::Horizontal) {
			if (m_pos.x() == 0.0)	return Left;
			else					return Right;
		} else {
			if (m_pos.y() == 0.0)	return Top;
			else					return Bottom;
		}
	}

	/*! Comparison operator to find socket by name. */
	bool operator==(const QString & s) const { return m_name == s; }

	QString			m_name;

	/*! Position (connection point) of Socket.
		Relative to parent block.
	*/
	QPointF			m_pos;

	/*! Orientation defines together with the position relative to the parent block, which
		direction the block points to and how to compute the first connector start coordinate.

		Example: m_orientation = Qt::Horizontal and m_pos.x() != 0 --> connector at the right side of block pointing to the right
	*/
	Qt::Orientation	m_orientation;

	/*! If true, painted as a socket, if false, painted as an outgoing arrow. */
	bool			m_inlet;
};

} // namespace BLOCKMOD


#endif // BM_SocketH
