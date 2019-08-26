#ifndef BLOCKITEM_H
#define BLOCKITEM_H

#include <QGraphicsRectItem>

namespace BLOCKMOD {

class Block;
class SocketItem;

/*! A graphics item that represents a block. */
class BlockItem : public QGraphicsRectItem {
public:
	/*! Constructor, takes a pointer to the associated block data structure (which
		must have a lifetime longer than the graphics item.
	*/
	explicit BlockItem(Block * b);


protected:
	/*! This function is called from the constructor and creates child socket items. */
	virtual void createSocketItems();

	/*! Re-implemented to draw the styled rectangle of the block. */
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	/*! Re-implemented to reset the m_moved flag. */
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

	/*! Overloaded to react on block move.
		Implements the snap-to-grid functionality, and updates attached connectors. */
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value) override;


private:
	/*! Pointer to associated block. */
	Block		*m_block;

	/*! Indicates, that the block has been moved. */
	bool		m_moved;

	/*! Our socket items, childs of this block item. */
	QList<SocketItem*> m_socketItems;

	friend class SceneManager;
};

} // namespace BLOCKMOD


#endif // BLOCKITEM_H
