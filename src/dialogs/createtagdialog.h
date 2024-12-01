#ifndef CREATETAGDIALOG_H
#define CREATETAGDIALOG_H

#include <Kanoop/gui/dialog.h>

namespace Ui {
class CreateTagDialog;
}

class CreateTagDialog : public Dialog
{
    Q_OBJECT

public:
    explicit CreateTagDialog(bool annotated, QWidget *parent = nullptr);
    ~CreateTagDialog();

    QString name() const;
    QString message() const;

private:
    Ui::CreateTagDialog *ui;

    bool _annotated;

protected:
    virtual void validate() override;
    virtual void applyClicked() override;
    virtual void okClicked() override;
};

#endif // CREATETAGDIALOG_H
