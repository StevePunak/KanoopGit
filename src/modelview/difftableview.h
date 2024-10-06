#ifndef DIFFTABLEVIEW_H
#define DIFFTABLEVIEW_H
#include <Kanoop/gui/tableviewbase.h>
#include <git2qt.h>

class DiffTableView : public TableViewBase
{
    Q_OBJECT
public:
    explicit DiffTableView(QWidget* parent = nullptr);

    void createModelTreeToTree(GIT::Repository* repo, const GIT::Tree& oldTree, const GIT::Tree& newTree, const GIT::DiffDelta& delta);
    void createModelIndexToWorkDir(GIT::Repository* repo, const GIT::DiffDelta& delta);

    void scrollToNextDelta();
    void scrollToPreviousDelta();

    bool hasNextDelta() const;
    bool hasPreviousDelta() const;

    GIT::DiffDelta delta() const { return _delta; }

private:
    GIT::DiffDelta _delta;
    int _lastDeltaRow = -1;

signals:

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
};

#endif // DIFFTABLEVIEW_H
