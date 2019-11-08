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

#include "BM_SceneManager.h"

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsView>
#include <QDebug>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>

#include <iostream>

#include "BM_Network.h"
#include "BM_Socket.h"
#include "BM_BlockItem.h"
#include "BM_ConnectorSegmentItem.h"
#include "BM_Globals.h"
#include "BM_SocketItem.h"

namespace BLOCKMOD {

SceneManager::SceneManager(QObject *parent) :
	QGraphicsScene(parent),
	m_network(new Network),
	m_connectionModeEnabled(false)
{
	// listen for selection changes
}


SceneManager::~SceneManager() {
	delete m_network;
}


void SceneManager::setNetwork(const Network & network) {
	*m_network = network;

	qDeleteAll(m_blockItems);
	m_blockItems.clear();
	qDeleteAll(m_connectorSegmentItems);
	m_connectorSegmentItems.clear();

	// clear block connector map
	m_blockConnectorMap.clear();

	// create new graphics items
	for (Block & b : m_network->m_blocks) {
		BlockItem * item = createBlockItem( b );
		addItem(item);
		m_blockItems.append(item);
	}

	// create new graphics items for connectors
	for (Connector & c : m_network->m_connectors) {
		QList<ConnectorSegmentItem *> newConns = createConnectorItems(c);
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


const BlockItem * SceneManager::blockItemByName(const QString & blockName) const {
	for (BlockItem* item : m_blockItems) {
		if (item->m_block->m_name == blockName)
			return item;
	}
	return nullptr;
}


void SceneManager::blockMoved(const Block * block, const QPointF /*oldPos*/) {
	// lookup connected connectors
	QSet<Connector *> & cons = m_blockConnectorMap[block];
	// adjust connectors to new block positions
	for (Connector * con : cons) {
		m_network->adjustConnector(*con);
		// update corresponding connectorItems (maybe remove/add items)
		updateConnectorSegmentItems(*con, nullptr);
	}

	// Mind the following problem:
	// - this function is called from within BlockItem::itemChange() event handler
	// - when you set a new network in the scene manager, for example in the slot connected
	//	to networkGeometryChanged(), this will delete all existing block items and create new ones
	// - when the execution returns from this function, it accesses now invalid memory --> boom!

	// Rule: never ever call setNetwork() when processing this signal!

	emit networkGeometryChanged(); /// \todo add old position to signal
}


void SceneManager::blockSelected(const Block * block) {
	emit newBlockSelected(block->m_name);
}


void SceneManager::connectorSegmentMoved(ConnectorSegmentItem * currentItem) {
	// update corresponding connectorItems (maybe remove/add items)
	updateConnectorSegmentItems(*currentItem->m_connector, currentItem);
	emit networkGeometryChanged();
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


void SceneManager::selectConnectorSegments(const Connector & con) {
	for (ConnectorSegmentItem* segmentItem : m_connectorSegmentItems) {
		if (segmentItem->m_connector == &con) {
			if (!segmentItem->isSelected())
				segmentItem->setSelected(true);
			segmentItem->update();
		}
	}
	this->update();
}


void SceneManager::mergeConnectorSegments(Connector & con) {
	// collect an ordered list of segment items
	QList<ConnectorSegmentItem*> segmentItems;
	for (int i=0; i<con.m_segments.count(); ++i)
		segmentItems.append(nullptr);
	for (ConnectorSegmentItem* segmentItem : m_connectorSegmentItems) {
		if (segmentItem->m_connector == &con) {
			if (segmentItem->m_segmentIdx == -1 || segmentItem->m_segmentIdx == -2)
				continue;
			Q_ASSERT(segmentItem->m_segmentIdx < segmentItems.count());
			segmentItems[segmentItem->m_segmentIdx] = segmentItem;
		}
	}
	// now look for segments with near zero distance
	int i = 0;
	bool updateSegments = false;
	while (i<segmentItems.count()) {

		for (i=0; i<segmentItems.count(); ++i) {
			ConnectorSegmentItem * segItem = segmentItems[i];
			Q_ASSERT(i == segItem->m_segmentIdx);
			Connector::Segment & seg = con.m_segments[i];

			// if segment has the same orientation as the previous segment, we merge it such that the previous segment gets modified
			// and the current segment gets offset of zero
			if (i>0 && con.m_segments[i-1].m_direction == seg.m_direction) {
				con.m_segments[i-1].m_offset += seg.m_offset;
				seg.m_offset = 0;
				updateSegments = true;
			}

			if (Globals::nearZero(seg.m_offset)) {
				break;
			}
		}

		if (i == segmentItems.count())
			break; // none found, we are done

		// if i < segmentItems.count() we have found a zero length segment
		// if this is the first or last segment, we can simply remove it
		if (i == 0) {
			con.m_segments.removeFirst();
			ConnectorSegmentItem * segItem = segmentItems.front();
			segmentItems.removeFirst();
			m_connectorSegmentItems.removeOne(segItem);
			delete segItem;
			// update segment indexes of remaining segments
			for (int j=0; j<segmentItems.count(); ++j)
				--segmentItems[j]->m_segmentIdx;
		}
		else if (i == segmentItems.count()-1) {
			con.m_segments.removeLast();
			ConnectorSegmentItem * segItem = segmentItems.back();
			segmentItems.removeLast();
			m_connectorSegmentItems.removeOne(segItem);
			delete segItem;
			i = 0; // signal to try again
		}
		else {
			// segment is somewhere in the middle

			// remove the segment in question
			con.m_segments.removeAt(i);
			ConnectorSegmentItem * segItem = segmentItems[i];
			segmentItems.removeAt(i);
			m_connectorSegmentItems.removeOne(segItem);
			delete segItem;
			// update segment indexes of remaining segments
			for (int j=i; j<segmentItems.count(); ++j)
				--segmentItems[j]->m_segmentIdx;

			// check if new neighbors have same orientation
			Connector::Segment & previousSeg = con.m_segments[i-1];
			Connector::Segment & nextSeg = con.m_segments[i];
			if (previousSeg.m_direction == nextSeg.m_direction) {
				// extend the previous segment
				previousSeg.m_offset += nextSeg.m_offset;
				// remove the next
				con.m_segments.removeAt(i);
				ConnectorSegmentItem * segItem = segmentItems[i];
				segmentItems.removeAt(i);
				m_connectorSegmentItems.removeOne(segItem);
				delete segItem;
				// update segment indexes of remaining segments
				for (int j=i; j<segmentItems.count(); ++j)
					--segmentItems[j]->m_segmentIdx;
				// update segments
				// remember to update the segment coordinates afterwards
				updateSegments = true;
			}

			i = 0; // signal to try again
		}
	}
	if (updateSegments)
		updateConnectorSegmentItems(con, nullptr);
	QApplication::restoreOverrideCursor();
}


bool SceneManager::isConnectedSocket(const Block * b, const Socket * s) const {
	QMap<const Block*, QSet<Connector*> >::const_iterator it = m_blockConnectorMap.find(b);
	if (it == m_blockConnectorMap.end())
		return false;
	// process all connectors and check, if they connect to the given socket
	for (const Connector * c : it.value() ) {
		const Socket * socket;
		const Block * block;
		m_network->lookupBlockAndSocket(c->m_sourceSocket, block, socket);
		if (s == socket)
			return true;
		m_network->lookupBlockAndSocket(c->m_targetSocket, block, socket);
		if (s == socket)
			return true;
	}
	return false;
}



void SceneManager::enableConnectionMode() {
	for (BLOCKMOD::BlockItem * block : m_blockItems) {
		for (QGraphicsItem * item : block->childItems()) {
			BLOCKMOD::SocketItem * socketItem = dynamic_cast<BLOCKMOD::SocketItem*>(item);
			Q_ASSERT(socketItem != nullptr);

			// we expect users to click on an outlet, so only outlets use hovering
			if (!socketItem->socket()->m_inlet)
				socketItem->setHoverEnabled(true);
		}
		block->setFlags(QGraphicsItem::GraphicsItemFlag(0));
	}
	for (BLOCKMOD::ConnectorSegmentItem * segmentItem : m_connectorSegmentItems) {
		segmentItem->setFlags(QGraphicsItem::GraphicsItemFlag(0));
	}
	m_connectionModeEnabled = true;
	QApplication::setOverrideCursor(Qt::CrossCursor);
}


void SceneManager::disableConnectionMode() {
	for (BLOCKMOD::BlockItem * block : m_blockItems) {
		for (QGraphicsItem * item : block->childItems()) {
			BLOCKMOD::SocketItem * socketItem = dynamic_cast<BLOCKMOD::SocketItem*>(item);
			Q_ASSERT(socketItem != nullptr);

			socketItem->setHoverEnabled(false);
		}
		block->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
	}
	for (BLOCKMOD::ConnectorSegmentItem * segmentItem : m_connectorSegmentItems) {
		if (segmentItem->m_segmentIdx >= 0)
			segmentItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
	}

	// remove our artifical block and block item, if it exists
	if (!m_network->m_blocks.empty() && m_network->m_blocks.back().m_name  == Globals::InvisibleLabel)
		removeBlock(m_network->m_blocks.size()-1);

	m_connectionModeEnabled = false;
	QApplication::restoreOverrideCursor();
}


void SceneManager::startSocketConnection(const SocketItem & outletSocketItem, const QPointF & mousePos) {
	Q_ASSERT(!outletSocketItem.socket()->m_inlet);
	// TODO : add flag to enable/disable connect mode

	// process all existing socket items and toggle the m_hoverEnabled flag for inlets and outlets


	// determine block that this outlet socket item belongs to
	BlockItem * bitem = dynamic_cast<BlockItem *>(outletSocketItem.parentItem());
	Q_ASSERT(bitem != nullptr);
	const Block * sourceBlock = bitem->block();
	const Socket * sourceSocket = outletSocketItem.socket();
	// compose connector start name
	QString startSocketName = sourceBlock->m_name + "." + sourceSocket->m_name;

	// create a dummy block
	Block dummyBlock;
//	QPointF p(sourceSocket->m_pos);
//	p += sourceBlock->m_pos;
	dummyBlock.m_pos = mousePos;
	dummyBlock.m_size = QSizeF(20,20);
	dummyBlock.m_name = Globals::InvisibleLabel; // "Mich gibt's gar nicht";
	dummyBlock.m_connectionHelperBlock = true;
	Socket dummySocket;
	dummySocket.m_name = Globals::InvisibleLabel; // "Mich gibt's auch nicht";
	dummySocket.m_inlet = true;
	dummySocket.m_orientation = Qt::Horizontal;
	dummySocket.m_pos = QPointF(0,0);
	dummyBlock.m_sockets.append(dummySocket);

	m_network->m_blocks.push_back(dummyBlock); // does not invalidate block pointers!

	QString targetSocketName = dummyBlock.m_name + "." + dummySocket.m_name;

	// create and add the connector
	Connector con;
	con.m_name = Globals::InvisibleLabel; // "Mich gibt's erst recht nicht";
	con.m_sourceSocket = startSocketName;
	con.m_targetSocket = targetSocketName;
	m_network->m_connectors.push_back(con);

	// now create block item and connector items
	BlockItem * bi = createBlockItem(m_network->m_blocks.back()); // Mind: always pass the object in the m_block list
	bi->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	bi->setPos(dummyBlock.m_pos);
	m_blockItems.append(bi);

	bi->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
	bi->setPos(dummyBlock.m_pos);

	addItem(bi);

	QList<ConnectorSegmentItem*> newConns = createConnectorItems(m_network->m_connectors.back());  // Mind: always pass the object in the m_connectors list
	for( BLOCKMOD::ConnectorSegmentItem * item : newConns) {
		addItem(item);
		m_connectorSegmentItems.append(item);
	}

	// turn off hovering for all sockets
	// Mind: while mouse button is pressed, hover events are not send
	for (BLOCKMOD::BlockItem * block : m_blockItems) {
		for (QGraphicsItem * item : block->childItems()) {
			BLOCKMOD::SocketItem * socketItem = dynamic_cast<BLOCKMOD::SocketItem*>(item);
			Q_ASSERT(socketItem != nullptr);
			socketItem->setHoverEnabled(false);
		}
	}
}


QList<const Block*> SceneManager::selectedBlocks() const {
	QList<QGraphicsItem*> selected = selectedItems();
	QList<const BLOCKMOD::Block *> selectedBlocks;
	for (QGraphicsItem * item : selected) {
		BLOCKMOD::BlockItem * bi = dynamic_cast<BLOCKMOD::BlockItem *>(item);
		if (bi != nullptr)
			selectedBlocks.append(bi->block());
	}
	return selectedBlocks;
}


const Connector * SceneManager::selectedConnector() const {
	QList<QGraphicsItem*> selected = selectedItems();
	// find selected connector
	for (QGraphicsItem * item : selected) {
		BLOCKMOD::ConnectorSegmentItem * segi = dynamic_cast<BLOCKMOD::ConnectorSegmentItem *>(item);
		if (segi != nullptr) {
			return segi->m_connector;
		}
	}
	return nullptr;
}


void SceneManager::addBlock(const Block & block) {
	m_network->m_blocks.push_back(block);
	BlockItem * item = createBlockItem( m_network->m_blocks.back() );
	addItem(item);
	m_blockItems.append(item);
}


void SceneManager::addConnector(const Connector & con) {
	// first check, that indeed the source/target connectors are valid
	const Block * b1, * b2;
	const Socket * s1, * s2;
	try {
		m_network->lookupBlockAndSocket(con.m_sourceSocket, b1, s1);
	} catch (...) {
		throw std::runtime_error("[SceneManager::addConnector] Invalid source socket identifyer.");
	}
	try {
		m_network->lookupBlockAndSocket(con.m_targetSocket, b2, s2);
	} catch (...) {
		throw std::runtime_error("[SceneManager::addConnector] Invalid target socket identifyer.");
	}
	if (s1->m_inlet)
		throw std::runtime_error("[SceneManager::addConnector] Invalid source socket (must be an outlet socket).");
	if (!s2->m_inlet)
		throw std::runtime_error("[SceneManager::addConnector] Invalid target socket (must be an inlet socket).");
	// check, if inlet socket is already connected to
	if (isConnectedSocket(b2, s2))
		throw std::runtime_error("[SceneManager::addConnector] Invalid target socket (has already an incoming connection).");
	m_network->m_connectors.push_back(con);
	m_network->adjustConnector(m_network->m_connectors.back());
}


void SceneManager::removeBlock(const Block * block) {
	size_t idx = 0;
	for (auto bit = m_network->m_blocks.begin(); bit != m_network->m_blocks.end(); ++bit, ++idx)
		if (block == &(*bit))
			break;
	if (idx == m_network->m_blocks.size())
		throw std::runtime_error("[SceneManager::removeBlock] Invalid pointer (not in managed network)");
	removeBlock(idx);
}


void SceneManager::removeBlock(unsigned int blockIndex) {
	Q_ASSERT(m_network->m_blocks.size() > blockIndex);
	Q_ASSERT(m_blockItems.count() > (int)blockIndex);

	auto bit = m_network->m_blocks.begin(); std::advance(bit, blockIndex);
	Block * blockToBeRemoved = &(*bit);

	// find connectors that connect to this block
	QSet<Connector*> connectors = m_blockConnectorMap[blockToBeRemoved];
	for (Connector * con : connectors) {
		// find connector to be removed from list
		for (auto cit = m_network->m_connectors.begin(); cit != m_network->m_connectors.end(); ++cit) {
			if (&(*cit) == con) {
				m_network->m_connectors.erase(cit);
				break;
			}
		}
	}

	// now remove the block itself
	BlockItem * bi = m_blockItems[(int)blockIndex];
	m_blockItems.removeAt((int)blockIndex);
	delete bi;

	// finally remove block itself from list
	m_network->m_blocks.erase(bit);

	// and update all connector items; first remove all, then recreate as needed
	qDeleteAll(m_connectorSegmentItems); // will be recreated
	m_connectorSegmentItems.clear();
	m_blockConnectorMap.clear(); // will be recreated
	for (Connector & con : m_network->m_connectors) {
		updateConnectorSegmentItems(con, nullptr);
	}
}


void SceneManager::removeConnector(const Connector * con) {
	size_t idx = 0;
	for (auto cit = m_network->m_connectors.begin(); cit != m_network->m_connectors.end(); ++cit, ++idx)
		if (con == &(*cit))
			break;
	if (idx == m_network->m_connectors.size())
		throw std::runtime_error("[SceneManager::removeConnector] Invalid pointer (not in managed network)");
	removeConnector(idx);
}


void SceneManager::removeConnector(unsigned int connectorIndex) {
	Q_ASSERT(m_network->m_connectors.size() > connectorIndex);

	auto cit = m_network->m_connectors.begin(); std::advance(cit, connectorIndex);
	Connector * conToBeRemoved = &(*cit);

	// find corresponding connector items
	int i=0;
	while (i<m_connectorSegmentItems.count()) {
		if (m_connectorSegmentItems[i]->m_connector == conToBeRemoved) {
			delete m_connectorSegmentItems[i];
			m_connectorSegmentItems.removeAt(i);
			continue;
		}
		++i;
	}

	// remove connector associations in block-connector-map
	for (QMap<const Block*, QSet<Connector*> >::iterator it = m_blockConnectorMap.begin(); it != m_blockConnectorMap.end(); ++it) {
		QSet<Connector*> & conList = it.value();
		conList.remove(conToBeRemoved);
	}

	// finally remove connector at given index
	m_network->m_connectors.erase(cit);

}


// ** protected functions **


void SceneManager::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	if (mouseEvent->button() == Qt::RightButton) {
		disableConnectionMode();
	}
	// in case of click on socket while in connection mode, the next call will
	// result in a call to startSocketConnection(), which creates a new block and connection
	// we need to detect this and fire a mouse event again afterwards, which will cause
	// the newly created block to go into drag-move mode
	bool alreadyInConnectionProcess = !m_network->m_blocks.empty() && (m_network->m_blocks.back().m_name == Globals::InvisibleLabel);
	QGraphicsScene::mousePressEvent(mouseEvent);
	bool inConnectionProcess = !m_network->m_blocks.empty() && (m_network->m_blocks.back().m_name == Globals::InvisibleLabel);
	if (!alreadyInConnectionProcess && inConnectionProcess) {
		// make the last item the mouse grabber objects
		m_blockItems.back()->grabMouse();
	}
}


void SceneManager::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
//	qDebug() << mouseEvent;

	// check if in connection mode and if connection-block position is over an inlet socket that
	// is not yet connected - if so, mark the socket as "hovered" and update it

	if (m_connectionModeEnabled) {
		if (!m_blockItems.isEmpty() && m_blockItems.back()->block()->m_name == Globals::InvisibleLabel) {
			QPointF p = m_blockItems.back()->pos();
			for (BlockItem  * bi : m_blockItems) {
				if (bi->block()->m_name == Globals::InvisibleLabel)
					continue;
				// first un-hover all sockets
				for (SocketItem * si : bi->m_socketItems) {
					si->m_hovered = false;
					si->update();
				}

				// now search for sockets that may be hovered
				SocketItem * si = bi->inletSocketAcceptingConnection(p);
				if (si != nullptr) {
					// only allow hovering for sockets without connection
					if (!isConnectedSocket(bi->block(), si->socket())) {
						si->m_hovered = true;
						si->update();
					}
				}
			}
		}
	}

	QGraphicsScene::mouseMoveEvent(mouseEvent);
}


void SceneManager::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {

	QGraphicsScene::mouseReleaseEvent(mouseEvent);
	if (mouseEvent->button() & Qt::LeftButton) {
		QString startSocket;
		QString targetSocket;

		// check if we have dropped onto a connectable socket
		if (m_connectionModeEnabled) {
			if (!m_blockItems.isEmpty() && m_blockItems.back()->block()->m_name == Globals::InvisibleLabel) {
				QPointF p = m_blockItems.back()->pos();
				for (BlockItem  * bi : m_blockItems) {
					if (bi->block()->m_name == Globals::InvisibleLabel)
						continue;

					// now search for sockets that may be hovered
					SocketItem * si = bi->inletSocketAcceptingConnection(p);
					if (si != nullptr) {
						// only allow hovering for sockets without connection
						if (!isConnectedSocket(bi->block(), si->socket())) {
							// found one - remember this socket and the starting socket for our connection
							startSocket = m_network->m_connectors.back().m_sourceSocket;
							targetSocket = bi->block()->m_name + "." + si->socket()->m_name;
							break;
						}
					}
				}
			}
		}

		disableConnectionMode();

		// now create a new connector
		if (!startSocket.isEmpty() && !targetSocket.isEmpty()) {
			Connector con;
			con.m_name = "new connector";
			con.m_sourceSocket = startSocket;
			con.m_targetSocket = targetSocket;
			m_network->m_connectors.push_back(con);
			m_network->adjustConnector(m_network->m_connectors.back());
			updateConnectorSegmentItems(m_network->m_connectors.back(), nullptr);
			emit newConnectionAdded();
		}
	}
}


BlockItem * SceneManager::createBlockItem(Block & b) {
	BlockItem * item = new BlockItem(&b);
	item->setRect(0,0,b.m_size.width(), b.m_size.height());
	item->setPos(b.m_pos);
	return item;
}


ConnectorSegmentItem * SceneManager::createConnectorItem(Connector & con) {
	ConnectorSegmentItem * item = new ConnectorSegmentItem(&con);
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

		ConnectorSegmentItem * item = createConnectorItem(con);
		item->setLine(startLine);
		item->setFlags(QGraphicsItem::ItemIsSelectable);
		item->m_segmentIdx = -1; // start line
		newConns.append(item);

		item = createConnectorItem(con);
		item->setLine(endLine);
		item->setFlags(QGraphicsItem::ItemIsSelectable);
		item->m_segmentIdx = -2; // end line
		newConns.append(item);

		QPointF start = startLine.p2();
		for (int i=0; i<con.m_segments.count(); ++i) {
			const Connector::Segment & seg = con.m_segments[i];
			item = createConnectorItem(con);
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


void SceneManager::onSelectionChanged() {
	// get newly selected items, and if a connector is part of the selection, select all segments of the same collector,
	// but deselect all others
	QList<QGraphicsItem *> items = QGraphicsScene::selectedItems();

	QList<Connector*> selectedCons;
	for (QGraphicsItem * item : items) {
		ConnectorSegmentItem * segItem = dynamic_cast<ConnectorSegmentItem*>(item);
		if (segItem == nullptr)
			continue;
		if (selectedCons.count(segItem->m_connector) == 0)
			selectedCons.append(segItem->m_connector);
	}

	// if a connector segment was selected, de-select only the last connector and all segments of the same
	if (!selectedCons.isEmpty()) {
//		disconnect(this, &SceneManager::selectionChanged, this, &SceneManager::onSelectionChanged);
		clearSelection();
		// now select all items that belong to the connector
		for (ConnectorSegmentItem * item : m_connectorSegmentItems) {
			if (item->m_connector == selectedCons.back())
				item->setSelected(true);
		}
//		connect(this, &SceneManager::selectionChanged, this, &SceneManager::onSelectionChanged);
	}
}


void SceneManager::blockDoubleClicked(const BlockItem * blockItem) {
	emit blockActionTriggered(blockItem);
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
	// re-create entire connector if all pointers are emtpy
	if (startSegment == nullptr && endSegment == nullptr && segmentItems.isEmpty()) {
		QList<ConnectorSegmentItem *> newConns = createConnectorItems(const_cast<Connector&>(con)); // const-cast is safe here, since we only expect connector objects that we own ourselves
		for( BLOCKMOD::ConnectorSegmentItem * item : newConns) {
			addItem(item);
			m_connectorSegmentItems.append(item);
//			qDebug() << item << " : " << item->m_connector << " : " << item->m_segmentIdx << " : " << item->line();
		}
		return;
	}
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
		ConnectorSegmentItem * item = createConnectorItem(const_cast<Connector&>(con)); // need to get write access for connector in newly created item
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
