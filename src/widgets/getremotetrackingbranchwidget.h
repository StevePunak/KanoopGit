#ifndef GETREMOTETRACKINGBRANCHWIDGET_H
#define GETREMOTETRACKINGBRANCHWIDGET_H

#include <QDialog>
#include <git2qt.h>

namespace Ui {
class GetRemoteTrackingBranchWidget;
}

class GetRemoteTrackingBranchWidget : public QDialog
{
    Q_OBJECT

public:
    explicit GetRemoteTrackingBranchWidget(GIT::Repository* repo, const GIT::Reference& localBranch, QWidget *parent = nullptr);
    ~GetRemoteTrackingBranchWidget();

    QString remoteBranchName() const;

private:
    Ui::GetRemoteTrackingBranchWidget *ui;

private slots:
    void maybeEnableButtons();
};

#endif // GETREMOTETRACKINGBRANCHWIDGET_H
