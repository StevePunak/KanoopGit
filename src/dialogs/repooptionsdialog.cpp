#include "repooptionsdialog.h"
#include "ui_repooptionsdialog.h"
#include "gitentities.h"

#include <QFileDialog>
#include <gitassets.h>
#include <settings.h>
#include <kanoopgittypes.h>

RepoOptionsDialog::RepoOptionsDialog(GIT::Repository* repo, QWidget *parent) :
    Dialog(parent),
    ui(new Ui::RepoOptionsDialog),
    _repo(repo)
{
    ui->setupUi(this);

    performLayout();

    // Sidebar setup
    ui->sidebar->addItem(GitEntities::Credentials, "Repo Credentials", GitAssets::Security);

    // Initial values
    _repoConfig = Settings::instance()->repoConfig(repo->localPath());
    if(_repoConfig.credentialsRef().isValid() == false) {
        _repoConfig.setCredentials(Settings::instance()->defaultCredentials());
    }
    ui->credentialsWidget->setCredentials(_repoConfig.credentialsRef());
    ui->sidebar->selectItem(GitEntities::Credentials);

    // Perform default validation
    connectValidationSignals();
}

RepoOptionsDialog::~RepoOptionsDialog()
{
    delete ui;
}

void RepoOptionsDialog::validate()
{
    setDirty(true);
    setValid(true);
}

void RepoOptionsDialog::applyClicked()
{
    _repoConfig.setCredentials(ui->credentialsWidget->credentials());
    Settings::instance()->saveRepoConfig(_repoConfig);
}

void RepoOptionsDialog::okClicked()
{
    applyClicked();
}


