#include "playlist_model.h"
#include <QDebug>

PlaylistModel::PlaylistModel(QObject *parent)
    : QStandardItemModel(parent)
{
}

PlaylistModel::~PlaylistModel()
{
}

QStringList PlaylistModel::getNames()
{
    QStringList names;
    for (int i = 0; i < rowCount(); ++i)
    {
        names.append(item(i, 0)->text());
    }
    return names;
}

QStringList PlaylistModel::getPaths()
{
    QStringList paths;
    for (int i = 0; i < rowCount(); ++i)
    {
        paths.append(item(i, 1)->text());
    }
    return paths;
}

QString PlaylistModel::nameForPath(QString name)
{
    int index = getNames().indexOf(name);
    qDebug() << name << index;
    if (index >= 0)
        return getPaths()[index];
    else
        return QString();
}

bool PlaylistModel::removeByName(QString name)
{
    int index = getNames().indexOf(name); 
    return removeRow(index);
}

QString PlaylistModel::getPathForId(int id)
{
    if (id >= rowCount())
        return QString::Null();
    
    return item(id, 1)->text();

}

int PlaylistModel::getIdForName(QString name)
{
    return getNames().indexOf(name);
}

QModelIndex PlaylistModel::getIndexForPath(QString path)
{
    return indexFromItem(item(getPaths().indexOf(path)));
}
