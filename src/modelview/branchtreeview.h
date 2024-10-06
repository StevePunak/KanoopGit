#ifndef BRANCHTREEVIEW_H
#define BRANCHTREEVIEW_H
#include <Kanoop/gui/treeviewbase.h>
#include <git2qt.h>

namespace GIT {
class Repository;
}

class BranchTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    explicit BranchTreeView(QWidget* parent = nullptr);

    void createModel(GIT::Repository* repo, GIT::BranchType typeToShow);

signals:
    void folderClicked(const QString& folderPath);
    void referenceClicked(const GIT::Reference& reference);
    void referenceDoubleClicked(const GIT::Reference& reference);
    void refresh();

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
    void onDoubleClicked(const QModelIndex& index);
};

#endif // BRANCHTREEVIEW_H
