#ifndef BM_SOCKETITEM_H
#define BM_SOCKETITEM_H

#include <QGraphicsItem>

namespace BLOCKMOD {

class Socket;
class BlockItem;

/*! Base class for sockets to be painted on a block item. */
class SocketItem : public QGraphicsItem {
public:
	/*! Constructor, takes a pointer to the associated socket data structure (which
		must have a lifetime longer than the graphics item.
	*/
	explicit SocketItem(BlockItem * parent, Socket * socket);

	QRectF boundingRect() const override;

protected:
	/*! Re-implemented to draw the styled rectangle of the block. */
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:

	/*! Pointer to the socket data structure. */
	Socket	*m_socket;

	/*! The bounding rectangle of the symbol (updated whenever content of the socket changes). */
	QRectF	m_symbolRect;
};

} // namespace BLOCKMOD


#endif // BM_SOCKETITEM_H
