#include "leftsidebartreeview.h"
#include "kanoopgittypes.h"
#include "leftsidebartreemodel.h"

#include <QPainter>
#include <repoconfig.h>
#include <settings.h>

#include <widgets/submodulelabelwidget.h>

#include <Kanoop/geometry/rectangle.h>

#include <Kanoop/stringutil.h>

using namespace GIT;

LeftSidebarTreeView::LeftSidebarTreeView(QWidget *parent) :
    TreeViewBase(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &LeftSidebarTreeView::expanded, this, &LeftSidebarTreeView::onExpanded);
    connect(this, &LeftSidebarTreeView::collapsed, this, &LeftSidebarTreeView::onCollapsed);
    connect(this, &LeftSidebarTreeView::doubleClicked, this, &LeftSidebarTreeView::onDoubleClicked);
}

LeftSidebarTreeView::~LeftSidebarTreeView()
{
    qDeleteAll(_submoduleWidgets);
}

void LeftSidebarTreeView::createModel(GIT::Repository* repo)
{
    if(model() != nullptr) {
        delete model();
    }

    qDeleteAll(_submoduleWidgets);
    _submoduleWidgets.clear();

    _repo = repo;

    LeftSidebarTreeModel* treeModel = new LeftSidebarTreeModel(repo, this);
    setModel(treeModel);

    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &LeftSidebarTreeView::onCurrentIndexChanged);

    RepoConfig config = Settings::instance()->repoConfig(_repo->localPath());
    if(config.localBranchesVisible()) {
        expandRecursively(treeModel->localBranchesIndex());
    }

    if(config.remoteBranchesVisible()) {
        expandRecursively(treeModel->remoteBranchesIndex());
    }

    if(config.submodulesVisible()) {
        expandRecursively(treeModel->submodulesIndex());
    }

    setItemDelegateForColumn(0, new SubmoduleStyledItemDelegate(this));

    // Create widgets
    for(const Submodule& submodule : _repo->submodules()) {
        SubmoduleLabelWidget* labelWidget = new SubmoduleLabelWidget(_repo, submodule);
        _submoduleWidgets.insert(submodule.name(), labelWidget);
    }

    for(const QModelIndex& index : treeModel->submoduleIndexes()) {
        openPersistentEditor(index);
    }
}

void LeftSidebarTreeView::setSubmoduleSpinning(const GIT::Submodule& submodule, bool value)
{
    SubmoduleLabelWidget* widget = _submoduleWidgets.value(submodule.name());
    if(widget != nullptr) {
        if(value) {
            widget->setSpinnerVisible(true);
            widget->setSpinning(true);
        }
        else {
            widget->setSpinning(false);
            widget->hideSpinnerIn(TimeSpan::fromSeconds(3));
        }
    }
}

void LeftSidebarTreeView::setSubmoduleSpinnerValue(const GIT::Submodule& submodule, int value)
{
    SubmoduleLabelWidget* widget = _submoduleWidgets.value(submodule.name());
    if(widget != nullptr) {
        if(widget->isSpinning() == false) {
            widget->setSpinning(true);
        }
        if(value != widget->spinnerValue()) {
            widget->setSpinnerValue(value);
        }
    }
}

void LeftSidebarTreeView::hideAllSubmoduleSpinners()
{
    for(SubmoduleLabelWidget* widget : qAsConst(_submoduleWidgets)) {
        widget->setSpinning(false);
    }
}

void LeftSidebarTreeView::onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    if(current.isValid()) {
        GitEntities::Type type = (GitEntities::Type)current.data(KANOOP::MetadataTypeRole).toInt();
        switch(type) {
        case GitEntities::Folder:
        {
            QString path = current.data(RelativePathRole).toString();
            emit folderClicked(path);
            break;
        }
        case GitEntities::Reference:
        {
            Reference reference = Reference::fromVariant(current.data(ReferenceRole));
            if(reference.isNull() == false) {
                emit referenceClicked(reference);
            }
            break;
        }
        case GitEntities::Submodule:
        {
            Submodule submodule = Submodule::fromVariant(current.data(KANOOP::DataRole));
            if(submodule.isNull() == false) {
                emit submoduleClicked(submodule);
            }
            break;
        }
        default:
            break;
        }
    }
}

