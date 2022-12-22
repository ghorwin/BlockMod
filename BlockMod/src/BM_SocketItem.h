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

#ifndef BM_SocketItemH
#define BM_SocketItemH

#include <QGraphicsItem>

namespace BLOCKMOD {

class Socket;
class Block;
class BlockItem;

/*! Base class for sockets to be painted on a block item. */
class SocketItem : public QGraphicsItem {
public:
	/*! Constructor, takes a pointer to the associated socket data structure (which
		must have a lifetime longer than the graphics item.
	*/
	explicit SocketItem(BlockItem * parent, Socket * socket);

	/*! Call this function whenever the socket's geometry in the associated socket object has changed. */
	void updateSocketItem();

	QRectF boundingRect() const override;

	/*! Returns pointer to socket. */
	const Socket * socket() const { return m_socket; }

protected:
	virtual void hoverEnterEvent (QGraphicsSceneHoverEvent *event) override;
	virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent *event) override;

	/*! Re-implemented to draw the styled rectangle of the block. */
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	/*! Only works for outlet socket items, puts the scene in connection mode. */
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
	/*! The parent block that this socket belongs to. */
	const Block	*m_block;

	/*! Pointer to the socket data structure. */
	Socket	*m_socket;

	/*! The bounding rectangle of the symbol (updated whenever content of the socket changes). */
	QRectF	m_symbolRect;

	/*! Set to true, when mouse hovers over item.
		Causes different pointing operation to be used.
	*/
	bool	m_hovered;

	friend class SceneManager;
};

} // namespace BLOCKMOD


#endif // BM_SocketItemH
