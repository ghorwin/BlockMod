#include "BM_BlockItem.h"

#include <cmath>

#include <QPainter>
#include <QLinearGradient>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

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


// *** protected functions ***

/*! This function is called from the constructor and creates child socket items. */
void BlockItem::createSocketItems() {
	Q_ASSERT(m_socketItems.isEmpty());

	// the socket items are children of the block item and are added/removed together with the
	// parent block item
	for (Socket & s : m_block->m_sockets) {
		// create a socket item
		SocketItem * item = new SocketItem(this, &s);
		m_socketItems.append(item);
	}
}


void BlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			QWidget */*widget*/)
{
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
		// snap to grid
		QPointF pos = value.toPointF();
		pos.setX( std::floor(pos.x() / Globals::GridSpacing) * Globals::GridSpacing);
		pos.setY( std::floor(pos.y() / Globals::GridSpacing) * Globals::GridSpacing);
		if (m_block->m_pos != pos.toPoint()) {
			m_moved = true;
			m_block->m_pos = pos.toPoint();
			// inform network to update connectors
			SceneManager * sceneManager = qobject_cast<SceneManager *>(scene());
			if (sceneManager != nullptr)
				sceneManager->blockMoved(m_block);
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
