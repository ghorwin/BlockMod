#include "BM_SocketItem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include "BM_Socket.h"
#include "BM_BlockItem.h"

namespace BLOCKMOD {

SocketItem::SocketItem(BlockItem * parent, Socket * socket) :
	QGraphicsItem (parent),
	m_socket(socket)
{
	if (m_socket->m_inlet) {
		if (m_socket->m_orientation == Qt::Horizontal) {
			if (m_socket->m_pos.x() == 0.0)
				m_symbolRect = QRectF(-4, m_socket->m_pos.y()-4, 8, 8);
			else
				m_symbolRect = QRectF(m_socket->m_pos.x()-4, m_socket->m_pos.y()-4, 4, 8);
		}
	}
	else {
		if (m_socket->m_orientation == Qt::Horizontal) {
			if (m_socket->m_pos.x() == 0.0)
				m_symbolRect = QRectF(0, m_socket->m_pos.y()-4, 4, 8);
			else
				m_symbolRect = QRectF(m_socket->m_pos.x()-4, m_socket->m_pos.y()-4, 4, 8);
		}
	}
	setZValue(12); // painted on top of block
}


QRectF SocketItem::boundingRect() const {
	return m_symbolRect;
}


// *** protected functions ***

void SocketItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			QWidget */*widget*/)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	// Socket items are children of the blocks.
	// Coordinates are hence defined with respect to the parent item's coordinate system.
	// 0,0 is the top-left corner of the parent block.

	// distinguish between inlet and outlet sockets
	if (m_socket->m_inlet) {
		painter->setBrush(Qt::white);
		QRectF r = m_symbolRect;
//		painter->drawRect(r);
		painter->setPen(Qt::white);
		painter->drawPie(r, -270*16, -180*16);
		painter->setPen(Qt::black);
		painter->drawArc(r, -270*16, -180*16);
//		if (option->state & QStyle::State_Selected) {
//			painter->setPen(QPen(QBrush(QColor(0,96,0)), 1.5));
//		}
//		else {
//			grad.setColorAt(0, QColor(196,196,255));
//			grad.setColorAt(1, QColor(255,255,255));
//		}
	}
	else {
		painter->setPen(Qt::black);
		painter->setBrush(Qt::white);
		QRectF r = m_symbolRect;
		painter->drawRect(r);
	}

	// now draw the label on the socket
//	painter->drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, m_block->m_name);
	painter->restore();
}

} // namespace BLOCKMOD

