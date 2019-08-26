#ifndef BLOCKITEM_H
#define BLOCKITEM_H

#include <QGraphicsRectItem>

namespace BLOCKMOD {

class Block;

/*! A graphics item that represents a block. */
class BlockItem : public QGraphicsRectItem {
public:
	/*! Constructor, takes a pointer to the associated block data structure (which
		must have a lifetime longer than the graphics item.
	*/
	explicit BlockItem(Block * b);

//	QRectF boundingRect() const;

protected:
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

	friend class SceneManager;
};

} // namespace BLOCKMOD


#endif // BLOCKITEM_H
