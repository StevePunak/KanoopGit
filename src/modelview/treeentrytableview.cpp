#include "treeentrytableview.h"

#include "treeentrytablemodel.h"

#include <git2qt.h>
#include "kanoopgittypes.h"

using namespace GIT;

TreeEntryTableView::TreeEntryTableView(QWidget *parent) :
    TableViewBase(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void TreeEntryTableView::createModel(Repository* repo, const ObjectId& commitId)
{
    if(model() != nullptr) {
        delete model();
    }

    TreeEntryTableModel* tableModel = new TreeEntryTableModel(repo, commitId, this);
    setModel(tableModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &TreeEntryTableView::onCurrentIndexChanged);
}

void TreeEntryTableView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    TreeChangeEntry entry = TreeChangeEntry::fromVariant(current.data(TreeChangeEntryRole));
    if(entry.isValid()) {
        emit treeChangeEntryClicked(entry);
    }
}
