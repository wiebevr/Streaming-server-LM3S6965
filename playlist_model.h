#ifndef PLAYLIST_MODEL_H
#define PLAYLIST_MODEL_H

#include <QStandardItemModel>
#include <QModelIndex>
#include <QStringList>
#include <QString>

class PlaylistModel
    : public QStandardItemModel
{
    Q_OBJECT

    public:
        PlaylistModel(QObject *parent);
        ~PlaylistModel();

        QStringList getNames();
        QStringList getPaths();
        QString nameForPath(QString name);
        bool removeByName(QString name);
        QModelIndex getIndexForPath(QString path);

};
#endif // PLAYLIST_MODEL_H
