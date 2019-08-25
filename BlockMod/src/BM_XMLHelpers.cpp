#include "BM_XMLHelpers.h"

#include <QXmlStreamReader>
#include <QCoreApplication>
#include <QStringList>

namespace BLOCKMOD {

// helper function for reading XML file
void readUnknownElement(QXmlStreamReader & reader) {
	if (reader.error() != QXmlStreamReader::NoError) return;
	Q_ASSERT(reader.isStartElement());

	while (!reader.atEnd()) {
		reader.readNext();
		if (reader.isEndElement())
			break;

		if (reader.isStartElement())
			readUnknownElement(reader);
	}
}

void readUntilEndElement(QXmlStreamReader & reader) {
	if (reader.error() != QXmlStreamReader::NoError) return;
	while (!reader.atEnd()) {
		reader.readNext();
		if (reader.isEndElement()) break;
		if (reader.isStartElement()) {
			readUnknownElement(reader);
		}
	}
}


void readDoubleAttribute(QXmlStreamReader & reader, const QString & name, double & val, bool optional) {
	if (reader.error() != QXmlStreamReader::NoError) return;
	bool ok;
	double tmp_val;
	QString str = reader.attributes().value(name).toString();
	tmp_val = str.toDouble(&ok);
	if (!ok) {
		if (!optional)
			reader.raiseError(qApp->translate("XMLHelpers", "Invalid value for attribute '%1'.").arg(name) );
	}
	else {
		val = tmp_val;
	}
}


void readIntAttribute(QXmlStreamReader & reader, const QString & name, int & val, bool optional) {
	if (reader.error() != QXmlStreamReader::NoError) return;
	bool ok;
	QString str = reader.attributes().value(name).toString();
	val = str.toInt(&ok);
	if (!ok) {
		if (!optional)
			reader.raiseError(qApp->translate("XMLHelpers", "Invalid value for attribute '%1'.").arg(name) );
	}
	else {
		val = 0;
	}
}


void readBoolAttribute(QXmlStreamReader & reader, const QString & name, bool & val, bool optional) {
	if (reader.error() != QXmlStreamReader::NoError) return;
	bool ok;
	bool tmp_val;
	QString str = reader.attributes().value(name).toString();
	tmp_val = str.toInt(&ok);
	if (!ok) {
		if (!optional)
			reader.raiseError(qApp->translate("XMLHelpers", "Invalid value for attribute '%1'.").arg(name) );
	}
	else {
		val = tmp_val;
	}
}


QString readTextElement(QXmlStreamReader & reader) {
	if (reader.error() != QXmlStreamReader::NoError)
		return QString();
	Q_ASSERT(reader.isStartElement());
	return reader.readElementText();
}


void readNamedDouble(QXmlStreamReader & reader, QString & name, double & val) {
	if (reader.error() != QXmlStreamReader::NoError) return;
	name = reader.attributes().value("name").toString();
	QString data = reader.readElementText();
	bool ok;
	val = data.toDouble(&ok);
	if (!ok) {
		reader.raiseError(qApp->translate("XMLHelpers", "Invalid value '%2' for Double element '%1'.").arg(name).arg(data) );
	}
}


void readNamedInteger(QXmlStreamReader & reader, QString & name, int & val) {
	if (reader.error() != QXmlStreamReader::NoError) return;
	name = reader.attributes().value("name").toString();
	QString data = reader.readElementText();
	bool ok;
	val = data.toInt(&ok);
	if (!ok) {
		reader.raiseError(qApp->translate("XMLHelpers", "Invalid value '%2' for Integer element '%1'.").arg(name).arg(data) );
	}
}


void readNamedString(QXmlStreamReader & reader, QString & name, QString & val) {
	if (reader.error() != QXmlStreamReader::NoError) return;
	name = reader.attributes().value("name").toString();
	val = reader.readElementText();
}


QString encodePoint(const QPointF & p) {
	return QString("%1, %2").arg(p.x()).arg(p.y());
}


QPointF decodePoint(const QString & pStr) {
	QStringList p = pStr.split(",");
	if (p.count() != 2)
		throw std::runtime_error("Invalid format of encoded point.");
	QPointF pos;
	pos.setX( p[0].toDouble() );
	pos.setY( p[1].toDouble() );
	return pos;
}

} // namespace BLOCKMOD
