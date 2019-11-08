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

#ifndef BM_SceneManagerH
#define BM_SceneManagerH

#include <QGraphicsScene>
#include <QMap>

class QGraphicsItem;

namespace BLOCKMOD {

class Network;
class Block;
class BlockItem;
class Socket;
class SocketItem;
class Connector;
class ConnectorSegmentItem;

/*! The graphics scene that visualizes the network. */
class SceneManager : public QGraphicsScene {
	Q_OBJECT
public:
	explicit SceneManager(QObject *parent = nullptr);

	/*! D-tor. */
	virtual ~SceneManager() override;

	/*! Set a new network (a local copy is made of the network object).
		This will recreate the entire scene.
	*/
	void setNetwork(const Network & network);

	/*! Provide read-only access to the network data structure.
		\note This data structure is internally used and modified by user actions.
		So, whenever a change signal is emitted, this network contains
		already the changes. If you need to create UNDO actions based on the previous
		state of the network, you must keep a separate copy of the network.
	*/
	const Network & network() const;


	// query functions

	/*! Looks up the block item with a block that has the given name. */
	const BlockItem * blockItemByName(const QString & blockName) const;


	// Functions called from blocks/items to adjust the network due to user interaction

	/*! Called from BlockItem when a block was moved to signal the scene manager
		to adjust the connected connectors.
	*/
	void blockMoved(const Block * block, const QPointF oldPos);

	/*! Calls from BlockItem when a block was moved.
		Results in blockSelected(blockName) to be emitted.
	*/
	void blockSelected(const Block * block);

	/*! Called from ConnectorSegmentItem when a segment was moved to signal the scene manager
		to adjust the connected connectors.
		\param currentItem Pointer to the item currently being moved. It's socket index is
			expected to be already correctly set including possible inserted new segments.

		This function creates new segment items as needed and removes no longer needed segments
		(after a segment merge, for example).
	*/
	void connectorSegmentMoved(ConnectorSegmentItem * currentItem);

	/*! Toggles high-lighting of connector segments. */
	void highlightConnectorSegments(const Connector & con, bool highlighted);

	/*! This will select all not yet selected segments of this connector.
		\note Item change flag will be turned off for all touched items, so that
		this function is not called several times.
	*/
	void selectConnectorSegments(const Connector & con);

	/*! Connected to selectionChanged() signal. */
	void onSelectionChanged();

	/*! Called from a block item when a block has been double-clicked, emits signal blockActionTriggered()  */
	void blockDoubleClicked(const BlockItem * blockItem);

	/*! This function removes line segments with 0 offset and merges the neighboring segments into one.
		Must not be called from within a move operation (only, for example, from mouse-release event handlers).
	*/
	void mergeConnectorSegments(Connector & con);

	/*! Quick test if a socket is connected anywhere by a connector. */
	bool isConnectedSocket(const Block * b, const Socket * s) const;


	// Functions related to connection mode

	/*! Puts the scene into connection mode.
		This turns on the hover-effect on outlet sockets which now start a connection upon click.
		Also, block and connector movement is disabled.
	*/
	void enableConnectionMode();

	/*! Turns of the connection mode. */
	void disableConnectionMode();

	/*! Returns true, if scene is currently in connection mode. */
	bool isConnectionModeEnabled() const { return m_connectionModeEnabled; }

	/*! Called from a socket item, so that the scene is put into connection mode.
		This means:
		- a virtual (invisible) block is created with a single inlet connector, the start line
			of this invisible block has zero length.
		- a connector between the virtual block and the current socket is created
		- all unconnected inlet sockets are marked as highlightable
		- all outlet sockets are marked as not highlightable
	*/
	void startSocketConnection(const SocketItem & outletSocketItem, const QPointF & mousePos);


	// functions to query current selection

	/*! There can be several currently selected blocks.
		\return Returns a list of selected blocks or an empty list, if none is selected.
	*/
	QList<const Block*> selectedBlocks() const;

