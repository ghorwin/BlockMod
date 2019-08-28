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
#include <QDebug>
#include <QApplication>

#include <iostream>

#include "BM_Network.h"
#include "BM_BlockItem.h"
#include "BM_ConnectorSegmentItem.h"
#include "BM_Globals.h"

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
	m_blockItems.clear();
	qDeleteAll(m_connectorSegmentItems);
	m_connectorSegmentItems.clear();

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
	QApplication::setOverrideCursor(Qt::ArrowCursor);
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
