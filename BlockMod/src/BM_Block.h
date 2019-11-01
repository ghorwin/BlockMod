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
	Block() : m_connectionHelperBlock(false) {}

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

	/*! Utility function, that determines a free position for inserting a slot.
		Inlet sockets are inserted first left (top to bottom), then on top (left to right).
		Existing socket coordinates are first converted into grid lines.
	*/
	void findSocketInsertPosition(bool inletSocket, int & x, int & y) const;

	/*! Finds unused socket locations, i.e. grid indexes where no socket is placed.
		Assumes that sockets are aligned to grid, and uses rounded detection to adjust
		socket coordinates to grid lines.
		If a socket has an x-coordinate of 0, it is assumed to be on the left side, otherwise it
		will be on the right side.
		If a socket has a y-coordinate of 0, it is assumed to be on the top side, otherwise on
		the bottom.
		The vectors have the size of grid lines of the current block. If you resize the block, you
		need to call this function again.
	*/
	void unusedSocketSpots(QList<int> & leftSockets, QList<int> & topSockets, QList<int> & rightSockets, QList<int> & bottomSockets);

	/*! Utility function that adjust the block object and updates its sockets such, that they correspond to the
		provided inlet and outlet socket lists.
		- all sockets that are not in the lists inletSockets and outletSockets are removed
		- new sockets (in the lists), that are not yet in the block are created
		- newly created sockets are positioned in free 'slots' for sockets (inlets left/top, outlets right, bottom)
		- if not enough socket slots are available, the sockets are positioned in the last slot (thus overlaying each other)

		\param inletSockets Variable names for inlet variables (without blockname prefix)
		\param outletSockets Variable names for output variables (without blockname prefix)
	*/
	void autoUpdateSockets(const QStringList & inletSockets, const QStringList & outletSockets);

	/*! Returns a list of socket pointers with either inlet or outlet sockets. */
	QList<const Socket*>	filterSockets(bool inletSocket) const;

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

	/*! If true, this block is only a virtual block with a single socket, that is invisible (not painted)
		and only exists, until the connected has been attached to a socket of another block.
	*/
	bool						m_connectionHelperBlock;
};

} // namespace BLOCKMOD


#endif // BM_BlockH
