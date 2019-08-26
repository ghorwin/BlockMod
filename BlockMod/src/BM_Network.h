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

	/*! Reads network from file. */
	void readXML(const QString & fname);
	/*! Writes network to file. */
	void writeXML(const QString & fname) const;
	/*! Flattens all ID names of sockets and blocks and checks for duplicates. */
	void checkNames() const;

	/*! Processes all connectors and updates their segments so that start/end sockets are connected. */
	void adjustConnectors();

	/*! Processes a single connector and adjusts the connection segments.
		Function first checks if blocks and sockets match using <block-name>.<socket-name> referencing.
	*/
	void adjustConnector(Connector & con);

	/*! Searches block and socket data structure by flat variable name. */
	void lookupBlockAndSocket(const QString & flatName, const Block * &block, const Socket * &socket) const;

	/*! List of all blocks in the network. */
	QList<Block>		m_blocks;

	/*! List of all connectors in the network.
		Connectors are always associated with sockets (referenced via
		block-id and socket-id).
	*/
	QList<Connector>	m_connectors;

	/*! Takes a flat name of format <block-name>.<socket-name> and extracts both parts.
		Throws an exception if . is missing or either name is an empty string.
	*/
	static void splitFlatName(const QString & flatVariableName, QString & blockName, QString & socketName);

private:

	void readBlocks(QXmlStreamReader & reader);
};

} // namespace BLOCKMOD

#endif // BM_NetworkH
