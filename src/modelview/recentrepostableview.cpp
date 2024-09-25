#include "recentrepostableview.h"
#include "recentrepostablemodel.h"
#include "gitroles.h"

RecentReposTableView::RecentReposTableView(QWidget *parent) :
    TableViewBase(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void RecentReposTableView::createModel(const QStringList& repoPaths)
{
    if(model() != nullptr) {
        delete model();
    }

    RecentReposTableModel* tableModel = new RecentReposTableModel(repoPaths, this);
    setSourceModel(tableModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &RecentReposTableView::onCurrentIndexChanged);
}

void RecentReposTableView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    QString path = current.data(RepoPathRole).toString();
    if(path.isEmpty() == false) {
        emit recentRepoPathClicked(path);
    }
}
