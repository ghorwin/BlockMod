#include "BM_SceneManager.h"

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QDebug>

#include <iostream>

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


void SceneManager::blockMoved(const Block * block) {
	// lookup connected connectors
	QSet<Connector *> & cons = m_blockConnectorMap[block];
	// adjust connectors to new block positions
	for (Connector * con : cons) {
		m_network->adjustConnector(*con);
		// update corresponding connectorItems (maybe remove/add items)
		updateConnectorSegmentItems(*con);
	}
}


void SceneManager::connectorMoved(const Connector * con) {
	// update corresponding connectorItems (maybe remove/add items)
	updateConnectorSegmentItems(*con);
}


// ** protected functions **

BlockItem * SceneManager::createBlockItem(Block & b) {
	BlockItem * item = new BlockItem(&b);
	item->setRect(0,0,b.m_size.width(), b.m_size.height());
	item->setPos(b.m_pos);
	return item;
}


QList<ConnectorSegmentItem *> SceneManager::createConnectorItems(Connector & con) {
	QList<ConnectorSegmentItem *> newConns;
	// we need to create two segment items for the start and end line segments, and give
	// them the properties to be painted appropriately

	try {
		const Socket * socket;
		const Block * block;
		m_network->lookupBlockAndSocket(con.m_sourceSocket, block, socket);
		m_blockConnectorMap[block].insert(&con); // remember association
		// get start line coordinates: first point is the socket's center, second point is the connection point outside the socket
		QLineF startLine = block->socketStartLine(socket);
		// get end line coordinates: first point is the socket's center, second point is the connection point outside the socket
		m_network->lookupBlockAndSocket(con.m_targetSocket, block, socket);
		m_blockConnectorMap[block].insert(&con); // remember association
		QLineF endLine = block->socketStartLine(socket);

		ConnectorSegmentItem * item = new ConnectorSegmentItem(&con);
		item->setLine(startLine);
		item->m_segmentIdx = -1; // start line
		item->setAcceptHoverEvents(false); // can't move start segment
		newConns.append(item);

		item = new ConnectorSegmentItem(&con);
		item->setLine(endLine);
		item->m_segmentIdx = -2; // end line
		item->setAcceptHoverEvents(false); // can't move start segment
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
	}
	catch (std::runtime_error & e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Error creating connector items - invalid network data?" << std::endl;
		qDeleteAll(newConns);
		newConns.clear();
	}

	return newConns;
}


void SceneManager::updateConnectorSegmentItems(const Connector & con) {
	// lookup corresponding connectorItems
	ConnectorSegmentItem*	startSegment = nullptr;
	ConnectorSegmentItem*	endSegment = nullptr;
	QList<ConnectorSegmentItem*> segmentItems;
	for (ConnectorSegmentItem* segmentItem : m_connectorSegmentItems) {
		if (segmentItem->m_connector == &con) {
			if (segmentItem->m_segmentIdx == -1)
				startSegment = segmentItem;
			else if (segmentItem->m_segmentIdx == -2)
				endSegment = segmentItem;
			else {
				while (segmentItems.count() <= segmentItem->m_segmentIdx)
					segmentItems.append(nullptr);
				segmentItems[segmentItem->m_segmentIdx] = segmentItem;
			}
		}
	}
	// sanity checks
	Q_ASSERT(startSegment != nullptr);
	Q_ASSERT(endSegment != nullptr);

	// remove any superfluous segment items
	while (segmentItems.count() > con.m_segments.count()) {
		ConnectorSegmentItem* segmentItem = segmentItems.back();
		m_connectorSegmentItems.removeOne(segmentItem);
		delete segmentItem;
		segmentItems.pop_back();
	}

	// now process all segment items
	try {
		const Socket * socket;
		const Block * block;
		m_network->lookupBlockAndSocket(con.m_sourceSocket, block, socket);
		// get start line coordinates: first point is the socket's center, second point is the connection point outside the socket
		QLineF startLine = block->socketStartLine(socket);
		QPointF pos = startSegment->pos();
		startLine.translate(-pos);
		startSegment->setLine(startLine);
		// get end line coordinates: first point is the socket's center, second point is the connection point outside the socket
		m_network->lookupBlockAndSocket(con.m_targetSocket, block, socket);
		QLineF endLine = block->socketStartLine(socket);
		pos = endSegment->pos();
		endLine.translate(-pos);
		endSegment->setLine(endLine);

		QPointF start = startLine.p2();
		qDebug() << "Updating " << con.m_segments.count() << " segments";
		for (int i=0; i<con.m_segments.count(); ++i) {
			const Connector::Segment & seg = con.m_segments[i];
			// create new segment items if new ones have been added in the meantime
			ConnectorSegmentItem* item;
			if (i>= segmentItems.count()) {
				item = new ConnectorSegmentItem(const_cast<Connector*>(&con)); // need to get write access for connector in newly created item
				qDebug() << "Adding item";
				addItem(item);
				m_connectorSegmentItems.append(item);
			}
			else {
				item = segmentItems[i];
				if (item == nullptr) {
					qDebug() << "Adding item";
					item = new ConnectorSegmentItem(const_cast<Connector*>(&con)); // need to get write access for connector in newly created item
					addItem(item);
					m_connectorSegmentItems.append(item);
				}
			}
			QPointF next(start);
			if (seg.m_direction == Qt::Horizontal)
				next += QPointF(seg.m_offset, 0);
			else
				next += QPointF(0, seg.m_offset);
			QLineF newLine(start, next);
			pos = item->pos();
			newLine.translate(-pos);
			item->setLine(newLine);
			item->m_segmentIdx = i; // regular line segment
			start = next;
			qDebug() << "[" << i << "] " << newLine;
		}
	} catch (...) {
		// error handling
	}
}

} // namespace BLOCKMOD
