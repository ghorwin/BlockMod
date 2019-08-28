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

#ifndef BM_BlockH
#define BM_BlockH

#include <QList>
#include <QString>
#include <QPointF>
#include <QSizeF>
#include <QLineF>
#include <QMap>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "BM_Socket.h"

namespace BLOCKMOD {

/*! Stores properties of a block.
	* appearance properties of block
	* position of block
	* sockets
	* custom properties in the property map
*/
class Block {
public:
	Block() {}

	Block(const QString & name);
	Block(const QString & name, double x, double y);

	/*! Reads content of the block from XML stream. */
	void readXML(QXmlStreamReader & reader);

	/*! Dumps out content of block to stream writer. */
	void writeXML(QXmlStreamWriter & writer) const;

	/*! Generate connection line between socket and point, where first connector segment starts.
		Returned coordinates are in scene-coordinates.
	*/
	QLineF socketStartLine(const Socket * socket) const;

	/*! Unique identification name of this block instance. */
	QString						m_name;

	/*! Position (top left corner) of block. */
	QPointF						m_pos;

	/*! Sockets that belong to this block. */
	QList<Socket>				m_sockets;

	/*! Size of block. */
	QSizeF						m_size;

	/*! Custom properties. */
	QMap<QString, QVariant>		m_properties;
};

} // namespace BLOCKMOD


#endif // BM_BlockH
