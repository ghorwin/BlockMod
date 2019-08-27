#include "BM_ConnectorSegmentItem.h"

#include <QApplication>

namespace BLOCKMOD {

ConnectorSegmentItem::ConnectorSegmentItem(Connector * connector) :
	m_connector(connector),
	m_segmentIdx(-1)
{
	setAcceptHoverEvents(true);
}


void ConnectorSegmentItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
	QGraphicsItem::hoverEnterEvent(event);
	if (line().dx() == 0.0)
		QApplication::setOverrideCursor(Qt::SplitHCursor);
	else
		QApplication::setOverrideCursor(Qt::SplitVCursor);
}


void ConnectorSegmentItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
	QGraphicsItem::hoverLeaveEvent(event);
	QApplication::setOverrideCursor(Qt::ArrowCursor);
}


} // namespace BLOCKMOD

