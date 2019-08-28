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
		Segment() :
			m_direction(Qt::Horizontal),
			m_offset(0)
		{}
		Segment(Qt::Orientation direction, double offset) :
			m_direction(direction),
			m_offset(offset)
		{}

		/*! Reads content of the segment from XML stream. */
		void readXML(QXmlStreamReader & reader);

		/*! Dumps out content of segment to stream writer. */
		void writeXML(QXmlStreamWriter & writer) const;

		Qt::Orientation m_direction;
		double m_offset;
	};

	/*! Reads content of the block from XML stream. */
	void readXML(QXmlStreamReader & reader);

	/*! Dumps out content of block to stream writer. */
	void writeXML(QXmlStreamWriter & writer) const;

	/*! Unique identification name of this connector instance. */
	QString						m_name;

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
