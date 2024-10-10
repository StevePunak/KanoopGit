#include "gitassets.h"
#include "gitentities.h"
#include "filesystemtreemodel.h"
#include "kanoopgittypes.h"

#include <QDirIterator>
#include <QFileInfo>

#include <Kanoop/pathutil.h>

#include <Kanoop/gui/resources.h>

using namespace GIT;

FileSystemTreeModel::FileSystemTreeModel(Repository* repo, QObject *parent) :
    AbstractTreeModel("gitfstree", parent),
    _repo(repo), _rootLazyLoadComplete(false)
{
    appendColumnHeader(CH_Name, "Name");
    // loadDirectories();
    // loadFiles();
}

void FileSystemTreeModel::refresh()
{
    QModelIndex topLeft = index(0, 0, QModelIndex());
    int rows = rowCount(QModelIndex());
    int cols = columnCount(QModelIndex());
    QModelIndex bottomRight = index(rows - 1, cols - 1, QModelIndex());
    emit dataChanged(topLeft, bottomRight);
}

bool FileSystemTreeModel::canFetchMore(const QModelIndex& parent) const
{
    bool result = false;
    if(parent.isValid() == false) {
        result = _rootLazyLoadComplete == false;
    }
    else {
        GitEntities::Type type = (GitEntities::Type)parent.data(KANOOP::EntityTypeRole).toInt();
        if(type == GitEntities::Folder) {
            FolderItem* item = static_cast<FolderItem*>(parent.internalPointer());
            result = item->lazyLoadComplete() == false;
        }
        else if(type == GitEntities::File) {
            result = false;
        }
    }
    return result;
}

void FileSystemTreeModel::fetchMore(const QModelIndex& parent)
{
    if(parent.isValid() == false) {
        if(_rootLazyLoadComplete == false) {
            loadDirectories(_repo->localPath(), nullptr);
            loadFiles(_repo->localPath(), nullptr);
            _rootLazyLoadComplete = true;
        }
    }
    else {
        GitEntities::Type type = (GitEntities::Type)parent.data(KANOOP::EntityTypeRole).toInt();
        if(type == GitEntities::Folder) {
            FolderItem* item = static_cast<FolderItem*>(parent.internalPointer());
            clearPlaceHolder(item);
            loadDirectories(item->absolutePath(), item);
            loadFiles(item->absolutePath(), item);
        }
        else if(type == GitEntities::File) {
            logText(LVL_WARNING, "Sholdn't get here");
        }
    }
}

void FileSystemTreeModel::clearPlaceHolder(FolderItem* item)
{
    if(item->children().count() == 1) {
        AbstractModelItem* child = item->children().at(0);
        if(child->entityMetadata().type() == GitEntities::PlaceHolder) {
           item->deleteChild(child);
        }
    }
}

void FileSystemTreeModel::loadDirectories(const QString& parentDirectory, FolderItem* parentItem)
{
    QDirIterator it(parentDirectory, QDir::Dirs | QDir::NoDotAndDotDot);
    while(it.hasNext()) {
        QDir dir(it.next());
        if(dir.exists()) {
            if(parentItem != nullptr) {
                parentItem->appendChild(new FolderItem(dir.absolutePath(), this));
            }
            else {
                appendRootItem(new FolderItem(dir.absolutePath(), this));
            }
        }
    }
}

void FileSystemTreeModel::loadFiles(const QString& parentDirectory, FolderItem* parentItem)
{
    QDirIterator it(parentDirectory, QDir::Files | QDir::NoDotAndDotDot);
    while(it.hasNext()) {
        QFileInfo fileinfo(it.next());
        if(parentItem != nullptr) {
            parentItem->appendChild(new FileItem(fileinfo.absoluteFilePath(), this));
        }
        else {
            appendRootItem(new FileItem(fileinfo.absoluteFilePath(), this));
        }
    }

    if(parentItem != nullptr) {
        parentItem->setLazyLoadComplete(true);
    }
}

