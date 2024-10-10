#include "filesystemtreeview.h"
#include "filesystemtreemodel.h"
#include "kanoopgittypes.h"
#include "gitentities.h"

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include <git2qt.h>

using namespace GIT;

FileSystemTreeView::FileSystemTreeView(QWidget *parent) :
    TreeViewBase(parent)
{
    header()->setVisible(true);
    setSortingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void FileSystemTreeView::createModel(Repository* repo)
{
    if(model() != nullptr) {
        delete model();
    }

    FileSystemTreeModel* treeModel = new FileSystemTreeModel(repo, this);
    connect(this, &FileSystemTreeView::refresh, treeModel, &FileSystemTreeModel::refresh);

    setModel(treeModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &FileSystemTreeView::onCurrentIndexChanged);
}

void FileSystemTreeView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    if(current.isValid()) {
        GitEntities::Type type = (GitEntities::Type)current.data(KANOOP::MetadataTypeRole).toInt();
        if(type == GitEntities::Folder) {
            QString path = current.data(AbsolutePathRole).toString();
            emit folderClicked(path);
        }
        else if(type == GitEntities::IndexEntry) {
            IndexEntry entry = IndexEntry::fromVariant(current.data(IndexEntryRole));
            if(entry.isValid()) {
                emit indexEntryClicked(entry);
            }
        }
    }
}
