#include "gitdifftablemodel.h"
#include "gitdifftableview.h"
#include "gitentities.h"
#include "kanoopgittypes.h"

#include <QHeaderView>
#include <QPainter>
#include <git2qt.h>

#include <Kanoop/geometry/rectangle.h>

using namespace GIT;
namespace Colors = QColorConstants::Svg;

GitDiffTableView::GitDiffTableView(QWidget *parent) :
    TableViewBase(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(SingleSelection);
}

void GitDiffTableView::createModelTreeToTree(Repository* repo, const Tree& oldTree, const Tree& newTree, const DiffDelta& delta)
{
    if(model() != nullptr) {
        delete model();
    }

    _delta = delta;
    _lastDeltaRow = -1;

    GitDiffTableModel* tableModel = new GitDiffTableModel(repo, oldTree, newTree, delta, this);
    setModel(tableModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &GitDiffTableView::onCurrentIndexChanged);

    restoreHeaderStates();
}

void GitDiffTableView::createModelIndexToWorkDir(GIT::Repository* repo, const GIT::DiffDelta& delta)
{
    if(model() != nullptr) {
        delete model();
    }

    _delta = delta;
    _lastDeltaRow = -1;

    GitDiffTableModel* tableModel = new GitDiffTableModel(repo, delta, this);
    setModel(tableModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &GitDiffTableView::onCurrentIndexChanged);

    restoreHeaderStates();
}

void GitDiffTableView::scrollToNextDelta()
{
    if(sourceModel() == nullptr) {
        return;
    }

    QModelIndexList indexes = static_cast<GitDiffTableModel*>(sourceModel())->nextDelta(_lastDeltaRow);
    if(indexes.count() > 0) {
        _lastDeltaRow = indexes.last().row();
        scrollTo(indexes.last());
    }
}

void GitDiffTableView::scrollToPreviousDelta()
{
    if(sourceModel() == nullptr) {
        return;
    }

    QModelIndexList indexes = static_cast<GitDiffTableModel*>(sourceModel())->previousDelta(_lastDeltaRow);
    if(indexes.count() > 0) {
        _lastDeltaRow = indexes.first().row();
        scrollTo(indexes.first());
    }
}

bool GitDiffTableView::hasNextDelta() const
{
    if(sourceModel() == nullptr) {
        return false;
    }

    return static_cast<GitDiffTableModel*>(sourceModel())->nextDelta(_lastDeltaRow).count() > 0;
}

bool GitDiffTableView::hasPreviousDelta() const
{
    if(sourceModel() == nullptr) {
        return false;
    }

    return static_cast<GitDiffTableModel*>(sourceModel())->previousDelta(_lastDeltaRow).count() > 0;
}

void GitDiffTableView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    GitEntities::Type itemType = (GitEntities::Type)current.data(KANOOP::MetadataTypeRole).toInt();

    switch (itemType) {
    default:
        break;
    }
}

