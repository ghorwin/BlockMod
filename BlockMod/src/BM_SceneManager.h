#ifndef BM_SceneManagerH
#define BM_SceneManagerH

#include <QGraphicsScene>
#include <QMap>

class QGraphicsItem;

namespace BLOCKMOD {

class Network;
class Block;
class BlockItem;
class Connector;
class ConnectorSegmentItem;

/*! The graphics scene that visualizes the network. */
class SceneManager : public QGraphicsScene {
	Q_OBJECT
public:
	explicit SceneManager(QObject *parent = nullptr);

	/*! D-tor. */
	virtual ~SceneManager();

	/*! Set a new network (a local copy is made of the network object).
		This will recreate the entire scene.
	*/
	void setNetwork(const Network & network);

	/*! Provide read-only access to the network data structure. */
	const Network & network() const;

	/*! Called from BlockItem when a block was moved to signal the scene manager
		to adjust the connected connectors.
	*/
	void blockMoved(const Block * block);

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

	/*! This function removes line segments with 0 offset and merges the neighboring segments into one.
		Must not be called from within a move operation (only, for example, from mouse-release event handlers).
	*/
	void mergeConnectorSegments(Connector & con);

protected:

	/*! Create a graphics item based on the data in the given block.
		You can override this method and create your own graphics items, derived from
		base class BlockItem (which contains all the move/selection logic).
	*/
	virtual BlockItem * createBlockItem(Block & b);

	/*! A single connect yields actually several line segments, which are created here.
		You can override this method and create your own graphics items, derived from
		base class ConnectorSegmentItem (which contains all the move/selection logic).
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
};

} // namespace BLOCKMOD

#endif // BM_SceneManagerH
