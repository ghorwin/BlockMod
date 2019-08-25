#ifndef BM_ConnectorH
#define BM_ConnectorH

#include <QVector>
#include <QPointF>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "BM_Entity.h"

namespace BLOCKMOD {

/*! Stores properties of a Connector.
*/
class Connector : public Entity {
public:

	/*! Reads content of the block from XML stream. */
	void readXML(QXmlStreamReader & reader);

	/*! Dumps out content of block to stream writer. */
	void writeXML(QXmlStreamWriter & writer) const;

	/*! Points of polygon, always from out (sourceSocket) to in (targetSocket). */
	QVector<QPointF>	m_points;

	/*! ID of socket that polygon originates from, empty if not assigned. */
	QString			m_sourceSocket;
	/*! ID of socket that polygon ends in, empty if not assigned. */
	QString			m_targetSocket;

};

} // namespace BLOCKMOD


#endif // BM_ConnectorH

