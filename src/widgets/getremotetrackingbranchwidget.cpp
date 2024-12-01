#include "getremotetrackingbranchwidget.h"
#include "ui_getremotetrackingbranchwidget.h"

#include <Kanoop/pathutil.h>

using namespace GIT;

GetRemoteTrackingBranchWidget::GetRemoteTrackingBranchWidget(GIT::Repository* repo, const Reference& localBranch, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GetRemoteTrackingBranchWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    for(const Remote& remote : repo->remotes()) {
        ui->comboRemotes->addItem(remote.name(), remote.toVariant());
    }
    if(ui->comboRemotes->count() > 0) {
        ui->comboRemotes->setCurrentIndex(0);
    }
    ui->textPrompt->setText(QString("What remote branch should %1 push and pull to?").arg(localBranch.friendlyName()));
    ui->textBranchName->setPlaceholderText(localBranch.friendlyName());

    connect(ui->pushSubmit, &QPushButton::clicked, this, &GetRemoteTrackingBranchWidget::accept);
    connect(ui->pushCancel, &QPushButton::clicked, this, &GetRemoteTrackingBranchWidget::reject);
    connect(ui->comboRemotes, &QComboBox::currentIndexChanged, this, &GetRemoteTrackingBranchWidget::maybeEnableButtons);
    connect(ui->textBranchName, &QLineEdit::textChanged, this, &GetRemoteTrackingBranchWidget::maybeEnableButtons);
}

GetRemoteTrackingBranchWidget::~GetRemoteTrackingBranchWidget()
{
    delete ui;
}

QString GetRemoteTrackingBranchWidget::remoteBranchName() const
{
    QString branchName = ui->textBranchName->text().isEmpty()
                         ? ui->textBranchName->placeholderText()
                         : ui->textBranchName->text();
    return PathUtil::combine(ui->comboRemotes->currentText(), branchName);
}

void GetRemoteTrackingBranchWidget::maybeEnableButtons()
{
    ui->pushSubmit->setEnabled(ui->comboRemotes->currentText().isEmpty() == false);
}

