#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include <QAbstractListModel>
#include <QBitArray>

class Note;
class Game;

class NoteModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum NoteModelType {
        ValueType = Qt::UserRole + 1,
        MarkedType
    };

public:
    explicit NoteModel(QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Q_INVOKABLE
    QObject *get(int index);
private:
    friend QDataStream &operator<<(QDataStream &stream, Game &game);
    friend QDataStream &readGameV2(QDataStream &stream, Game &game);

    QBitArray notes;
};

class Note : public QObject {
    Q_OBJECT
    Q_PROPERTY(int modelValue READ value CONSTANT)
    Q_PROPERTY(bool modelMarked READ marked WRITE setMarked NOTIFY markedChanged)
public:
    explicit Note(NoteModel *model, int value);

    int value() const;

    bool marked() const;
    void setMarked(bool marked);
signals:
    void markedChanged();
private slots:
    void onDataChanged(const QModelIndex &topLeft,
                       const QModelIndex &bottomRight);
private:
    NoteModel *m_model;
    int m_value;
};

#endif // NOTEMODEL_H
