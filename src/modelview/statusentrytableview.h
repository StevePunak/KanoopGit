#ifndef STATUSENTRYTABLEVIEW_H
#define STATUSENTRYTABLEVIEW_H
#include <Kanoop/gui/tableviewbase.h>
#include <git2qt.h>

class StatusEntryTableView : public TableViewBase
{
    Q_OBJECT
public:
    explicit StatusEntryTableView(QWidget* parent = nullptr);

    void createModel(GIT::Repository* repo, const GIT::StatusEntry::List& entries);
    GIT::StatusEntry::List entries() const;
    GIT::StatusEntry::List selectedEntries() const;
    QStringList selectedEntryPaths() const;

private:
    GIT::Repository* _repo = nullptr;

signals:
    void statusEntryClicked(const GIT::StatusEntry& statusEntry);

private slots:
    void onCurrentSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
};

#endif // STATUSENTRYTABLEVIEW_H
