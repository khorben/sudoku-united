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

#ifndef JOINMESSAGE_H
#define JOINMESSAGE_H

#include "message.h"

class JoinMessage : public Message {
public:
    JoinMessage();
    JoinMessage(QUuid uuid, QString name);

    const QString &name() const { return m_name; }
    const QUuid &uuid() const { return m_uuid; }

    virtual bool writeStream(QDataStream &dataStream);

    Message::MessageTypeId type() const { return Message::JoinMessage; }
protected:
    virtual bool parseStream(QDataStream &dataStream);
private:
    QString m_name;
    QUuid m_uuid;
};

#endif // JOINMESSAGE_H