void LeftSidebarTreeView::onDoubleClicked(const QModelIndex& index)
{
    if(index.isValid()) {
        GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
        if(type == GitEntities::Reference) {
            Reference reference = Reference::fromVariant(index.data(ReferenceRole));
            logText(LVL_DEBUG, QString("DblClick: %1").arg(reference.canonicalName()));
            if(reference.isNull() == false) {
                if(reference.isRemote()) {
                    emit remoteReferenceDoubleClicked(reference);
                }
                else {
                    emit localReferenceDoubleClicked(reference);
                }
            }
        }
        else if(type == GitEntities::Submodule) {
            Submodule submodule = Submodule::fromVariant(index.data(KANOOP::DataRole));
            logText(LVL_DEBUG, QString("DblClick: %1").arg(submodule.name()));
            if(submodule.isNull() == false) {
                emit submoduleDoubleClicked(submodule);
            }
        }
    }
}

void LeftSidebarTreeView::onExpanded(const QModelIndex& index)
{
    GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
    if(type == GitEntities::TitleItem) {
        RepoConfig config = Settings::instance()->repoConfig(_repo->localPath());
        ControlType controlType = (ControlType)index.data(ControlTypeRole).toInt();
        switch(controlType) {
        case LocalBranches:
            config.setLocalBranchesVisible(true);
            Settings::instance()->saveRepoConfig(config);
            break;
        case RemoteBranches:
            config.setRemoteBranchesVisible(true);
            Settings::instance()->saveRepoConfig(config);
            break;
        case Submodules:
            config.setSubmodulesVisible(true);
            Settings::instance()->saveRepoConfig(config);
            break;
        default:
            break;
        }
    }
}

void LeftSidebarTreeView::onCollapsed(const QModelIndex& index)
{
    GitEntities::Type type = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
    if(type == GitEntities::TitleItem) {
        RepoConfig config = Settings::instance()->repoConfig(_repo->localPath());
        ControlType controlType = (ControlType)index.data(ControlTypeRole).toInt();
        switch(controlType) {
        case LocalBranches:
            config.setLocalBranchesVisible(false);
            Settings::instance()->saveRepoConfig(config);
            break;
        case RemoteBranches:
            config.setRemoteBranchesVisible(false);
            Settings::instance()->saveRepoConfig(config);
            break;
        case Submodules:
            config.setSubmodulesVisible(false);
            Settings::instance()->saveRepoConfig(config);
            break;
        default:
            break;
        }
    }
}

void SubmoduleStyledItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    GitEntities::Type metadataType = (GitEntities::Type)index.data(KANOOP::MetadataTypeRole).toInt();
    if(metadataType != GitEntities::Submodule) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    // Draw the label
    QString name = index.data(Qt::DisplayRole).toString();
    if(name.isEmpty() == true) {
        return;
    }

    SubmoduleLabelWidget* widget = _tableView->getSubmoduleWidget(name);
    if(widget == nullptr) {
        return;
    }

    Rectangle rect = option.rect;
    painter->save();

    painter->translate(rect.topLeft());
    widget->resize(rect.size().toSize());
    widget->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);

    painter->restore();
}

QWidget* SubmoduleStyledItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)

    QString name = index.data(Qt::DisplayRole).toString();
    QWidget* result = _tableView->getSubmoduleWidget(name);
    if(result != nullptr) {
        result->setParent(parent);
    }

    return result;
}

void SubmoduleStyledItemDelegate::destroyEditor(QWidget* editor, const QModelIndex& index) const
{
    Q_UNUSED(index) Q_UNUSED(editor)
}

