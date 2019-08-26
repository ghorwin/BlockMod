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
QList<ConnectorSegmentItem *> SceneManager::createConnectorItems(Connector & con) {
	QList<ConnectorSegmentItem *> newConns;
	// we need to create two segment items for the start and end line segments, and give
	// them the properties to be painted appropriately

	try {
		const Socket * socket;
		const Block * block;
		m_network->lookupBlockAndSocket(con.m_sourceSocket, block, socket);
		// get start coordinates: first point is the socket's center, second point is the connection point outside the socket
		QLineF startLine = block->socketStartLine(socket);
		m_network->lookupBlockAndSocket(con.m_targetSocket, block, socket);
		// get start coordinates: first point is the socket's center, second point is the connection point outside the socket
		QLineF endLine = block->socketStartLine(socket);

		ConnectorSegmentItem * item = new ConnectorSegmentItem(&con);
		item->setLine(startLine);
		item->m_segmentIdx = -1; // start line
		newConns.append(item);

		item = new ConnectorSegmentItem(&con);
		item->setLine(endLine);
		item->m_segmentIdx = -2; // end line
		newConns.append(item);

		QPointF start = startLine.p2();
		for (int i=0; i<con.m_segments.count(); ++i) {
			const Connector::Segment & seg = con.m_segments[i];
			item = new ConnectorSegmentItem(&con);
			QPointF next(start);
			if (seg.m_direction == Qt::Horizontal)
				next += QPointF(seg.m_offset, 0);
			else
				next += QPointF(0, seg.m_offset);
			item->setLine(QLineF(start, next));
			item->m_segmentIdx = i; // regular line segment
			newConns.append(item);
			start = next;
		}
	} catch (...) {
		// error handling
	}

	return newConns;
}


} // namespace BLOCKMOD
