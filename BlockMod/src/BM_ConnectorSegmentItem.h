#ifndef BM_CONNECTORSEGMENTITEM_H
#define BM_CONNECTORSEGMENTITEM_H

#include <QGraphicsLineItem>

namespace BLOCKMOD {

class Connector;

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

protected:
	virtual void hoverEnterEvent (QGraphicsSceneHoverEvent *event) override;
	virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent *event) override;

	/*! Re-implemented to reset the m_moved flag. */
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

	/*! Overloaded to react on item move.
		Implements the snap-to-grid functionality. */
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value) override;

private:
	/*! Indicates, that the item has been moved. */
	bool		m_moved;

	/*! Caches current p1() position of the line to detect snapped grid movement. */
	QPoint		m_lastPos;
};

} // namespace BLOCKMOD


#endif // BM_CONNECTORSEGMENTITEM_H
