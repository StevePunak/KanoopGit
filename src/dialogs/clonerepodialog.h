#ifndef CLONEREPODIALOG_H
#define CLONEREPODIALOG_H

#include <Kanoop/gui/dialog.h>

#include <credentialset.h>

namespace Ui {
class CloneRepoDialog;
}

class CloneRepoDialog : public Dialog
{
    Q_OBJECT

public:
    explicit CloneRepoDialog(QWidget *parent = nullptr);
    ~CloneRepoDialog();

    QString localPath() const;
    QString url() const { return _url; }
    CredentialSet credentials() const;

private:
    Ui::CloneRepoDialog *ui;

    QString _url;

protected:
    virtual void validate() override;
    virtual void applyClicked() override;
    virtual void okClicked() override;

private slots:
    void onUrlTextChanged();
    void onLocalPathTextChanged();
    void onBrowseClicked();
};

#endif // CLONEREPODIALOG_H
