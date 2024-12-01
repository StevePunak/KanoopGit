#include "createtagdialog.h"
#include "ui_createtagdialog.h"

CreateTagDialog::CreateTagDialog(bool annotated, QWidget *parent) :
    Dialog(parent),
    ui(new Ui::CreateTagDialog),
    _annotated(annotated)
{
    ui->setupUi(this);

    performLayout();

    setApplyEnabled(false);

    setWindowTitle(_annotated ? "Create Annotated Tag" : "Create Lightweight Tag");

    if(_annotated == false) {
        ui->labelMessage->setVisible(false);
        ui->textMessage->setVisible(false);
    }

    // Default validation
    connectValidationSignals();

    // restore font etc
    onPreferencesChanged();
}

CreateTagDialog::~CreateTagDialog()
{
    delete ui;
}

QString CreateTagDialog::name() const
{
    return ui->textName->text();
}

QString CreateTagDialog::message() const
{
    return ui->textMessage->text();
}

void CreateTagDialog::validate()
{
    setValid(ui->textName->text().isEmpty() == false);
    if(isValid() && _annotated) {
        setValid(ui->textMessage->text().isEmpty() == false);
    }
}

void CreateTagDialog::applyClicked()
{
}

void CreateTagDialog::okClicked()
{
    applyClicked();
}
