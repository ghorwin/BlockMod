#ifndef BM_SocketH
#define BM_SocketH

#include <QList>
#include <QPointF>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "BM_Entity.h"

namespace BLOCKMOD {

/*! Stores properties of a Socket.

	* type: in/out
	* orientation: north, east, west, south

	Sockets are owned by parent block and do not exist in separate socket list.
*/
class Socket : public Entity {
public:
	Socket() {}

	Socket(const QString & name);

	/*! Reads content of the block from XML stream. */
	void readXML(QXmlStreamReader & reader);

	/*! Dumps out content of block to stream writer. */
	void writeXML(QXmlStreamWriter & writer) const;

	/*! Position (connection point) of Socket.
		Relative to parent block.
	*/
	QPointF			m_pos;

	Qt::Orientation	m_orientation;
	bool			m_inlet;
};

} // namespace BLOCKMOD


#endif // BM_SocketH

