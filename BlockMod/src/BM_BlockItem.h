#ifndef BLOCKITEM_H
#define BLOCKITEM_H

#include <QGraphicsItem>

#include <BM_Block.h>

namespace BLOCKMOD {

class BlockItem : public QGraphicsItem {
public:
	explicit BlockItem(QObject *parent = 0);

	/*! Creates an internal copy of the Block meta data and customizes
		the graphics item.
	*/
	void setFromBlock(const Block & b);


	QRectF boundingRect() const;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


private:
	Block	m_block; // the block that we draw our data from

	/*! Our own sockets. */
	QList<QGraphicsItem*>	m_socketItems;
};

} // namespace BLOCKMOD


#endif // BLOCKITEM_H
