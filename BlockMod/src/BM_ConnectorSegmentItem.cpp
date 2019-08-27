#include "BM_ConnectorSegmentItem.h"

#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

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
	m_lastPos = pos().toPoint();
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
		// TODO : signal that a line selection has been made and optionally de-select other lines
	}
	QGraphicsLineItem::mouseReleaseEvent(event);
	m_moved = false;
}


QVariant ConnectorSegmentItem::itemChange(GraphicsItemChange change, const QVariant & value) {
	if (change == QGraphicsItem::ItemPositionChange) {
//		qDebug() << this;
		// enable snappig
		QPointF pF = value.toPointF();
		QLineF oldLine = line();
//		qDebug() << "Line position = " << pF2;
//		qDebug() << "  Last position = " << m_lastPos;
		pF.setX( std::floor(pF.x() / Globals::GridSpacing) * Globals::GridSpacing);
		pF.setY( std::floor(pF.y() / Globals::GridSpacing) * Globals::GridSpacing);
		QPoint p = pF.toPoint();
		if (m_lastPos != pF.toPoint()) {
			qDebug() << m_segmentIdx;
			m_moved = true;
			QPoint moveDist = p-m_lastPos;


			// update connector segments

			double dx = moveDist.x();
			double dy = moveDist.y();

			int segIdx = m_segmentIdx;
			Q_ASSERT(segIdx >= 0);
			Q_ASSERT(segIdx < m_connector->m_segments.size());

			// we must distribute the dx and dy to both sides of the moved connector
			// modifying the offsets in connector segments with lower index will move the currently
			// selected segment accordingly.

			// For example: Suppose the mouse was moved by 8 pixels to the right.
			//              a) the start position is to the left, then we need to reduce the offset of the
			//                 first horizontal segments before the current segment by 8 pixels.
			//              b) the start position is to the right, then the distance is reduced.
			// Algorithm:
			// - process all segments to the left of the connector and distribute dx and dy
			// - if afterwards dx and dy is still != 0, check if this segment can be extended to compensate
			// - if not (different direction), insert a new line segment left of this position with the matching direction and offset

			while (--segIdx >= 0 && (!Globals::nearZero(dx) || !Globals::nearZero(dy)) ) {
				// get next segment to the left
				Connector::Segment & seg = m_connector->m_segments[segIdx];
				if (!Globals::nearZero(dx)) {
					if (seg.m_direction == Qt::Horizontal) {
						seg.m_offset += dx;
						dx = 0;
					}
				}
				if (!Globals::nearZero(dy)) {
					if (seg.m_direction == Qt::Vertical) {
						seg.m_offset += dy;
						dy = 0;
					}
				}
			}

			// reset segment index for currently selected segment
			segIdx = m_segmentIdx;

			// we may have dx or dy left, in this case insert a new segment to the left to compensate the distance
			if (!Globals::nearZero(dx)) {
				// check, if we can extend the currently selected segment
				Connector::Segment & seg = m_connector->m_segments[segIdx];
				if (seg.m_direction == Qt::Horizontal) {
					seg.m_offset += dx;
				}
				else {
					Connector::Segment newSeg;
					newSeg.m_direction = Qt::Horizontal;
					newSeg.m_offset = dx;
					dx = 0;
					// always insert as first element - since we checked for the presence of horizontal
					// segments above, and we know there are none - we can savely assume
					// that we have no horizontal segment at the begin
					m_connector->m_segments.insert(0, newSeg);
					++segIdx; // mind, our own segment index has shifted
					m_segmentIdx = segIdx;
				}
			}

			if (!Globals::nearZero(dy)) {
				Connector::Segment & seg = m_connector->m_segments[segIdx];
				if (seg.m_direction == Qt::Vertical) {
					seg.m_offset += dy;
				}
				else {
					Connector::Segment newSeg;
					newSeg.m_direction = Qt::Vertical;
					newSeg.m_offset = dy;
					dy = 0;
					m_connector->m_segments.insert(0, newSeg);
					++segIdx; // mind, our own segment index has shifted
					m_segmentIdx = segIdx;
				}
			}


			// same for the connectors towards the end
			dx = moveDist.x();
			dy = moveDist.y();

			while (++segIdx < m_connector->m_segments.count() && (!Globals::nearZero(dx) || !Globals::nearZero(dy)) ) {
				// get next segment to the left
				Connector::Segment & seg = m_connector->m_segments[segIdx];
				if (!Globals::nearZero(dx)) {
					if (seg.m_direction == Qt::Horizontal) {
						seg.m_offset -= dx;
						dx = 0;
					}
				}
				if (!Globals::nearZero(dy)) {
					if (seg.m_direction == Qt::Vertical) {
						seg.m_offset -= dy;
						dy = 0;
					}
				}
			}

			// we may have dx or dy left, in this case insert a new segment to the left to compensate the distance
			if (!Globals::nearZero(dx)) {
				Connector::Segment newSeg;
				newSeg.m_direction = Qt::Horizontal;
				newSeg.m_offset = -dx;
				dx = 0;
				m_connector->m_segments.append(newSeg);
			}

			if (!Globals::nearZero(dy)) {
				Connector::Segment newSeg;
				newSeg.m_direction = Qt::Vertical;
				newSeg.m_offset = -dy;
				dy = 0;
				m_connector->m_segments.append(newSeg);
			}

			// manually correct the line's coordinates

			// inform network to update connectors

//			setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
			// Note: when the sceneManager calls setLine() in this line segment, it already gives new scene coordinates.
			//       However, at the end of the function, also the position of line is modified. Thus, the line is moved twice the distance.
			SceneManager * sceneManager = qobject_cast<SceneManager *>(scene());
			if (sceneManager != nullptr)
				sceneManager->connectorSegmentMoved(this);
//			setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
//			qDebug() << "Move: " << moveDist;
//			qDebug() << "Before : " << oldLine;
//			qDebug() << "After scene manager: " << line();

			QPointF p1 = line().p1();
			QPointF p2 = line().p2();
			p1 -= moveDist;
			p2 -= moveDist;
			setLine( QLineF(p1, p2));
//			qDebug() << "After correction : " << line();

			// only now store the new position -> mind that setLine() is called from sceneManager but the position of the line is only updated when this function returns
			m_lastPos = p;
		}
		return pF;
	}
	return QGraphicsLineItem::itemChange(change, value);
}

} // namespace BLOCKMOD

