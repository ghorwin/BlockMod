#include "BM_ConnectorSegmentItem.h"

#include <QApplication>
#include <QGraphicsSceneMouseEvent>

#include <cmath>

#include "BM_Globals.h"
#include "BM_SceneManager.h"
#include "BM_Connector.h"

namespace BLOCKMOD {

ConnectorSegmentItem::ConnectorSegmentItem(Connector * connector) :
	m_connector(connector),
	m_segmentIdx(-1)
{
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
	setAcceptHoverEvents(true);
}


void ConnectorSegmentItem::setLine(const QLineF &line) {
	m_lastPos = line.p1().toPoint();
	QGraphicsLineItem::setLine(line);
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


void ConnectorSegmentItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	if (event->button() == Qt::LeftButton && event->modifiers().testFlag(Qt::ControlModifier)) {
		setSelected(true);
		event->accept();
		return;
	}
	if (event->button() == Qt::LeftButton && !m_moved) {
		// TODO : signal that a block selection has been made and optionally de-select other blocks
	}
	QGraphicsLineItem::mouseReleaseEvent(event);
	m_moved = false;
}


QVariant ConnectorSegmentItem::itemChange(GraphicsItemChange change, const QVariant & value) {
	if (change == QGraphicsItem::ItemPositionChange) {
		// enable snappig
		QPointF pos = value.toPointF();
		pos.setX( std::floor(pos.x() / Globals::GridSpacing) * Globals::GridSpacing);
		pos.setY( std::floor(pos.y() / Globals::GridSpacing) * Globals::GridSpacing);
		if (m_lastPos != pos.toPoint()) {
			m_moved = true;
			m_lastPos = pos.toPoint();
			// update connector segments

			// we must distribute the dx and dy to both sides of the moved connector
			// Algorithm:
			// - process all segments to the left of the connector and distribute dx and dy
			// - if afterwards dx and dy is still != 0, check if this segment can be extended to compensate
			// - if not (different direction), insert a new line segment left of this position with the matching direction and offset

			QPointF moveDist = pos-m_lastPos;
			double dx = moveDist.x();
			double dy = moveDist.y();

			int segIdx = m_segmentIdx;
			Q_ASSERT(segIdx >= 0);
			Q_ASSERT(segIdx < m_connector->m_segments.size());
			while (segIdx > 0 && !(std::fabs(dx/Globals::GridSpacing) < 1e-6) && !(std::fabs(dy/Globals::GridSpacing) > 1e-6)) {
				// get next segment to the left
				Connector::Segment & seg = m_connector->m_segments[segIdx];
				//
			}


			// inform network to update connectors
			SceneManager * sceneManager = qobject_cast<SceneManager *>(scene());
			if (sceneManager != nullptr)
				sceneManager->connectorMoved(m_connector);
		}
		return pos;
	}
	return QGraphicsLineItem::itemChange(change, value);
}

} // namespace BLOCKMOD

