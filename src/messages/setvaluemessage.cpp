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

#include "setvaluemessage.h"

#include <QDataStream>

SetValueMessage::SetValueMessage() {

}

SetValueMessage::SetValueMessage(quint8 x, quint8 y, quint8 value, QUuid valueOwner) :
    m_x(x), m_y(y), m_value(value), m_valueOwner(valueOwner) {

}

bool SetValueMessage::parseStream(QDataStream &dataStream) {
    dataStream >> m_x;
    dataStream >> m_y;
    dataStream >> m_valueOwner;
    dataStream >> m_value;

    return dataStream.status() == QDataStream::Ok;
}

bool SetValueMessage::writeStream(QDataStream &dataStream) {
    QByteArray buffer;
    QDataStream tempStream(&buffer, QIODevice::WriteOnly);

    quint16 messageType = Message::SetValueMessage;

    tempStream << messageType;
    tempStream << m_x;
    tempStream << m_y;
    tempStream << m_valueOwner;
    tempStream << m_value;

    quint16 length = buffer.size() + 2;

    dataStream << length;
    dataStream.writeRawData(buffer.constData(), buffer.size());

    return dataStream.status() == QDataStream::Ok;
}
