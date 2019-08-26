#include "BM_SceneManager.h"

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>

#include "BM_Network.h"
#include "BM_BlockItem.h"
#include "BM_ConnectorSegmentItem.h"

namespace BLOCKMOD {

SceneManager::SceneManager(QObject *parent) :
	QGraphicsScene(parent), m_network(new Network)
{
	// set render hints
}


SceneManager::~SceneManager() {
	delete m_network;
}


void SceneManager::setNetwork(const Network & network) {
	*m_network = network;

	qDeleteAll(m_blockItems);
	qDeleteAll(m_connectorSegmentItems);

	// create new graphics items
	for (int i=0; i<m_network->m_blocks.count(); ++i) {
		BlockItem * item = createBlockItem( m_network->m_blocks[i] );
		addItem(item);
		m_blockItems.append(item);
	}

	// create new graphics items for connectors
	for (int i=0; i<m_network->m_connectors.count(); ++i) {
		QList<ConnectorSegmentItem *> newConns = createConnectorItems(m_network->m_connectors[i]);
//		QGraphicsPolygonItem * item = new QGraphicsPolygonItem;
////		item->setPolygon( QPolygonF( network.m_connectors[i].m_points ) );
//		item->setBrush(Qt::NoBrush);
//		item->setPen( QPen(Qt::black) );
//		item->setZValue(1);
		for( BLOCKMOD::ConnectorSegmentItem * item : newConns) {
			addItem(item);
			m_connectorSegmentItems.append(item);
		}
	}

}

const Network & SceneManager::network() const {
	return *m_network;
}

Block & SceneManager::block(unsigned int idx) {
	return m_network->m_blocks[idx];
}


// ** protected functions **

BlockItem * SceneManager::createBlockItem(Block & b) {
	BlockItem * item = new BlockItem(&b);
	item->setRect(0,0,b.m_size.width(), b.m_size.height());
	item->setPos(b.m_pos);
	return item;
}

/*! A single connect yields actually several */
QList<ConnectorSegmentItem *> SceneManager::createConnectorItems(Connector & c) {
	QList<ConnectorSegmentItem *> newConns;
	return newConns;
}


} // namespace BLOCKMOD
