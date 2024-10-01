#include "gittreeentrytableview.h"

#include "gittreeentrytablemodel.h"

#include <git2qt.h>
#include "kanoopgittypes.h"

using namespace GIT;

GitTreeEntryTableView::GitTreeEntryTableView(QWidget *parent) :
    TableViewBase(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void GitTreeEntryTableView::createModel(Repository* repo, const ObjectId& commitId)
{
    if(model() != nullptr) {
        delete model();
    }

    GitTreeEntryTableModel* tableModel = new GitTreeEntryTableModel(repo, commitId, this);
    setModel(tableModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &GitTreeEntryTableView::onCurrentIndexChanged);
}

void GitTreeEntryTableView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    TreeChangeEntry entry = TreeChangeEntry::fromVariant(current.data(TreeChangeEntryRole));
    if(entry.isValid()) {
        emit treeChangeEntryClicked(entry);
    }
}
