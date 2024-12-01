#include "amendcommitmessagedialog.h"
#include "ui_amendcommitmessagedialog.h"

using namespace GIT;

AmendCommitMessageDialog::AmendCommitMessageDialog(const GIT::Commit& commit, QWidget *parent) :
    Dialog(parent),
    ui(new Ui::AmendCommitMessageDialog),
    _commit(commit)
{
    ui->setupUi(this);

    performLayout();
    setApplyEnabled(false);

    ui->textMessage->setPlainText(commit.message());

    connect(ui->textMessage, &QPlainTextEdit::textChanged, this, &AmendCommitMessageDialog::validate);
}

AmendCommitMessageDialog::~AmendCommitMessageDialog()
{
    delete ui;
}

QString AmendCommitMessageDialog::message() const
{
    return ui->textMessage->toPlainText();
}

void AmendCommitMessageDialog::validate()
{
    setDirty(message() != _commit.message());
    setValid(message().isEmpty() == false);
}

void AmendCommitMessageDialog::applyClicked()
{
}

void AmendCommitMessageDialog::okClicked()
{
    applyClicked();
}
