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

#include "sectiondatamodel.h"

SectionDataModel::SectionDataModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_model(NULL), m_sectionRole(0)
{
}

int SectionDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

int SectionDataModel::rowCount(const QModelIndex &parent) const
{
    if (!m_model || !m_sectionRole)
        return 0;

    if (!parent.isValid())
        return m_sectionMap.length();
    else {
        if (parent.row() >= m_sectionMap.length())
            return 0;

        return m_sectionMap[parent.row()].entries.length();
    }
}

QVariant SectionDataModel::data(const QModelIndex &index, int role) const
{
    if (!m_model || !m_sectionRole)
        return QVariant();

    if (index.parent().isValid()) {
        if (index.internalId() >= m_sectionMap.length())
            return QVariant();

        if (index.row() >= m_sectionMap[index.internalId()].entries.length())
            return QVariant();

        QModelIndex childIndex = m_sectionMap[index.internalId()].entries[index.row()];
        return m_model->data(childIndex, role);
    } else {
        if (index.row() >= m_sectionMap.length())
            return QVariant();

        if (role == m_sectionRole || role == Qt::DisplayRole)
            return m_sectionMap[index.row()].section;
        else
            return QVariant();
    }
}

QModelIndex SectionDataModel::parent(const QModelIndex &child) const
{
    if (child.internalId() >= 0)
        return createIndex(child.internalId(), 0, -1);
    else
        return QModelIndex();
}

QModelIndex SectionDataModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return createIndex(row, column, parent.row());
    } else {
        return createIndex(row, column, -1);
    }
}

QAbstractItemModel *SectionDataModel::model() const
{
    return m_model;
}

void SectionDataModel::setModel(QAbstractItemModel *model)
{
    if (model == m_model)
        return;

    if (m_model) {
        disconnect(m_model, SIGNAL(modelReset()), this, SLOT(buildSectionMap()));
        disconnect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(handleRowsInserted(QModelIndex,int,int)));
        disconnect(m_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(handleRowsMoved(QModelIndex,int,int,QModelIndex,int)));
        disconnect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(handleRowsRemoved(QModelIndex,int,int)));
        disconnect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(handleDataChanged(QModelIndex,QModelIndex)));
    }

    m_model = model;

    if (m_model) {
        connect(m_model, SIGNAL(modelReset()), SLOT(buildSectionMap()));
        connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(handleRowsInserted(QModelIndex,int,int)));
        connect(m_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), SLOT(handleRowsMoved(QModelIndex,int,int,QModelIndex,int)));
        connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), SLOT(handleRowsRemoved(QModelIndex,int,int)));
        connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(handleDataChanged(QModelIndex,QModelIndex)));

        setRoleNames(m_model->roleNames());
    } else {
        setRoleNames(QHash<int, QByteArray>());
    }

    if (!m_sectionKey.isEmpty()) {
        m_sectionRole = roleNames().key(m_sectionKey.toLatin1());
    }

    emit modelChanged();

    buildSectionMap();
}

QString SectionDataModel::sectionKey() const
{
    return m_sectionKey;
}

void SectionDataModel::setSectionKey(const QString &sectionKey)
{
    if (sectionKey == m_sectionKey)
        return;

    m_sectionKey = sectionKey;
    if (m_sectionKey.isEmpty())
        m_sectionRole = 0;
    else {
        m_sectionRole = roleNames().key(m_sectionKey.toLatin1());
    }

    emit sectionKeyChanged();

    buildSectionMap();
}

void SectionDataModel::buildSectionMap()
{
    beginResetModel();

    m_sectionMap.clear();

    if (m_model && m_sectionRole) {
        for (int i = 0; i < m_model->rowCount(); i++) {
            bool inserted = false;
            QModelIndex index = m_model->index(i, 0);
            QVariant section = m_model->data(index, m_sectionRole);

            QMutableListIterator<SectionEntries> it(m_sectionMap);
            while (it.hasNext() && !inserted) {
                SectionEntries &entry = it.next();
                if (entry.section != section)
                    continue;

                entry.entries.append(index);
                inserted = true;
            }

            if (!inserted) {
                m_sectionMap.append((SectionEntries) { section, QList<QModelIndex>() << index });
            }
        }
    }

    endResetModel();
}

void SectionDataModel::handleRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);

    buildSectionMap();
}

void SectionDataModel::handleRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &newParent, int newStart)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    Q_UNUSED(newParent);
    Q_UNUSED(newStart);

    buildSectionMap();
}

void SectionDataModel::handleRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);

    buildSectionMap();
}

void SectionDataModel::handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);

    buildSectionMap();
}
