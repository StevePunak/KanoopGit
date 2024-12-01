#ifndef AMENDCOMMITMESSAGEDIALOG_H
#define AMENDCOMMITMESSAGEDIALOG_H

#include <Kanoop/gui/dialog.h>
#include <git2qt.h>

namespace Ui {
class AmendCommitMessageDialog;
}

class AmendCommitMessageDialog : public Dialog
{
    Q_OBJECT

public:
    explicit AmendCommitMessageDialog(const GIT::Commit& commit, QWidget *parent = nullptr);
    ~AmendCommitMessageDialog();

    GIT::Commit commit() const { return _commit; }
    QString message() const;

private:
    Ui::AmendCommitMessageDialog *ui;

    GIT::Commit _commit;

protected:
    virtual void validate() override;
    virtual void applyClicked() override;
    virtual void okClicked() override;
};

#endif // AMENDCOMMITMESSAGEDIALOG_H
