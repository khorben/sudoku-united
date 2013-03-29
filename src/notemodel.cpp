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

#include "notemodel.h"
#include <QDebug>

NoteModel::NoteModel(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roleNames;

    roleNames[ValueType] = "modelValue";
    roleNames[MarkedType] = "modelMarked";

    setRoleNames(roleNames);

    notes.resize(9);
}

int NoteModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 9;
}

QVariant NoteModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case ValueType:
        return index.row() + 1;
    case MarkedType:
        return notes[index.row()];
    default:
        return QVariant();
    }
}

bool NoteModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid())
        return false;

    if (role != MarkedType) {
        qWarning() << roleNames()[role] << "is immutable";
        return false;
    }

    bool newValue = value.toBool();
    if (notes[index.row()] == newValue)
        return true;

    bool wasEmpty = isEmpty();

    notes[index.row()] = newValue;
    emit dataChanged(index, index);
    emit markedChanged(index.row() + 1, newValue);

    if (wasEmpty != isEmpty())
        emit emptyChanged();

    return true;
}

/**
  * Return QObject rather than Note as otherwise the GC does not clean up the
  * allocated object (http://developer.qt.nokia.com/forums/viewthread/6903).
  */
QObject *NoteModel::get(int index)
{
    return new Note(this, index + 1);
}

bool NoteModel::isMarked(int value) const
{
    return data(index(value - 1), NoteModel::MarkedType).toBool();
}

void NoteModel::setMarked(int value, bool marked)
{
    setData(index(value - 1), marked, NoteModel::MarkedType);
}

void NoteModel::clear()
{
    for (quint8 i = 0; i < 9; i++)
        setMarked(i + 1, false);
}

bool NoteModel::isEmpty() const
{
    return notes.count(true) == 0;
}

Note::Note(NoteModel *model, int value) :
    m_model(model), m_value(value)
{
    connect(m_model,
            SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            SLOT(onDataChanged(QModelIndex,QModelIndex)));
}

int Note::value() const
{
    return m_value;
}

bool Note::marked() const
{
    return m_model->data(m_model->index(m_value - 1), NoteModel::MarkedType).toBool();
}

void Note::setMarked(bool marked)
{
    m_model->setData(m_model->index(m_value - 1), marked, NoteModel::MarkedType);
}

void Note::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(bottomRight);

    if (topLeft.row() != m_value - 1)
        return;

    emit markedChanged();
}


