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

#include "BM_SocketItem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QFontMetrics>
#include <QDebug>
#include <QGraphicsView>
#include <QApplication>

#include "BM_Socket.h"
#include "BM_BlockItem.h"
#include "BM_Block.h"
#include "BM_Globals.h"
#include "BM_SceneManager.h"

namespace BLOCKMOD {

SocketItem::SocketItem(BlockItem * parent, Socket * socket) :
	QGraphicsItem (parent),
	m_socket(socket),
	m_hoverEnabled(false),
	m_hovered(false)
{
	updateSocketItem();
	setZValue(12);
	setAcceptHoverEvents(true);
}


void SocketItem::updateSocketItem() {
	if (m_socket->m_inlet) {
		switch (m_socket->direction()) {
			case Socket::Left		: m_symbolRect = QRectF(-4, m_socket->m_pos.y()-4, 8, 8); break;
			case Socket::Right		: m_symbolRect = QRectF(m_socket->m_pos.x()-4, m_socket->m_pos.y()-4, 8, 8); break;
			case Socket::Top		: m_symbolRect = QRectF(m_socket->m_pos.x()-4, -4, 8, 8); break;
			case Socket::Bottom		: m_symbolRect = QRectF(m_socket->m_pos.x()-4, m_socket->m_pos.y()-4, 8, 8); break;
		}
	}
	else {
		switch (m_socket->direction()) {
			case Socket::Left		: m_symbolRect = QRectF(-8, m_socket->m_pos.y()-4, 8, 8); break;
			case Socket::Right		: m_symbolRect = QRectF(m_socket->m_pos.x(), m_socket->m_pos.y()-4, 8, 8); break;
			case Socket::Top		: m_symbolRect = QRectF(m_socket->m_pos.x()-4, -8, 8, 8); break;
			case Socket::Bottom		: m_symbolRect = QRectF(m_socket->m_pos.x()-4, m_socket->m_pos.y(), 8, 8); break;
		}
	}
}


QRectF SocketItem::boundingRect() const {
	QRectF r = m_symbolRect;
	// add space for text
	QFont f;
	f.setPointSizeF(Globals::LabelFontSize);
	QFontMetricsF metrics(f);
	QRectF textBoundingRect = metrics.boundingRect(m_socket->m_name);
	textBoundingRect.setWidth(textBoundingRect.width()+6); // add some space to avoid clipping of italic fonts to the right

	switch (m_socket->direction()) {
		case Socket::Left		:
			// left side - expand rect to left
			r.moveLeft(r.left()-textBoundingRect.width()-6);
		break;
		case Socket::Right		:
			// right side - expand rect to right
			r.setWidth(r.width()+textBoundingRect.width()+6);
		break;
		case Socket::Top		:
			// top side - move top
			r.moveTop(r.top()-textBoundingRect.width()-6);
		break;
		case Socket::Bottom		:
			r.setHeight(r.height() + textBoundingRect.width()+6);
		break;
	}
	return r;
}


void SocketItem::setHoverEnabled(bool enabled) {
	if (enabled) {
		m_hoverEnabled = true;
	}
	else {
		m_hoverEnabled = false;
		m_hovered = false;
	}
	update();
}


// *** protected functions ***

void SocketItem::hoverEnterEvent (QGraphicsSceneHoverEvent *event) {
	if (m_hoverEnabled)
		m_hovered = true;
	QGraphicsItem::hoverEnterEvent(event);
}


void SocketItem::hoverLeaveEvent (QGraphicsSceneHoverEvent *event) {
	m_hovered = false;
	QGraphicsItem::hoverLeaveEvent(event);
}


void SocketItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/ ) {
	// special handling for invisible blocks
	BlockItem * bi = dynamic_cast<BlockItem*>(parentItem());
	if (bi->block()->m_name == Globals::InvisibleLabel)
		return; // nothing to be drawn
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	// Socket items are children of the blocks.
	// Coordinates are hence defined with respect to the parent item's coordinate system.
	// 0,0 is the top-left corner of the parent block.

	// distinguish between inlet and outlet sockets
	QRectF r = m_symbolRect;
	if (m_socket->m_inlet) {
		painter->setBrush(Qt::white);
		switch (m_socket->direction()) {
			case Socket::Left		:
				// left side
				painter->setPen(Qt::white);
				painter->drawPie(r, 90*16, -180*16);
				painter->setPen(Qt::black);
				painter->drawArc(r, 90*16, -180*16);
			break;
			case Socket::Right		:
				// right side
				painter->setPen(Qt::white);
				painter->drawPie(r, 90*16, 180*16);
				painter->setPen(Qt::black);
				painter->drawArc(r, 90*16, 180*16);
			break;
			case Socket::Top		:
				// top side
				painter->setPen(Qt::white);
				painter->drawPie(r, 0*16, -180*16);
				painter->setPen(Qt::black);
				painter->drawArc(r, 0*16, -180*16);
			break;
			case Socket::Bottom		:
				// bottom side
				painter->setPen(Qt::white);
				painter->drawPie(r, 0*16, 180*16);
				painter->setPen(Qt::black);
				painter->drawArc(r, 0*16, 180*16);
			break;
		}
		// if socket is connected, paint a circle
		SceneManager * sceneManager = qobject_cast<SceneManager *>(scene());
		BlockItem * blockItem = dynamic_cast<BlockItem *>(parentItem());
		if (sceneManager != nullptr && blockItem != nullptr && sceneManager->isConnectedSocket(blockItem->block(), m_socket)) {
			painter->save();
			painter->setPen(Qt::NoPen);
			painter->setBrush(Qt::black);
			QRectF r2(r.x()+2, r.y()+2, r.width() - 4, r.height() - 4);
			painter->drawEllipse(r2);
			painter->restore();
		}

		if (m_hovered) {
			painter->save();
			QPen p(QColor(192,0,0), 0.8);
			painter->setPen(p);
			painter->setBrush(QBrush(QColor(96,0,0)));
			QRectF r2(r.x()-1, r.y()-1, r.width() + 2, r.height() + 2);
			painter->drawEllipse(r2);
			painter->restore();
		}
	}
	else {
		QPainterPath p;
		switch (m_socket->direction()) {
			case Socket::Left		:
				// left side
				p.moveTo(m_symbolRect.right(), m_symbolRect.y());
				p.lineTo(m_symbolRect.left(), 0.5*(m_symbolRect.top() + m_symbolRect.bottom()));
				p.lineTo(m_symbolRect.right(), m_symbolRect.bottom());
			break;
			case Socket::Right		:
				// right side
				p.moveTo(m_symbolRect.left(), m_symbolRect.y());
				p.lineTo(m_symbolRect.right(), 0.5*(m_symbolRect.top() + m_symbolRect.bottom()));
				p.lineTo(m_symbolRect.left(), m_symbolRect.bottom());
			break;
			case Socket::Top		:
				// top side
				p.moveTo(m_symbolRect.left(), m_symbolRect.bottom());
				p.lineTo(0.5*(m_symbolRect.left() + m_symbolRect.right()), m_symbolRect.top());
				p.lineTo(m_symbolRect.right(), m_symbolRect.bottom());
			break;
			case Socket::Bottom		:
				// bottom side
				p.moveTo(m_symbolRect.left(), m_symbolRect.top());
				p.lineTo(0.5*(m_symbolRect.left() + m_symbolRect.right()), m_symbolRect.bottom());
				p.lineTo(m_symbolRect.right(), m_symbolRect.top());
			break;
		}
		QPen pen(Qt::black);
		pen.setCapStyle(Qt::RoundCap);
		painter->setPen(pen);
		if (m_hovered)
			painter->fillPath(p, Qt::white);
		else
			painter->setBrush(QColor(0,0,196));
		painter->drawPath(p);
	}


	// now draw the label on the socket
	QFont f(painter->font());
	f.setPointSizeF(Globals::LabelFontSize);
	QFontMetricsF metrics(f);
	painter->setFont(f);
	QRectF textBoundingRect = metrics.boundingRect(m_socket->m_name);
	textBoundingRect.setWidth(textBoundingRect.width()+6); // add some space to avoid clipping of italic fonts to the right

	switch (m_socket->direction()) {
		case Socket::Left		:
			// left side
			textBoundingRect.moveTo(r.left()-textBoundingRect.width(), r.top()-textBoundingRect.height()+3);
			painter->drawText(textBoundingRect, Qt::AlignRight | Qt::AlignTop, m_socket->m_name);
		break;
		case Socket::Right		:
			// right side
			textBoundingRect.moveTo(r.right(), r.top()-textBoundingRect.height()+3);
			painter->drawText(textBoundingRect, Qt::AlignLeft | Qt::AlignTop, m_socket->m_name);
		break;
		case Socket::Top		:
			// top side
			painter->translate(r.left(), r.top());
			painter->rotate(-90);
			textBoundingRect.moveTo(0, -textBoundingRect.height());
			painter->drawText(textBoundingRect, Qt::AlignLeft | Qt::AlignTop, m_socket->m_name);
		break;
		case Socket::Bottom		:
			// bottom side
			painter->translate(r.left(), r.bottom());
			painter->rotate(-90);
			textBoundingRect.moveTo(-textBoundingRect.width(), -textBoundingRect.height());
			painter->drawText(textBoundingRect, Qt::AlignRight | Qt::AlignTop, m_socket->m_name);
		break;
	}
	//	painter->setBrush(Qt::NoBrush);
	//	painter->drawRect(textBoundingRect);

	painter->restore();
}


void SocketItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	// ignore clicks on inlet sockets
	if (!m_hoverEnabled) {
		event->ignore();
		return;
	}
	// starting a connection?
	if (!m_socket->m_inlet) {
		if (event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
			SceneManager * sceneManager = qobject_cast<SceneManager *>(scene());
			if (sceneManager) {
				QPointF p = event->pos(); // this is the position of the socket relative to the parent block
				p = mapToScene(p); // this is the global scene coordinate
				sceneManager->startSocketConnection(*this, p);
				event->accept(); // needed or fall through?
			}
		}
	}
}



} // namespace BLOCKMOD

