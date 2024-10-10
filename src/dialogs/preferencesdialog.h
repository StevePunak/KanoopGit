#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <Kanoop/gui/dialog.h>
#include <credentialset.h>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public Dialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

private:
    Ui::PreferencesDialog *ui;

    CredentialSet _credentials;

protected:
    virtual void validate() override;
    virtual void applyClicked() override;
    virtual void okClicked() override;

private slots:
};

#endif // PREFERENCESDIALOG_H
