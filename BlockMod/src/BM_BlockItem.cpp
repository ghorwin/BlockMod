#include "BM_BlockItem.h"

#include <QPainter>
#include <QLinearGradient>
#include <QGraphicsPolygonItem>


namespace BLOCKMOD {

BlockItem::BlockItem(QObject *parent) :
	QGraphicsItem()
{
	setZValue(10);
}


void BlockItem::setFromBlock(const Block & b) {
	m_block = b;
	setPos(b.m_pos);

	qDeleteAll(m_socketItems);

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


QRectF BlockItem::boundingRect() const {
	double penWidth = 1;
	return QRectF(0 - penWidth / 2, 0 - penWidth / 2,
				  m_block.m_size.width() + penWidth, m_block.m_size.height() + penWidth);
}


void BlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			QWidget *widget)
{

	QLinearGradient grad(QPointF(0,0), QPointF(m_block.m_size.width(),0));
	grad.setColorAt(0, QColor(196,196,255));
	grad.setColorAt(1, QColor(255,255,255));
	painter->setBrush(grad);
	painter->fillRect(QRectF(0, 0, m_block.m_size.width(), m_block.m_size.height()), grad);
	painter->setPen( Qt::black );
	painter->drawRect(0, 0, m_block.m_size.width(), m_block.m_size.height());
}


} // namespace BLOCKMOD
