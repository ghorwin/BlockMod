#include "BM_BlockItem.h"

#include <cmath>

#include <QPainter>
#include <QLinearGradient>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include "BM_Block.h"
#include "BM_Globals.h"

namespace BLOCKMOD {


BlockItem::BlockItem(Block * b) :
	QGraphicsRectItem(),
	m_block(b)
{
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
	setZValue(10);
}

#if 0
void BlockItem::setFromBlock(const Block & b) {
	m_block = &b;
	setPos(b.m_pos);

//	qDeleteAll(m_socketItems);

	for (int i=0; i<b.m_sockets.size(); ++i) {
		// a socket is drawn by a polygon based on its point, orientation and inlet type

		const Socket & s = b.m_sockets[i];
		if (s.m_orientation == Qt::Horizontal) {
			// arrow pointing right
			QPolygonF poly;
			poly.append(s.m_pos);
			poly.append(s.m_pos + QPointF(-5, 2));
			poly.append(s.m_pos + QPointF(-5, -2));
			poly.append(s.m_pos);
			QGraphicsPolygonItem * item = new QGraphicsPolygonItem(this);
			item->setPolygon(poly);
			item->setZValue(15);
			QLinearGradient grad(QPointF(s.m_pos+QPointF(-5,0)), s.m_pos);
//			QLinearGradient grad(QPointF(0,0), QPointF(5,0));
			grad.setColorAt(0, QColor(128,0,0));
			grad.setColorAt(1, QColor(255,64,64));
			item->setBrush(grad);
			item->setPen( QPen(Qt::black) );
			m_socketItems.append(item);
		}
		else {

		}
	}
}

#endif


// *** protected functions ***

void BlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			QWidget */*widget*/)
{
	QLinearGradient grad(QPointF(0,0), QPointF(rect().width(),0));
	if (option->state & QStyle::State_Selected) {
		painter->setPen(QPen(QBrush(QColor(0,128,0)), 1.5));
		grad.setColorAt(0, QColor(255,255,255));
		grad.setColorAt(1, QColor(200,255,200));
	}
	else {
		grad.setColorAt(0, QColor(196,196,255));
		grad.setColorAt(1, QColor(255,255,255));
	}
	painter->setBrush(grad);
	painter->fillRect(rect(), grad);
	painter->setPen( Qt::black );
	painter->drawRect(rect());
	// now draw the label of the block
	painter->drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, m_block->m_name);
}


void BlockItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	if (event->button() == Qt::LeftButton && event->modifiers().testFlag(Qt::ControlModifier)) {
		setSelected(true);
		event->accept();
		return;
	}
	if (event->button() == Qt::LeftButton && !m_moved) {
		// TODO : signal that a block selection has been made and optionally de-select other blocks
	}
	QGraphicsRectItem::mouseReleaseEvent(event);
	m_moved = false;
}


QVariant BlockItem::itemChange(GraphicsItemChange change, const QVariant & value) {
	if (change == QGraphicsItem::ItemPositionChange) {
		// enable snappig
		QPointF pos = value.toPointF();
		pos.setX( std::floor(pos.x() / Globals::GridSpacing) * Globals::GridSpacing);
		pos.setY( std::floor(pos.y() / Globals::GridSpacing) * Globals::GridSpacing);
		if (m_block->m_pos != pos.toPoint()) {
			m_moved = true;
			m_block->m_pos = pos.toPoint();
			// TODO : modify network

			// TODO : modify connectors
		}
		// notify scene of changed scene rect
		QGraphicsScene * sc = scene();
		if (sc == nullptr)
			return pos;
		QRectF srect = scene()->sceneRect();
		if (pos.x() < srect.left() || pos.y() < srect.top() ||
			(pos.x() + rect().width()) > srect.right() ||
			(pos.y() + rect().height()) > srect.bottom())
		{
			scene()->setSceneRect( QRectF()); // tell scene to recompute scene rect
		}
		return pos;
	}
	return QGraphicsRectItem::itemChange(change, value);
}

} // namespace BLOCKMOD
