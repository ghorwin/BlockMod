#ifndef BM_BlockH
#define BM_BlockH

#include <QList>
#include <QString>
#include <QPointF>
#include <QSizeF>
#include <QLineF>
#include <QMap>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "BM_Socket.h"

namespace BLOCKMOD {

/*! Stores properties of a block.
	* appearance properties of block
	* position of block
	* sockets
	* custom properties in the property map
*/
class Block {
public:
	Block() {}

	Block(const QString & name);
	Block(const QString & name, double x, double y);

	/*! Reads content of the block from XML stream. */
	void readXML(QXmlStreamReader & reader);

	/*! Dumps out content of block to stream writer. */
	void writeXML(QXmlStreamWriter & writer) const;

	/*! Generate connection line between socket and point, where first connector segment starts.
		Returned coordinates are in scene-coordinates.
	*/
	QLineF socketStartLine(const Socket * socket) const;

	/*! Unique identification name of this block instance. */
	QString						m_name;

	/*! Position (top left corner) of block. */
	QPointF						m_pos;

	/*! Sockets that belong to this block. */
	QList<Socket>				m_sockets;

	/*! Size of block. */
	QSizeF						m_size;

	/*! Custom properties. */
	QMap<QString, QVariant>		m_properties;
};

} // namespace BLOCKMOD


#endif // BM_BlockH

