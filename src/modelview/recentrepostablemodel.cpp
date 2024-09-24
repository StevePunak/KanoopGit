#include "recentrepostablemodel.h"

#include <QFileInfo>
#include <QStandardPaths>

#include <Kanoop/pathutil.h>
#include "gitroles.h"

RecentReposTableModel::RecentReposTableModel(const QStringList& repoPaths, QObject *parent) :
    AbstractTableModel("recenttable", parent)
{
    appendColumnHeader(CH_RecentRepos, "Recent Repos");

    QString homeDir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0);
    for(const QString& path : repoPaths) {
        QString displayPath = path;
        if(path.startsWith(homeDir)) {
            displayPath = "~/" + path.mid(homeDir.length());
        }
        appendRootItem(new RepoPathItem(path, displayPath, this));
    }
}

QVariant RecentReposTableModel::RepoPathItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    switch(role) {
    case Qt::DisplayRole:
    {
        TableHeader header = static_cast<AbstractTableModel*>(model())->columnHeader(index.column());
        switch(header.type()) {
        case CH_RecentRepos:
            result = _displayPath;
            break;
        default:
            break;
        }
    }
    break;

    case RepoPathRole:
        result = _fullPath;
        break;

    default:
        break;
    }

    if(result.isValid() == false) {
        result = AbstractModelItem::data(index, role);
    }
    return result;
}
