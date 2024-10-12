#include "clonerepodialog.h"
#include "ui_clonerepodialog.h"

#include <QFileDialog>
#include <settings.h>
#include <kanoopgittypes.h>

#include <Kanoop/pathutil.h>

CloneRepoDialog::CloneRepoDialog(QWidget *parent) :
    Dialog(parent),
    ui(new Ui::CloneRepoDialog)
{
    ui->setupUi(this);

    performLayout();

    setApplyEnabled(false);

    // Initial values
    ui->textLocalPath->setText(Settings::instance()->lastDirectory(CloneToDirectory));
    CredentialSet credentials = Settings::instance()->defaultCredentials();
    credentials.setName("temp");
    ui->credentialsWidget->setCredentials(credentials);

    // Wiring
    connect(ui->textLocalPath, &QLineEdit::textChanged, this, &CloneRepoDialog::onLocalPathTextChanged);
    connect(ui->textUrl, &QLineEdit::textChanged, this, &CloneRepoDialog::onUrlTextChanged);
    connect(ui->pushBrowse, &QToolButton::clicked, this, &CloneRepoDialog::onBrowseClicked);

    // Default validation
    connectValidationSignals();

}

CloneRepoDialog::~CloneRepoDialog()
{
    delete ui;
}

QString CloneRepoDialog::localPath() const
{
    QString path = ui->textLocalPath->text();
    if(ui->textDirectory->text().isEmpty()) {
        QFileInfo fileInfo(ui->textUrl->text());
        path = PathUtil::combine(path, fileInfo.baseName());
    }
    else {
        path = PathUtil::combine(path, ui->textDirectory->text());
    }
    return path;
}

void CloneRepoDialog::setLocalPath(const QString& localPath)
{
    ui->textLocalPath->setText(localPath);
    ui->textLocalPath->setEnabled(false);
}

CredentialSet CloneRepoDialog::credentials() const
{
    return ui->credentialsWidget->credentials();
}

void CloneRepoDialog::validate()
{
    setDirty(true);
    setValid(
                QDir(ui->textLocalPath->text()).exists() &&
                ui->textUrl->text().isEmpty() == false
            );
}

void CloneRepoDialog::applyClicked()
{
    _url = ui->textUrl->text();
    Settings::instance()->saveLastDirectory(CloneToDirectory, ui->textLocalPath->text());
}

void CloneRepoDialog::okClicked()
{
    applyClicked();
}

void CloneRepoDialog::onUrlTextChanged()
{
    if(ui->textDirectory->text().isEmpty()) {
        QFileInfo fileInfo(ui->textUrl->text());
        ui->textDirectory->setPlaceholderText(fileInfo.baseName());
        onLocalPathTextChanged();
    }
}

void CloneRepoDialog::onLocalPathTextChanged()
{
    RepoConfig repoConfig = Settings::instance()->repoConfig(localPath());
    if(repoConfig.isValid()) {
        ui->credentialsWidget->setCredentials(repoConfig.credentialsRef());
    }
}

void CloneRepoDialog::onBrowseClicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this, "Select Directory", Settings::instance()->lastDirectory(CloneToDirectory));
    if(dirName.isEmpty() == false) {
        Settings::instance()->saveLastDirectory(CloneToDirectory, dirName);
        ui->textLocalPath->setText(dirName);
    }
}
