#ifndef REPOOPTIONSDIALOG_H
#define REPOOPTIONSDIALOG_H

#include <Kanoop/gui/dialog.h>
#include <credentialset.h>
#include <git2qt.h>
#include <repoconfig.h>

namespace Ui {
class RepoOptionsDialog;
}

class RepoOptionsDialog : public Dialog
{
    Q_OBJECT

public:
    explicit RepoOptionsDialog(GIT::Repository* repo, QWidget *parent = nullptr);
    ~RepoOptionsDialog();

private:
    Ui::RepoOptionsDialog *ui;
    GIT::Repository* _repo;

    RepoConfig _repoConfig;

protected:
    virtual void validate() override;
    virtual void applyClicked() override;
    virtual void okClicked() override;

private slots:
};

#endif // REPOOPTIONSDIALOG_H
