#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <Kanoop/gui/dialog.h>
#include <credentialset.h>
#include <kanoopgittypes.h>


class QRadioButton;
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
    PaletteType paletteTypeFromButtons() const;
    QRadioButton* buttonFromPaletteType(PaletteType type) const;

    Ui::PreferencesDialog *ui;

    CredentialSet _credentials;

protected:
    virtual void validate() override;
    virtual void applyClicked() override;
    virtual void okClicked() override;

signals:
    void preferencesChanged();

private slots:
    void onSidebarItemClicked(int entityType);
};

#endif // PREFERENCESDIALOG_H