QModelIndex FileSystemTreeModel::findFolderIndex(const QString& path)
{
    QModelIndex result;
    QModelIndex startSearchIndex = index(0, 0, QModelIndex());
    QModelIndexList found = match(startSearchIndex, AbsolutePathRole, path, 1, Qt::MatchRecursive | Qt::MatchWrap);
    if(found.count() > 0) {
        result = found.first();
    }
    return result;
}

QString FileSystemTreeModel::parentPath(const QString& path)
{
    return PathUtil::popLevel(path);
}

// -------------------------------- TreeBaseItem --------------------------------


FileSystemTreeModel::TreeBaseItem::TreeBaseItem(const EntityMetadata &metadata, const QString& path, FileSystemTreeModel* model) :
    AbstractModelItem(metadata, model),
    _absolutePath(path)
{
    QFileInfo fileInfo(_absolutePath);
    _filename = fileInfo.fileName();
    _relativePath = _absolutePath.mid(repo()->localPath().length() + 1);
}

QVariant FileSystemTreeModel::TreeBaseItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DisplayRole:
            result = _filename;
            break;
        case AbsolutePathRole:
            result = _absolutePath;
            break;
        case RelativePathRole:
            result = _relativePath;
            break;
        default:
            break;
        }
    }

    if(result.isValid() == false) {
        result = AbstractModelItem::data(index, role);
    }
    return result;
}

// -------------------------------- FolderItem --------------------------------


FileSystemTreeModel::FolderItem::FolderItem(const QString& path, FileSystemTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::Folder), path, model),
    _lazyLoadComplete(false)
{
    QFileInfo fileInfo(path);
    _folderName = fileInfo.fileName();
    _icon = Resources::getIcon(GitAssets::Folder);
    appendChild(new PlaceHolderItem(path, model));
}

QVariant FileSystemTreeModel::FolderItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DecorationRole:
            return _icon;
            break;
        case AbsolutePathRole:
            result = absolutePath();
            break;
        default:
            break;
        }
    }

    if(result.isValid() == false) {
        result = TreeBaseItem::data(index, role);
    }
    return result;
}

void FileSystemTreeModel::FolderItem::setLazyLoadComplete(bool value)
{
    _lazyLoadComplete = value;
}


// -------------------------------- FileItem --------------------------------


FileSystemTreeModel::FileItem::FileItem(const QString& path, FileSystemTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::File), path, model)
{
}

QVariant FileSystemTreeModel::FileItem::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.column() == 0) {
        switch(role) {
        case Qt::DecorationRole:
        {
            StatusEntry entry = repo()->status().entries().findByPath(relativePath());
            if(entry.isValid()) {
                switch(entry.status()) {
                case NewInIndex:
                case NewInWorkdir:
                    result = Resources::getIcon(GitAssets::PlusGreen);
                    break;
                case ModifiedInWorkdir:
                    result = Resources::getIcon(GitAssets::LedRed);
                    break;
                case ModifiedInIndex:
                    result = Resources::getIcon(GitAssets::PlusWhiteOnGrey);
                    break;
                default:
                    Log::logText(LVL_DEBUG, QString("Unhandled status %1 for %2").arg(getFileStatusString(entry.status())).arg(entry.path()));
                    break;
                }
            }
            else {
                IndexEntry indexEntry = repo()->index()->findByPath(relativePath());
                if(indexEntry.isValid()) {
                    result = Resources::getIcon(GitAssets::CheckGreen);
                }
                else {
                    // Ignored
                    result = Resources::getIcon(GitAssets::DashWhiteOnGrey);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    if(result.isValid() == false) {
        result = TreeBaseItem::data(index, role);
    }
    return result;
}


FileSystemTreeModel::PlaceHolderItem::PlaceHolderItem(const QString& path, FileSystemTreeModel* model) :
    TreeBaseItem(EntityMetadata(GitEntities::PlaceHolder), path, model)
{

}
