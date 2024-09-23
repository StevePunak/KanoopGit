#ifndef GITBRANCHTREEVIEW_H
#define GITBRANCHTREEVIEW_H
#include <Kanoop/gui/treeviewbase.h>
#include <git2qt.h>

namespace GIT {
class Repository;
}

class GitBranchTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    explicit GitBranchTreeView(QWidget* parent = nullptr);

    void createModel(GIT::Repository* repo, GIT::BranchType typeToShow);

signals:
    void folderClicked(const QString& folderPath);
    void referenceClicked(const GIT::Reference& reference);
    void refresh();

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
};

#endif // GITBRANCHTREEVIEW_H
