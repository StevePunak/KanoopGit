#include "gitfilesystemtreeview.h"
#include "gitfilesystemtreemodel.h"
#include "kanoopgittypes.h"
#include "gitentities.h"

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include <git2qt.h>

using namespace GIT;

GitFileSystemTreeView::GitFileSystemTreeView(QWidget *parent) :
    TreeViewBase(parent)
{
    header()->setVisible(true);
    setSortingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void GitFileSystemTreeView::createModel(Repository* repo)
{
    if(model() != nullptr) {
        delete model();
    }

    GitFileSystemTreeModel* treeModel = new GitFileSystemTreeModel(repo, this);
    connect(this, &GitFileSystemTreeView::refresh, treeModel, &GitFileSystemTreeModel::refresh);

    setSourceModel(treeModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &GitFileSystemTreeView::onCurrentIndexChanged);
}

void GitFileSystemTreeView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
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
