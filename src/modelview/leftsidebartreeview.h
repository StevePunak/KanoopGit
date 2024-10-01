#ifndef LEFTSIDEBARTREEVIEW_H
#define LEFTSIDEBARTREEVIEW_H
#include <Kanoop/gui/treeviewbase.h>
#include <git2qt.h>

class LeftSidebarTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    explicit LeftSidebarTreeView(QWidget* parent = nullptr);

    void createModel(GIT::Repository* repo);

private:
    GIT::Repository* _repo;

signals:
    void folderClicked(const QString& folderPath);
    void referenceClicked(const GIT::Reference& reference);
    void referenceDoubleClicked(const GIT::Reference& reference);
    void submoduleDoubleClicked();
    void refresh();

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
    void onDoubleClicked(const QModelIndex& index);
    void onExpanded(const QModelIndex& index);
    void onCollapsed(const QModelIndex& index);
};

#endif // LEFTSIDEBARTREEVIEW_H
