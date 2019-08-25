#ifndef BLOCKITEM_H
#define BLOCKITEM_H

#include <QGraphicsRectItem>

namespace BLOCKMOD {

class Block;

/*! A graphics item that represents a block. */
class BlockItem : public QGraphicsRectItem {
public:
	explicit BlockItem(QObject *parent = nullptr);

//	QRectF boundingRect() const;

//	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	/*! Pointer to associated block. */
	Block	*m_block;
};

} // namespace BLOCKMOD


#endif // BLOCKITEM_H
