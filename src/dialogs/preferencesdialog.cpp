#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "gitentities.h"

#include <QFileDialog>
#include <gitassets.h>
#include <settings.h>
#include <kanoopgittypes.h>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    Dialog(parent),
    ui(new Ui::PreferencesDialog),
    _credentials(Settings::instance()->credentials(CredentialSet::DefaultName))
{
    ui->setupUi(this);

    performLayout();

    // Sidebar setup
    ui->sidebar->addItem(GitEntities::Credentials, "Default Credentials", GitAssets::Security);

    // Initial values
    CredentialSet credentials = Settings::instance()->defaultCredentials();
    credentials.setName(CredentialSet::DefaultName);
    ui->credentialsWidget->setCredentials(credentials);
    ui->sidebar->selectItem(GitEntities::Credentials);

    // Perform default validation
    connectValidationSignals();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::validate()
{
    setDirty(true);
    setValid(true);
}

void PreferencesDialog::applyClicked()
{
    _credentials = ui->credentialsWidget->credentials();
    Settings::instance()->saveCredentials(_credentials);
}

void PreferencesDialog::okClicked()
{
    applyClicked();
}

