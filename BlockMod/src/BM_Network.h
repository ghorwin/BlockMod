#ifndef BM_NetworkH
#define BM_NetworkH

#include <QList>

#include <BM_Block.h>
#include <BM_Socket.h>
#include <BM_Connector.h>

class QXmlStreamReader;

namespace BLOCKMOD {

/*! Holds data of connected block network.
	The network acts as the 'project' data structure that owns all other entities of the network.

	You can derive this class to implement/add own meta data for the network.
*/
class Network {
public:
	/*! Default C'tor. */
	Network();
	/*! D'tor, cleanup of allocated resources. */
	~Network();

	/*! Reads network from file. */
	void readXML(const QString & fname);
	/*! Writes network to file. */
	void writeXML(const QString & fname) const;

	/*! Flattens all ID names of sockets and blocks and checks for duplicates. */
	void checkNames() const;

	/*! List of all blocks in the network. */
	QList<Block>		m_blocks;

	/*! List of all connectors in the network. */
	QList<Connector>	m_connectors;

private:
	void readBlocks(QXmlStreamReader & reader);
	void readSockets(QXmlStreamReader & reader);

};

} // namespace BLOCKMOD

#endif // BM_NetworkH
