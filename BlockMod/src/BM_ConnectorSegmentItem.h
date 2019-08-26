#ifndef BM_CONNECTORSEGMENTITEM_H
#define BM_CONNECTORSEGMENTITEM_H

#include <QGraphicsLineItem>

namespace BLOCKMOD {

class Connector;

class ConnectorSegmentItem : public QGraphicsLineItem {
public:
	explicit ConnectorSegmentItem(Connector * connector);

	/*! The connector, that this line segment belongs to. */
	Connector	*m_connector;

	/*! The index of the segment this item corresponds to.
		- -1 means start segment
		- -2 means end segment
		- 0...n corresponds to the index in m_connector->m_segments[]
	*/
	int			m_segmentIdx;

};

} // namespace BLOCKMOD


#endif // BM_CONNECTORSEGMENTITEM_H
