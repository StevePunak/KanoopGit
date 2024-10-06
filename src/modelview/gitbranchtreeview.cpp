#include "gitbranchtreemodel.h"
#include "gitbranchtreeview.h"
#include "gitentities.h"
#include "kanoopgittypes.h"

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include <git2qt.h>

using namespace GIT;

GitBranchTreeView::GitBranchTreeView(QWidget *parent) :
    TreeViewBase(parent)
{
    header()->setVisible(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &GitBranchTreeView::doubleClicked, this, &GitBranchTreeView::onDoubleClicked);
}

void GitBranchTreeView::createModel(Repository* repo, BranchType typeToShow)
{
    if(model() != nullptr) {
        delete model();
    }

    GitBranchTreeModel* treeModel = new GitBranchTreeModel(repo, typeToShow, this);
    connect(this, &GitBranchTreeView::refresh, treeModel, &GitBranchTreeModel::refresh);

    setModel(treeModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &GitBranchTreeView::onCurrentIndexChanged);

    expandAll();
}

void GitBranchTreeView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    if(current.isValid()) {
        GitEntities::Type type = (GitEntities::Type)current.data(KANOOP::MetadataTypeRole).toInt();
        if(type == GitEntities::Folder) {
            QString path = current.data(RelativePathRole).toString();
            emit folderClicked(path);
        }
        else if(type == GitEntities::Reference) {
            Reference reference = Reference::fromVariant(current.data(ReferenceRole));
            if(reference.isNull() == false) {
                emit referenceClicked(reference);
            }
        }
    }
}

void GitBranchTreeView::onDoubleClicked(const QModelIndex &index)
{
    if(index.isValid()) {
        GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
        if(type == GitEntities::Reference) {
            Reference reference = Reference::fromVariant(index.data(ReferenceRole));
            logText(LVL_DEBUG, QString("DblClick: %1").arg(reference.canonicalName()));
            if(reference.isNull() == false) {
                emit referenceDoubleClicked(reference);
            }
        }
    }
}
