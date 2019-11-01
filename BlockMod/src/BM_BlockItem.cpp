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

#include "BM_BlockItem.h"

#include <cmath>
#include <iostream>

#include <QPainter>
#include <QLinearGradient>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QDebug>

#include "BM_Block.h"
#include "BM_Globals.h"
#include "BM_SocketItem.h"
#include "BM_SceneManager.h"

namespace BLOCKMOD {

BlockItem::BlockItem(Block * b) :
	QGraphicsRectItem(),
	m_block(b)
{
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
	setZValue(10);

	createSocketItems();
}


SocketItem * BlockItem::inletSocketAcceptingConnection(const QPointF & scenePos) {
	for (SocketItem * si : m_socketItems) {
		QPointF socketScenePos = si->mapToScene(si->socket()->m_pos);

//		QPointF socketScenePos2(socketScenePos);
		socketScenePos -= scenePos;
		double d = socketScenePos.manhattanLength();
		if (d < Globals::GridSpacing/2) { // half grid spacing snapping tolerance
//			qDebug() << d << scenePos << socketScenePos2;
			return si;
		}
	}
	return nullptr;
}


bool BlockItem::isInvisible() const {
	return m_block->m_name == Globals::InvisibleLabel;
}


void BlockItem::resize(int newWidth, int newHeight) {
	// adjust size of associated block
	m_block->m_size = QSizeF(newWidth, newHeight);
	setRect(0, 0, newWidth, newHeight);

	// adjust positions of sockets
	for (Socket & s : m_block->m_sockets) {
		if (s.m_orientation == Qt::Horizontal) {
			if (s.m_pos.x() != 0.0)
				s.m_pos.setX(newWidth);
		}
		else {
			if (s.m_pos.y() != 0.0)
				s.m_pos.setY(newHeight);
		}

	}

	// tell all sockest to update
	for (QGraphicsItem * item : childItems()) {
		SocketItem * sitem = (SocketItem*)(item);
		sitem->updateSocketItem();
		item->update();
	}
	update();

}


// *** protected functions ***


void BlockItem::createSocketItems() {
	Q_ASSERT(m_socketItems.isEmpty());

	// the socket items are children of the block item and are added/removed together with the
	// parent block item
	for (Socket & s : m_block->m_sockets) {
		// create a socket item
		SocketItem * item = new SocketItem(this, &s);
		// enable hover-highlight on outlet nodes
		if (!s.m_inlet) {
			item->setZValue(20); // outlet nodes are drawn over lines
		}
		m_socketItems.append(item);
	}
}


void BlockItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget */*widget*/) {
	// special handling for invisible blocks
	if (isInvisible())
		return; // nothing to be drawn
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	QLinearGradient grad(QPointF(0,0), QPointF(rect().width(),0));
	if (option->state & QStyle::State_Selected) {
		painter->setPen(QPen(QBrush(QColor(0,128,0)), 1.5));
		grad.setColorAt(0, QColor(230,255,230));
		grad.setColorAt(1, QColor(200,240,180));
	}
	else {
		grad.setColorAt(0, QColor(196,196,255));
		grad.setColorAt(1, QColor(220,220,255));
	}
	painter->setBrush(grad);
	painter->fillRect(rect(), grad);
	painter->setPen( Qt::black );
	painter->drawRect(rect());
	// now draw the label of the block
	QRectF r = rect();
	r.moveTop(4);
	painter->drawText(r, Qt::AlignTop | Qt::AlignHCenter, m_block->m_name);
	painter->restore();
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
		SceneManager * sceneManager = qobject_cast<SceneManager *>(scene());

		// snap to grid
		QPointF pos = value.toPointF();

		// apply true rounding
		pos.setX( std::floor((pos.x()+0.5*Globals::GridSpacing) / Globals::GridSpacing) * Globals::GridSpacing);
		pos.setY( std::floor((pos.y()+0.5*Globals::GridSpacing) / Globals::GridSpacing) * Globals::GridSpacing);
		if (m_block->m_pos != pos.toPoint()) {
			m_moved = true;
			QPointF oldPos = m_block->m_pos;
			m_block->m_pos = pos.toPoint();
			// inform network to update connectors
			if (sceneManager != nullptr)
				sceneManager->blockMoved(m_block, oldPos);
		}
		// notify scene of changed scene rect
		if (sceneManager == nullptr)
			return pos;
		QRectF srect = sceneManager->sceneRect();
		if (pos.x() < srect.left() || pos.y() < srect.top() ||
			(pos.x() + rect().width()) > srect.right() ||
			(pos.y() + rect().height()) > srect.bottom())
		{
			sceneManager->setSceneRect( QRectF()); // tell scene to recompute scene rect
		}
		return pos;
	}
	return QGraphicsRectItem::itemChange(change, value);
}


void BlockItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
	SceneManager * sceneManager = qobject_cast<SceneManager *>(scene());
	sceneManager->blockDoubleClicked(this);
	QGraphicsRectItem::mouseDoubleClickEvent(event);
}

} // namespace BLOCKMOD
