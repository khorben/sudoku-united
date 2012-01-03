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

    notes[index.row()] = newValue;
    emit dataChanged(index, index);

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