	/*! There can be only one selected connector.
		\return Returns a pointer to the selected connector or a nullptr, if none is selected.
	*/
	const Connector * selectedConnector() const;


	// functions to modify managed network

	/*! Adds a new block to the network.
		The block is copied into the network and shown at the given coordinates.
	*/
	void addBlock(const Block & block);

	/*! Adds a new connector.
		The source and target sockets must match existing blocks/sockets in the network.
		Otherwise an exception is thrown.
	*/
	void addConnector(const Connector & con);

	/*! Removes the block by giving a pointer to the block.
		Block must be stored in the network's block list.
		Also removes any connections made to this block.
	*/
	void removeBlock(const Block * block);

	/*! Removes the block at the given index in the network's block list.
		Index must be a valid, otherwise an exception is thrown.
		Also removes any connections made to this block.
	*/
	void removeBlock(unsigned int blockIndex);

	/*! Removes connector by giving a pointer to a connector in the managed network.
		Connector must be stored in the network's connector list.
	*/
	void removeConnector(const Connector * con);

	/*! Removes connector with given index.
		Index must be a valid, otherwise an exception is thrown.
	*/
	void removeConnector(unsigned int connectorIndex);



signals:
	/*! Emitted when a new connection was made and a connector was added.
		The new connector is added to the end of the connectors in the network.
	*/
	void newConnectionAdded();

	/*! Emitted whenever a block action was triggered (usually by double-clicking on the block). */
	void blockActionTriggered(const BlockItem * blockItem);

	/*! Emitted, when a block was selected. */
	void newBlockSelected(const QString & blockName);

	/*! Emitted when a block or connector has been moved. */
	void networkGeometryChanged();

protected:
	/*! Listens for right-mouse-button clicks that turn off connection mode. */
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

	/*! When mouse is released, check if we are hovering over an inlet socket. If yet, make a new connection, otherwise
		just leave connection mode.
	*/
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

	/*! Listens for right-mouse-button clicks that turn off connection mode. */
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

	/*! Create a graphics item based on the data in the given block.
		You can override this method and create your own graphics items, derived from
		base class BlockItem (which contains all the move/selection logic).
	*/
	virtual BlockItem * createBlockItem(Block & b);

	/*! Create the graphics item for a single connector line segment.
		You can override this method and create your own graphics items, derived from
		base class ConnectorSegmentItem (which contains all the move/selection logic).
	*/
	virtual ConnectorSegmentItem * createConnectorItem(Connector & con);

	/*! A single connect yields actually several line segments, which are created here.
		You can override this method and create your own graphics items, derived from
		base class ConnectorSegmentItem (which contains all the move/selection logic).
		\note This function calls createConnectorItem() internally, so it may be sufficient to
		replace the logic there.
	*/
	virtual QList<ConnectorSegmentItem *> createConnectorItems(Connector & con);


private:
	/*! Looks up all segment items belonging to this connector and updates
		their coordinates.
		Adds/removes segment items as necessary and updates m_connectorSegmentItems accordingly.

		\param con Connector to sync with connector items
		\param currentItem Pointer to currently moved item. May be a nullptr, in which case the
			first segment is treated as current item.
	*/
	void updateConnectorSegmentItems(const Connector & con, ConnectorSegmentItem * currentItem);

	/*! The network that we own and manage. */
	Network							*m_network;

	/*! The block-graphics items that we show on the scene. */
	QList<BlockItem*>				m_blockItems;

	/*! The connector-graphics items that we show on the scene. */
	QList<ConnectorSegmentItem*>	m_connectorSegmentItems;

	/*! Map to speed up lookup of connectors connected to a block.
		This map is initialized in createConnectorItems() and updated, whenever a connection is
		made/removed.
	*/
	QMap<const Block*, QSet<Connector*> >	m_blockConnectorMap;

	/*! If true, the scene is in connection mode and dragging of connectors/blocks is disabled. */
	bool							m_connectionModeEnabled;

};

} // namespace BLOCKMOD

#endif // BM_SceneManagerH
