#include "difftablemodel.h"
#include "difftableview.h"
#include "gitentities.h"
#include "kanoopgittypes.h"

#include <QHeaderView>
#include <QPainter>
#include <git2qt.h>

#include <Kanoop/geometry/rectangle.h>

using namespace GIT;
namespace Colors = QColorConstants::Svg;

DiffTableView::DiffTableView(QWidget *parent) :
    TableViewBase(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(SingleSelection);
}

void DiffTableView::createModelTreeToTree(Repository* repo, const Tree& oldTree, const Tree& newTree, const DiffDelta& delta)
{
    if(model() != nullptr) {
        delete model();
    }

    _delta = delta;
    _lastDeltaRow = -1;

    DiffTableModel* tableModel = new DiffTableModel(repo, oldTree, newTree, delta, this);
    setModel(tableModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &DiffTableView::onCurrentIndexChanged);

    restoreHeaderStates();
}

void DiffTableView::createModelIndexToWorkDir(GIT::Repository* repo, const GIT::DiffDelta& delta)
{
    if(model() != nullptr) {
        delete model();
    }

    _delta = delta;
    _lastDeltaRow = -1;

    DiffTableModel* tableModel = new DiffTableModel(repo, delta, this);
    setModel(tableModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &DiffTableView::onCurrentIndexChanged);

    restoreHeaderStates();
}

void DiffTableView::scrollToNextDelta()
{
    if(sourceModel() == nullptr) {
        return;
    }

    QModelIndexList indexes = static_cast<DiffTableModel*>(sourceModel())->nextDelta(_lastDeltaRow);
    if(indexes.count() > 0) {
        _lastDeltaRow = indexes.last().row();
        scrollTo(indexes.last());
    }
}

void DiffTableView::scrollToPreviousDelta()
{
    if(sourceModel() == nullptr) {
        return;
    }

    QModelIndexList indexes = static_cast<DiffTableModel*>(sourceModel())->previousDelta(_lastDeltaRow);
    if(indexes.count() > 0) {
        _lastDeltaRow = indexes.first().row();
        scrollTo(indexes.first());
    }
}

bool DiffTableView::hasNextDelta() const
{
    if(sourceModel() == nullptr) {
        return false;
    }

    return static_cast<DiffTableModel*>(sourceModel())->nextDelta(_lastDeltaRow).count() > 0;
}

bool DiffTableView::hasPreviousDelta() const
{
    if(sourceModel() == nullptr) {
        return false;
    }

    return static_cast<DiffTableModel*>(sourceModel())->previousDelta(_lastDeltaRow).count() > 0;
}

void DiffTableView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    GitEntities::Type itemType = (GitEntities::Type)current.data(KANOOP::MetadataTypeRole).toInt();

    switch (itemType) {
    default:
        break;
    }
}

