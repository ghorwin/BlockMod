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

#ifndef BM_ConnectorSegmentItemH
#define BM_ConnectorSegmentItemH

#include <QGraphicsLineItem>

namespace BLOCKMOD {

class Connector;

/*! A segment (line item) of a connection. */
class ConnectorSegmentItem : public QGraphicsLineItem {
public:
	explicit ConnectorSegmentItem(Connector * connector);

	/*! Re-implemented to initialize m_lastPos. */
	void setLine(const QLineF &line);


	/*! The connector, that this line segment belongs to. */
	Connector	*m_connector;

	/*! The index of the segment this item corresponds to.
		- -1 means start segment
		- -2 means end segment
		- 0...n corresponds to the index in m_connector->m_segments[]
	*/
	int			m_segmentIdx;

	/*! If true, the connector segment is painted in highlighted mode. */
	bool		m_isHighlighted;

protected:
	/*! Re-implemented to draw the highlighted connection. */
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	/*! Re-implemented to highlight the entire connection line (all segments) when hovered. */
	virtual void hoverEnterEvent (QGraphicsSceneHoverEvent *event) override;
	/*! Re-implemented to turn off highlighting of the entire connection line (all segments) when hovered. */
	virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent *event) override;

	/*! Re-implemented to select the segment and all other segments. */
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	/*! Re-implemented to reset the m_moved flag. */
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

	/*! Overloaded to react on item move.
		Implements the snap-to-grid functionality.
	*/
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value) override;

private:
	/*! Indicates, that the item has been moved. */
	bool		m_moved;

	/*! Caches current position of the line to detect snapped grid movement.
		\note Position of the line is not the same as p1 and p2, which are changed with setLine().
	*/
	QPoint		m_lastPos;
};

} // namespace BLOCKMOD


#endif // BM_ConnectorSegmentItemH
