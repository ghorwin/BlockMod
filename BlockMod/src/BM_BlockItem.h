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

#ifndef BM_BlockItemH
#define BM_BlockItemH

#include <QGraphicsRectItem>

namespace BLOCKMOD {

class Block;
class SocketItem;

/*! A graphics item that represents a block. */
class BlockItem : public QGraphicsRectItem {
public:
	/*! Constructor, takes a pointer to the associated block data structure (which
		must have a lifetime longer than the graphics item.
	*/
	explicit BlockItem(Block * b);

	const Block * block() const { return m_block; }

	/*! Searches for a socket item at the given scene position and returns a pointer to it, if
		it is not yet connected.
	*/
	SocketItem * inletSocketAcceptingConnection(const QPointF & scenePos);

	/*! Returns true, if this block is invisible (call this when re-implementing the paint() function). */
	bool isInvisible() const;

	/*! Changes size of a block item (and moves socket items accordingly). */
	void resize(int newWidth, int newHeight);

protected:
	/*! This function is called from the constructor and creates child socket items.
		You can overload this function to create your own socket items.
	*/
	virtual void createSocketItems();

	/*! Re-implemented to draw the styled rectangle of the block. */
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	/*! Re-implemented to trigger the editor. */
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

	/*! Re-implemented to reset the m_moved flag. */
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

	/*! Overloaded to react on block move.
		Implements the snap-to-grid functionality, and updates attached connectors. */
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value) override;


private:
	/*! Pointer to associated block. */
	Block				*m_block;

	/*! Indicates, that the block has been moved. */
	bool				m_moved;

	/*! Our socket items, childs of this block item. */
	QList<SocketItem*>	m_socketItems;

	friend class SceneManager;
};

} // namespace BLOCKMOD


#endif // BM_BlockItemH
