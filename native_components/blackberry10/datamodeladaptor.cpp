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

#include "datamodeladaptor.h"

#include <QDebug>

DataModelAdaptor::DataModelAdaptor(QObject *parent) :
    bb::cascades::DataModel(parent),
    m_model(NULL)
{
}

int DataModelAdaptor::childCount(const QVariantList &indexPath)
{
    if (!m_model)
        return 0;

    QModelIndex index = toModelIndex(indexPath);

    return m_model->rowCount(index);
}

QVariant DataModelAdaptor::data(const QVariantList &indexPath)
{
    if (!m_model)
        return QVariant();

    QModelIndex index = toModelIndex(indexPath);

    QVariantMap map;
    QHashIterator<int, QByteArray> it(m_model->roleNames());

    while (it.hasNext()) {
        it.next();

        map[it.value()] = m_model->data(index, it.key());
    }

    if (!m_model->roleNames().contains(Qt::DisplayRole))
        map["title"] = m_model->data(index, Qt::DisplayRole);

    return map;
}

bool DataModelAdaptor::hasChildren(const QVariantList &indexPath)
{
    if (!m_model)
        return false;

    QModelIndex index = toModelIndex(indexPath);

    return m_model->hasChildren(index);
}

QAbstractItemModel *DataModelAdaptor::model() const
{
    return m_model;
}

void DataModelAdaptor::setModel(QAbstractItemModel *model)
{
    if (model == m_model)
        return;

    if (m_model) {
        disconnect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(relayDataChanged(QModelIndex,QModelIndex)));
        disconnect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(relayRowsInserted(QModelIndex,int,int)));
        disconnect(m_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int,int)), this, SLOT(relayRowsMoved(QModelIndex,int,int,QModelIndex,int)));
        disconnect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(relayRowsRemoved(QModelIndex,int,int)));
        disconnect(m_model, SIGNAL(modelReset()), this, SLOT(relayModelReset()));
    }

    m_model = model;
    emit modelChanged();

    emit itemsChanged(bb::cascades::DataModelChangeType::Init);

    if (m_model) {
        connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(relayDataChanged(QModelIndex,QModelIndex)));
        connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(relayRowsInserted(QModelIndex,int,int)));
        connect(m_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), SLOT(relayRowsMoved(QModelIndex,int,int,QModelIndex,int)));
        connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), SLOT(relayRowsRemoved(QModelIndex,int,int)));
        connect(m_model, SIGNAL(modelReset()), SLOT(relayModelReset()));
    }
}


QModelIndex DataModelAdaptor::toModelIndex(const QVariantList &indexPath) const
{
    QModelIndex modelIndex;

    foreach (const QVariant &entry, indexPath) {
        int row = entry.toInt();
        modelIndex = m_model->index(row, 0, modelIndex);
    }

    return modelIndex;
}

QVariantList DataModelAdaptor::toIndexPath(const QModelIndex &index) const
{
    QVariantList result;
    QModelIndex currentIndex = index;

    while (currentIndex.isValid())
        result.prepend(currentIndex.row());

    return result;

}

void DataModelAdaptor::relayDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    QVariantList indexPath = toIndexPath(topLeft);
    for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
        indexPath.replace(indexPath.size() - 1, row);
        emit itemUpdated(indexPath);
    }
}

void DataModelAdaptor::relayRowsInserted(const QModelIndex &parent, int start, int end)
{
    QVariantList indexPath = toIndexPath(parent) << 0;
    for (int row = start; row <= end; row++) {
        indexPath.replace(indexPath.size() - 1, row);
        emit itemAdded(indexPath);
    }
}

void DataModelAdaptor::relayRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationParent);
    Q_UNUSED(destinationRow);

    // TODO - IndexMapper implementation
    emit itemsChanged(bb::cascades::DataModelChangeType::AddRemove);
}

void DataModelAdaptor::relayRowsRemoved(const QModelIndex &parent, int start, int end)
{
    QVariantList indexPath = toIndexPath(parent) << 0;
    for (int row = start; row <= end; row++) {
        indexPath.replace(indexPath.size() - 1, row);
        emit itemRemoved(indexPath);
    }
}

void DataModelAdaptor::relayModelReset()
{
    emit itemsChanged(bb::cascades::DataModelChangeType::Init);
}



