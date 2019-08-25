#include "BM_Network.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QSet>
#include <QDebug>

#include <stdexcept>

#include "BM_Block.h"
#include "BM_XMLHelpers.h"

namespace BLOCKMOD {

Network::Network()
{
}


Network::~Network() {
}




void Network::readXML(const QString & fname) {
	QFile xmlFile(fname);
	if (!xmlFile.open(QIODevice::ReadOnly | QFile::Text))
		throw std::runtime_error("Cannot read file.");

	QXmlStreamReader reader(&xmlFile);

	// we start reading the XML
	while (!reader.atEnd() && !reader.hasError()) {
		reader.readNext();
		if (reader.isStartElement()) {
			// first element tag must be the BlockMod
			QString ename = reader.name().toString();
			//QString ver = reader.attributes().value("Version").toString();
			if (ename != "BlockMod" ) {
				reader.raiseError("Expected BlockMod XML tag.");
				break;
			}

			// keep reading until next start element is either Blocks, Sockets or Connectors
			while (!reader.atEnd() && !reader.hasError()) {
				reader.readNext();
				if (reader.isStartElement()) {
					QString sectionName = reader.name().toString();
					if (sectionName == "Blocks")
						readList(reader, m_blocks);
					else if (sectionName == "Connectors")
						readList(reader, m_connectors);
					else {
						reader.raiseError( QString("Unknown tag '%1'.").arg(sectionName));
						break;
					}
				}
				else if (reader.isEndElement()) {
					QString sectionName = reader.name().toString();
					Q_ASSERT(sectionName == "BlockMod");
				}
			}
		}
	}

	if (reader.hasError()) {
		throw std::runtime_error( reader.errorString().toStdString() );
	}

}


void Network::writeXML(const QString & fname) const {
	QFile xmlFile(fname);
	if (!xmlFile.open(QIODevice::WriteOnly | QFile::Truncate | QFile::Text))
		throw std::runtime_error("Cannot create output file.");
	QXmlStreamWriter stream(&xmlFile);
	stream.setAutoFormatting(true);
	stream.setAutoFormattingIndent(-1);
	stream.writeStartDocument();

	stream.writeStartElement("BlockMod");

	if (!m_blocks.isEmpty()) {
		stream.writeComment("Blocks");
		stream.writeStartElement("Blocks");
		for (int i=0; i<m_blocks.count(); ++i)
			m_blocks[i].writeXML(stream);

		stream.writeEndElement(); // Blocks
	}

	if (!m_connectors.isEmpty()) {
		stream.writeComment("Connectors");
		stream.writeStartElement("Connectors");
		for (int i=0; i<m_connectors.count(); ++i)
			m_connectors[i].writeXML(stream);

		stream.writeEndElement(); // Connectors
	}


	stream.writeEndElement(); // BlockMod

	stream.writeEndDocument();
}



void Network::checkNames() const {
	QSet<QString> blockNames;
	QSet<QString> socketNames;
	for (int i=0; i<m_blocks.count(); ++i) {
		QString bName = m_blocks[i].m_name;
		if (bName.indexOf(".") != -1) {
			qDebug() << QString("Invalid Block ID name '%1'").arg(bName);
		}
		if (blockNames.contains(bName)) {
			qDebug() << QString("Duplicate Block ID name '%1'").arg(bName);
		}
		blockNames.insert(bName);
		qDebug() << bName;
		// now all sockets
		for (int j=0; j<m_blocks[i].m_sockets.count(); ++j) {
			QString sName = m_blocks[i].m_sockets[j].m_name;
			if (sName.indexOf(".") != -1) {
				qDebug() << QString("Invalid Socket ID name '%1'").arg(sName);
			}
			// get flattened name
			sName = bName + "." + sName;
			if (socketNames.contains(sName)) {
				qDebug() << QString("Duplicate Socket ID name '%1'").arg(sName);
			}
			socketNames.insert(sName);
			qDebug() << sName;
		}
	}
}

} // namespace BLOCKMOD
