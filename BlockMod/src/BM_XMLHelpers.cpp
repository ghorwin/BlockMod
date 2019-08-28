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
