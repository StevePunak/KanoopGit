#include "credentialswidget.h"
#include "ui_credentialswidget.h"

#include <QFileDialog>
#include <settings.h>
#include <kanoopgittypes.h>

#include <Kanoop/pathutil.h>

CredentialsWidget::CredentialsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CredentialsWidget),
    _credentialsName(CredentialSet::DefaultName)
{
    ui->setupUi(this);

    // Wiring
    connect(ui->pushBrowsePublicKey, &QToolButton::clicked, this, &CredentialsWidget::onBrowsePublicKeyClicked);
    connect(ui->pushBrowsePrivateKey, &QToolButton::clicked, this, &CredentialsWidget::onBrowsePrivateKeyClicked);
}

CredentialsWidget::~CredentialsWidget()
{
    delete ui;
}

CredentialSet CredentialsWidget::credentials() const
{
    CredentialSet result;
    result.setName(_credentialsName);
    result.setUsername(ui->textUserName->text());
    result.setPassword(ui->textPassword->text());
    result.setPrivateKeyFilename(ui->textPrivateKey->text());
    result.setPublicKeyFilename(ui->textPublicKey->text());
    return result;
}

void CredentialsWidget::setCredentials(const CredentialSet& credentials)
{
    ui->textUserName->setText(credentials.username());
    ui->textPassword->setText(credentials.password());
    ui->textPublicKey->setText(credentials.publicKeyFilename());
    ui->textPrivateKey->setText(credentials.privateKeyFilename());
    _credentialsName = credentials.name();
}

void CredentialsWidget::onBrowsePublicKeyClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Public Key File", Settings::instance()->lastDirectory(SshPublicKey), "Public Key Files (*.pub)");
    if(filename.isEmpty() == false) {
        QFileInfo fileInfo(filename);
        ui->textPublicKey->setText(filename);
        Settings::instance()->saveLastDirectory(SshPublicKey, fileInfo.absolutePath());

        QString privateKeyPath = PathUtil::combine(fileInfo.absolutePath(), fileInfo.baseName());
        if(QFile::exists(privateKeyPath)) {
            ui->textPrivateKey->setText(privateKeyPath);
        }
    }
}

void CredentialsWidget::onBrowsePrivateKeyClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Private Key File", Settings::instance()->lastDirectory(SshPrivateKey));
    if(filename.isEmpty() == false) {
        QFileInfo fileInfo(filename);
        ui->textPrivateKey->setText(filename);
        Settings::instance()->saveLastDirectory(SshPrivateKey, fileInfo.absolutePath());
    }
}
