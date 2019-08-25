#include "BM_SceneManager.h"

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>

#include "BM_Network.h"
#include "BM_BlockItem.h"

namespace BLOCKMOD {

SceneManager::SceneManager(QObject *parent) :
	QGraphicsScene(parent), m_network(NULL)
{
}


void SceneManager::setNetwork(Network & network) {
	m_network = &network;

	qDeleteAll(m_items);

	// create new graphics items
	for (int i=0; i<network.m_blocks.count(); ++i) {
		BlockItem * item = new BlockItem;
		item->setFromBlock(network.m_blocks[i]);
		addItem(item);
		m_items.append(item);
	}

	// create new graphics items for connectors
	for (int i=0; i<network.m_connectors.count(); ++i) {
		QGraphicsPolygonItem * item = new QGraphicsPolygonItem;
		item->setPolygon( QPolygonF( network.m_connectors[i].m_points ) );
		item->setBrush(Qt::NoBrush);
		item->setPen( QPen(Qt::black) );
		item->setZValue(1);
		addItem(item);
	}

}

} // namespace BLOCKMOD
