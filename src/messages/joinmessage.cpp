/**
    This file is part of Sudoku United.

    Sudoku United is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Sudoku United is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sudoku United.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "joinmessage.h"

#include <QDataStream>

JoinMessage::JoinMessage() {

}

JoinMessage::JoinMessage(QUuid uuid, QString name) :
    m_name(name), m_uuid(uuid) {

}


bool JoinMessage::parseStream(QDataStream &dataStream) {
    m_name = readString(dataStream);
    m_uuid = readUuid(dataStream);

    return dataStream.status() == QDataStream::Ok;
}

bool JoinMessage::writeStream(QDataStream &dataStream) {
    QByteArray buffer;
    QDataStream tempStream(&buffer, QIODevice::WriteOnly);

    quint16 messageType = Message::JoinMessage;

    tempStream << messageType;
    writeString(tempStream, m_name);
    writeUuid(tempStream, m_uuid);

    quint16 length = buffer.size() + 2;

    dataStream << length;
    dataStream.writeRawData(buffer.constData(), buffer.size());

    return dataStream.status() == QDataStream::Ok;
}
