#include "BM_SocketItem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QFontMetrics>

#include "BM_Socket.h"
#include "BM_BlockItem.h"
#include "BM_Globals.h"

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
				m_symbolRect = QRectF(m_socket->m_pos.x()-4, m_socket->m_pos.y()-4, 8, 8);
		}
		else {
			if (m_socket->m_pos.y() == 0.0)
				m_symbolRect = QRectF(m_socket->m_pos.x()-4, -4, 8, 8);
			else
				m_symbolRect = QRectF(m_socket->m_pos.x()-4, m_socket->m_pos.y()-4, 8, 8);
		}
	}
	else {
		if (m_socket->m_orientation == Qt::Horizontal) {
			if (m_socket->m_pos.x() == 0.0)
				m_symbolRect = QRectF(-8, m_socket->m_pos.y()-4, 8, 8);
			else
				m_symbolRect = QRectF(m_socket->m_pos.x(), m_socket->m_pos.y()-4, 8, 8);
		}
		else {
			if (m_socket->m_pos.y() == 0.0)
				m_symbolRect = QRectF(m_socket->m_pos.x()-4, -8, 8, 8);
			else
				m_symbolRect = QRectF(m_socket->m_pos.x()-4, m_socket->m_pos.y(), 8, 8);
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
	QRectF r = m_symbolRect;
	if (m_socket->m_inlet) {
		painter->setBrush(Qt::white);
		if (m_socket->m_orientation == Qt::Horizontal) {
			if (m_socket->m_pos.x() == 0.0) {
				// left side
				painter->setPen(Qt::white);
				painter->drawPie(r, 90*16, -180*16);
				painter->setPen(Qt::black);
				painter->drawArc(r, 90*16, -180*16);
			}
			else {
				// right side
				painter->setPen(Qt::white);
				painter->drawPie(r, 90*16, 180*16);
				painter->setPen(Qt::black);
				painter->drawArc(r, 90*16, 180*16);
			}
		}
		else {
			if (m_socket->m_pos.y() == 0.0) {
				// top side
				painter->setPen(Qt::white);
				painter->drawPie(r, 0*16, -180*16);
				painter->setPen(Qt::black);
				painter->drawArc(r, 0*16, -180*16);
			}
			else {
				// bottom side
				painter->setPen(Qt::white);
				painter->drawPie(r, 0*16, 180*16);
				painter->setPen(Qt::black);
				painter->drawArc(r, 0*16, 180*16);
			}
		}
//		if (option->state & QStyle::State_Selected) {
//			painter->setPen(QPen(QBrush(QColor(0,96,0)), 1.5));
//		}
//		else {
//			grad.setColorAt(0, QColor(196,196,255));
//			grad.setColorAt(1, QColor(255,255,255));
//		}
	}
	else {
		QPainterPath p;
		if (m_socket->m_orientation == Qt::Horizontal) {
			if (m_socket->m_pos.x() == 0.0) {
				// left side
				p.moveTo(m_symbolRect.right(), m_symbolRect.y());
				p.lineTo(m_symbolRect.left(), 0.5*(m_symbolRect.top() + m_symbolRect.bottom()));
				p.lineTo(m_symbolRect.right(), m_symbolRect.bottom());
			}
			else {
				// right side
				p.moveTo(m_symbolRect.left(), m_symbolRect.y());
				p.lineTo(m_symbolRect.right(), 0.5*(m_symbolRect.top() + m_symbolRect.bottom()));
				p.lineTo(m_symbolRect.left(), m_symbolRect.bottom());
			}
		}
		else {
			if (m_socket->m_pos.y() == 0.0) {
				// top side
				p.moveTo(m_symbolRect.left(), m_symbolRect.bottom());
				p.lineTo(0.5*(m_symbolRect.left() + m_symbolRect.right()), m_symbolRect.top());
				p.lineTo(m_symbolRect.right(), m_symbolRect.bottom());
			}
			else {
				// bottom side
				p.moveTo(m_symbolRect.left(), m_symbolRect.top());
				p.lineTo(0.5*(m_symbolRect.left() + m_symbolRect.right()), m_symbolRect.bottom());
				p.lineTo(m_symbolRect.right(), m_symbolRect.top());
			}
		}
		QPen pen(Qt::black);
		pen.setCapStyle(Qt::RoundCap);
		painter->setPen(pen);
		painter->setBrush(QColor(0,0,96));
		painter->drawPath(p);
	}


	// now draw the label on the socket
	QFont f(painter->font());
	f.setPointSizeF(Globals::LabelFontSize);
	QFontMetricsF metrics(f);
	painter->setFont(f);
	QRectF textBoundingRect = metrics.boundingRect(m_socket->m_name);
	textBoundingRect.setWidth(textBoundingRect.width()+2);

	if (m_socket->m_orientation == Qt::Horizontal) {
		if (m_socket->m_pos.x() == 0.0) {
			// left side
			textBoundingRect.moveTo(r.left()-textBoundingRect.width(), r.top()-textBoundingRect.height());
			painter->drawText(textBoundingRect, Qt::AlignRight | Qt::AlignTop, m_socket->m_name);
//			painter->setBrush(Qt::NoBrush);
//			painter->drawRect(textBoundingRect);
		}
		else {
			// right side
			textBoundingRect.moveTo(r.right(), r.top()-textBoundingRect.height());
			painter->drawText(textBoundingRect, Qt::AlignLeft | Qt::AlignTop, m_socket->m_name);
//			painter->setBrush(Qt::NoBrush);
//			painter->drawRect(textBoundingRect);
		}
	}
	else {
		if (m_socket->m_pos.y() == 0.0) {
			// top side
			painter->translate(r.left(), r.top());
			painter->rotate(-90);
			textBoundingRect.moveTo(0, -textBoundingRect.height());
			painter->drawText(textBoundingRect, Qt::AlignLeft | Qt::AlignTop, m_socket->m_name);
//			painter->setBrush(Qt::NoBrush);
//			painter->drawRect(textBoundingRect);
		}
		else {
			// bottom side
			painter->translate(r.left(), r.bottom());
			painter->rotate(-90);
			textBoundingRect.moveTo(-textBoundingRect.width(), -textBoundingRect.height());
			painter->drawText(textBoundingRect, Qt::AlignRight | Qt::AlignTop, m_socket->m_name);
//			painter->setBrush(Qt::NoBrush);
//			painter->drawRect(textBoundingRect);
		}
	}

	painter->restore();
}

} // namespace BLOCKMOD

