#include "branchtreemodel.h"
#include "branchtreeview.h"
#include "gitentities.h"
#include "kanoopgittypes.h"

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include <git2qt.h>

using namespace GIT;

BranchTreeView::BranchTreeView(QWidget *parent) :
    TreeViewBase(parent)
{
    header()->setVisible(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &BranchTreeView::doubleClicked, this, &BranchTreeView::onDoubleClicked);
}

void BranchTreeView::createModel(Repository* repo, BranchType typeToShow)
{
    if(model() != nullptr) {
        delete model();
    }

    BranchTreeModel* treeModel = new BranchTreeModel(repo, typeToShow, this);
    connect(this, &BranchTreeView::refresh, treeModel, &BranchTreeModel::refresh);

    setModel(treeModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &BranchTreeView::onCurrentIndexChanged);

    expandAll();
}

void BranchTreeView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
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

void BranchTreeView::onDoubleClicked(const QModelIndex &index)
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
