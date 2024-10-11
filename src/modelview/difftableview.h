#ifndef DIFFTABLEVIEW_H
#define DIFFTABLEVIEW_H
#include <Kanoop/gui/tableviewbase.h>
#include <git2qt.h>

class DiffScrollBar;
class DiffTableView : public TableViewBase
{
    Q_OBJECT
public:
    explicit DiffTableView(QWidget* parent = nullptr);

    void createModelForSingleDelta(GIT::Repository* repo, const GIT::TreeEntry& oldEntry, const GIT::TreeEntry& newEntry, const GIT::DiffDelta& delta);
    void createModelIndexToWorkDir(GIT::Repository* repo, const GIT::DiffDelta& delta);

    void scrollToNextDelta();
    void scrollToPreviousDelta();

    bool hasNextDelta() const;
    bool hasPreviousDelta() const;

    GIT::DiffDelta delta() const { return _delta; }

private:
    GIT::DiffDelta _delta;
    int _lastDeltaRow = -1;
    DiffScrollBar* _scrollBar;

signals:

private slots:
    void onCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
};

#endif // DIFFTABLEVIEW_H
