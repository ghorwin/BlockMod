/*	BSD 3-Clause License

	This file is part of the BlockMod Library.

	Copyright (c) 2019, Andreas Nicolai
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	   contributors may be used to endorse or promote products derived from
	   this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

	/*! Efficient swap function. */
	void swap(Network & other);

	/*! Reads network from file. */
	void readXML(const QString & fname);
	/*! Writes network to file. */
	void writeXML(const QString & fname) const;
	/*! Flattens all ID names of sockets and blocks and checks for duplicates. */
	void checkNames() const;

	/*! Tests, if the network has a block with a socket, both identified by socketVariableName in format
		"<block>.<socketName>" and if this socket is an inlet socket or outlet socket (depending on inletSocket parameter).
		Returns true, if such a socket exists.
	*/
	bool haveSocket(const QString & socketVariableName, bool inletSocket) const;



	/*! Processes all connectors and updates their segments so that start/end sockets are connected. */
	void adjustConnectors();

	/*! Processes a single connector and adjusts the connection segments.
		Function first checks if blocks and sockets match using <block-name>.<socket-name> referencing.
	*/
	void adjustConnector(Connector & con);

	/*! Searches block and socket data structure by flat variable name. */
	void lookupBlockAndSocket(const QString & flatName, const Block * &block, const Socket * &socket) const;

	/*! Removes block at given index and all associated connectors.
		\warning Invalidates all block and connector pointers!
	*/
	void removeBlock(unsigned int blockIdx);

	/*! Renames a single block. */
	void renameBlock(unsigned int blockIdx, const QString & newName);


	// *** member variables ***

	/*! List of all blocks in the network.
		\note Cannot use a QList here, because we maintain persistent pointers to block objects
			and QList's copy-on-write functionality breaks these persistent pointers.
			We must use a list here, so that when we add a block during the socket-connect operation
			(the invisible block) the existing nodes are not invalidated.
	*/
	std::list<Block>		m_blocks;

	/*! List of all connectors in the network.
		Connectors are always associated with sockets (referenced via
		block-id and socket-id).
	*/
	std::list<Connector>	m_connectors;


	// *** static functions ***

	/*! Takes a flat name of format <block-name>.<socket-name> and extracts both parts.
		Throws an exception if . is missing or either name is an empty string.
	*/
	static void splitFlatName(const QString & flatVariableName, QString & blockName, QString & socketName);

private:

	void readBlocks(QXmlStreamReader & reader);
};

} // namespace BLOCKMOD

#endif // BM_NetworkH
