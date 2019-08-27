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
		for( BLOCKMOD::ConnectorSegmentItem * item : newConns) {
			addItem(item);
			m_connectorSegmentItems.append(item);
//			qDebug() << item << " : " << item->m_connector << " : " << item->m_segmentIdx << " : " << item->line();
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
		updateConnectorSegmentItems(*con, nullptr);
	}
}


void SceneManager::connectorSegmentMoved(ConnectorSegmentItem * currentItem) {
	// update corresponding connectorItems (maybe remove/add items)
	updateConnectorSegmentItems(*currentItem->m_connector, currentItem);
}


void SceneManager::highlightConnectorSegments(const Connector & con, bool highlighted) {
	for (ConnectorSegmentItem* segmentItem : m_connectorSegmentItems) {
		if (segmentItem->m_connector == &con) {
			segmentItem->m_isHighlighted = highlighted;
			segmentItem->update();
		}
	}
	this->update();
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


void SceneManager::updateConnectorSegmentItems(const Connector & con, ConnectorSegmentItem * currentItem) {
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
				if (currentItem == nullptr || segmentItem != currentItem)
					segmentItems.append(segmentItem);
			}
		}
	}
	// sanity checks
	Q_ASSERT(startSegment != nullptr);
	Q_ASSERT(endSegment != nullptr);
	// segmentItems now contains all segment items matching this connected, except the currentItem
	int itemsNeeded = con.m_segments.count();
	if (currentItem != nullptr)
		--itemsNeeded;

	// remove any superfluous segment items
	while (segmentItems.count() > itemsNeeded) {
		ConnectorSegmentItem* segmentItem = segmentItems.back();
		m_connectorSegmentItems.removeOne(segmentItem);
		delete segmentItem;
		segmentItems.pop_back();
	}

	bool highlighted = false;
	if (currentItem != nullptr)
		highlighted = currentItem->m_isHighlighted;
	else if (!segmentItems.isEmpty())
		highlighted = segmentItems.front()->m_isHighlighted;

	// add missing items
	for (int i=segmentItems.count(); i<itemsNeeded; ++i) {
		ConnectorSegmentItem * item = new ConnectorSegmentItem(const_cast<Connector*>(&con)); // need to get write access for connector in newly created item
		item->m_isHighlighted = highlighted;
		addItem(item);
		m_connectorSegmentItems.append(item);
		segmentItems.append(item);
	}

	// insert current item at correct index position
	if (currentItem != nullptr) {
		Q_ASSERT(currentItem->m_segmentIdx < segmentItems.count());
		segmentItems.insert(currentItem->m_segmentIdx, currentItem);
	}

	Q_ASSERT(segmentItems.count() == con.m_segments.count());

	// now process all segment items
	try {
		// first start and end segments
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

		// now all others
		QPointF start = startLine.p2();
		for (int i=0; i<con.m_segments.count(); ++i) {
			const Connector::Segment & seg = con.m_segments[i];
			// create new segment items if new ones have been added in the meantime
			ConnectorSegmentItem* item = segmentItems[i];
			Q_ASSERT(item != nullptr);
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
		}
	} catch (...) {
		// error handling
	}
//	for (auto item : m_connectorSegmentItems)
//		qDebug() << item << " : " << item->m_connector << " : " << item->m_segmentIdx << " : " << item->line();
}

} // namespace BLOCKMOD
