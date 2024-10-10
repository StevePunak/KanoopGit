#ifndef LEFTSIDEBARTREEVIEW_H
#define LEFTSIDEBARTREEVIEW_H
#include <Kanoop/gui/treeviewbase.h>
#include <QStyledItemDelegate>
#include <git2qt.h>

class LocalBranchLabelWidget;
class SubmoduleLabelWidget;
class LeftSidebarTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    explicit LeftSidebarTreeView(QWidget* parent = nullptr);
    virtual ~LeftSidebarTreeView();

    void createModel(GIT::Repository* repo);

    void setSubmoduleSpinning(const GIT::Submodule& submodule, bool value);
    void setSubmoduleSpinnerValue(const GIT::Submodule& submodule, int value);
    void hideAllSubmoduleSpinners();

private:
    SubmoduleLabelWidget* getSubmoduleWidget(const QString& name) { return _submoduleWidgets.value(name); }
    LocalBranchLabelWidget* getLocalBranchWidget(const QString& canonicalName) { return _localBranchWidgets.value(canonicalName); }
    GIT::Repository* _repo;
    QMap<QString, SubmoduleLabelWidget*> _submoduleWidgets;
    QMap<QString, LocalBranchLabelWidget*> _localBranchWidgets;

    friend class SubmoduleStyledItemDelegate;

signals:
    void folderClicked(const QString& folderPath);
    void referenceClicked(const GIT::Reference& reference);
    void localReferenceDoubleClicked(const GIT::Reference& reference);
    void remoteReferenceDoubleClicked(const GIT::Reference& reference);
    void submoduleClicked(const GIT::Submodule& submodule);
    void submoduleDoubleClicked(const GIT::Submodule& submodule);

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
    void onDoubleClicked(const QModelIndex& index);
    void onExpanded(const QModelIndex& index);
    void onCollapsed(const QModelIndex& index);
};

class SubmoduleStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    SubmoduleStyledItemDelegate(LeftSidebarTreeView* parent = nullptr) :
        QStyledItemDelegate(parent),
        _tableView(parent) {}

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paintSubmodule(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void paintLocalBranch(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void destroyEditor(QWidget *editor, const QModelIndex &index) const override;

private:
    LeftSidebarTreeView* _tableView;
};

#endif // LEFTSIDEBARTREEVIEW_H
