#ifndef BM_BlockH
#define BM_BlockH

#include <QList>
#include <QString>
#include <QPointF>
#include <QSizeF>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "BM_Socket.h"
#include "BM_Entity.h"

namespace BLOCKMOD {

/*! Stores properties of a block.

	* appearance properties of block
	* position of block
*/
class Block : public Entity {
public:
	Block() {}

	Block(const QString & name);
	Block(const QString & name, double x, double y);


	/*! Reads content of the block from XML stream. */
	void readXML(QXmlStreamReader & reader);

	/*! Dumps out content of block to stream writer. */
	void writeXML(QXmlStreamWriter & writer) const;

	/*! Position (top left corner) of block. */
	QPointF			m_pos;

	/*! Sockets that belong to this block. */
	QList<Socket>	m_sockets;

	// Identifier for theme'd appearance

	// Properties for custom appearance

	/*! Size of block. */
	QSizeF			m_size;
};

} // namespace BLOCKMOD


#endif // BM_BlockH

