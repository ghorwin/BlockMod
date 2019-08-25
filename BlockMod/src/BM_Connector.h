#ifndef BM_ConnectorH
#define BM_ConnectorH

#include <QVector>
#include <QPointF>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace BLOCKMOD {

/*! Stores properties of a Connector.
*/
class Connector {
public:

	/*! Defines a line segment, can be either vertical or horizontal.
		Stored is only the offset, should be a multiple of the global grid spacing.
	*/
	struct Segment {
		Qt::Orientation m_direction;
		double offset;
	};

	/*! Reads content of the block from XML stream. */
	void readXML(QXmlStreamReader & reader);

	/*! Dumps out content of block to stream writer. */
	void writeXML(QXmlStreamWriter & writer) const;



	/*! Holds always at least one horizontal and one vertical segment.
		A segment of size 0 is ignored when generating the polygon.

		The connector line starts at the socket's connector point (a little
		outside the block), which is defined by the socket's position and
		orientation with respect to the parent block.
	*/
	QList<Segment> m_segments;

	/*! ID of socket that polygon originates from, empty if not assigned.
		Format <block-name>.<socket-name>
	*/
	QString			m_sourceSocket;
	/*! ID of socket that polygon ends in, empty if not assigned. */
	QString			m_targetSocket;

};

} // namespace BLOCKMOD


#endif // BM_ConnectorH

