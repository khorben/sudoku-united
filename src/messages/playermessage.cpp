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

#include "playermessage.h"


PlayerMessage::PlayerMessage(const QUuid &uuid, const QString &name, Player::State state) :
    Message(), m_uuid(uuid), m_state(state), m_name(name) {

}

const QUuid &PlayerMessage::uuid() const {
    return m_uuid;
}

Player::State PlayerMessage::state() const {
    return m_state;
}

const QString &PlayerMessage::name() const {
    return m_name;
}

bool PlayerMessage::writeStream(QDataStream &dataStream) {
    QByteArray buffer;
    QDataStream tempStream(&buffer, QIODevice::WriteOnly);

    quint16 messageType = Message::PlayerMessage;

    tempStream << messageType;
    tempStream << m_uuid;
    writeString(tempStream, m_name);
    tempStream << quint8(m_state);

    quint16 length = buffer.size() + 2;

    dataStream << length;
    dataStream.writeRawData(buffer.constData(), buffer.size());

    return dataStream.status() == QDataStream::Ok;
}

bool PlayerMessage::parseStream(QDataStream &dataStream) {
    dataStream >> m_uuid;
    m_name = readString(dataStream);
    quint8 state;
    dataStream >> state;
    m_state = (Player::State) state;

    return dataStream.status() == QDataStream::Ok;
}
