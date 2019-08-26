#ifndef BM_SocketH
#define BM_SocketH

#include <QList>
#include <QPointF>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace BLOCKMOD {

/*! Stores properties of a Socket.

	* type: in/out
	* orientation: north, east, west, south

	Sockets are owned by parent block and do not exist in separate socket list.
*/
class Socket {
public:
	/*! Default C'tor. */
	Socket() :
		m_pos(0,0),
		m_orientation(Qt::Horizontal),
		m_inlet(false)
	{
	}

	/*! C'tor, creating named default socket. */
	Socket(const QString & name) :
		m_name(name),
		m_pos(0,0),
		m_orientation(Qt::Horizontal),
		m_inlet(false)
	{
	}

	/*! C'tor, initializes all members. */
	Socket(const QString & name, const QPointF & pos, Qt::Orientation orientation, bool inlet) :
		m_name(name),
		m_pos(pos),
		m_orientation(orientation),
		m_inlet(inlet)
	{
	}

	/*! Reads content of the block from XML stream. */
	void readXML(QXmlStreamReader & reader);

	/*! Dumps out content of block to stream writer. */
	void writeXML(QXmlStreamWriter & writer) const;

	QString			m_name;

	/*! Position (connection point) of Socket.
		Relative to parent block.
	*/
	QPointF			m_pos;

	/*! Orientation defines together with the position relative to the parent block, which
		direction the block points to and how to compute the first connector start coordinate.

		Example: m_orientation = Qt::Horizontal and m_pos.x() != 0 --> connector at the right side of block pointing to the right
	*/
	Qt::Orientation	m_orientation;

	/*! If true, painted as a socket, if false, painted as an outgoing arrow. */
	bool			m_inlet;
};

} // namespace BLOCKMOD


#endif // BM_SocketH

