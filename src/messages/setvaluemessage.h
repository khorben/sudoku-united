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

#ifndef SETVALUEMESSAGE_H
#define SETVALUEMESSAGE_H

#include "message.h"

class SetValueMessage : public Message {
public:
    SetValueMessage();
    SetValueMessage(quint8 x, quint8 y, quint8 value, QUuid valueOwner);

    quint8 x() const { return m_x; }
    quint8 y() const { return m_y; }
    quint8 value() const { return m_value; }
    const QUuid & valueOwner() const { return m_valueOwner; }

    bool writeStream(QDataStream &dataStream);

    Message::MessageTypeId type() const { return Message::SetValueMessage; }
protected:
    bool parseStream(QDataStream &dataStream);
private:
    quint8 m_x;
    quint8 m_y;
    quint8 m_value;
    QUuid m_valueOwner;
};

#endif // SETVALUEMESSAGE_H
