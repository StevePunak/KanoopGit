#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "gitentities.h"

#include <QFileDialog>
#include <gitassets.h>
#include <settings.h>

#include <Kanoop/gui/palette.h>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    Dialog(parent),
    ui(new Ui::PreferencesDialog),
    _credentials(Settings::instance()->credentials(CredentialSet::DefaultName))
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, true);

    performLayout();

    // Sidebar setup
    ui->sidebar->addItem(GitEntities::Display, "Display", GitAssets::Computer);
    ui->sidebar->addItem(GitEntities::Credentials, "Default Credentials", GitAssets::Security);
    connect(ui->sidebar, &SidebarWidget::itemClicked, this, &PreferencesDialog::onSidebarItemClicked);

    // Initial values
    CredentialSet credentials = Settings::instance()->defaultCredentials();
    credentials.setName(CredentialSet::DefaultName);
    ui->credentialsWidget->setCredentials(credentials);
    ui->sidebar->selectItem(GitEntities::Credentials);
    ui->spinFontSize->setValue(Settings::instance()->fontSize());
    buttonFromPaletteType(Settings::instance()->paletteType())->setChecked(true);

    // Perform default validation
    connectValidationSignals();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

PaletteType PreferencesDialog::paletteTypeFromButtons() const
{
    PaletteType result;
    if(ui->radioFusionLight->isChecked()) {
        result = FusionLight;
    }
    else if(ui->radioFusionDark->isChecked()) {
        result = FusionDark;
    }
    else {
        result = SystemPalette;
    }
    return result;
}

QRadioButton* PreferencesDialog::buttonFromPaletteType(PaletteType type) const
{
    QRadioButton* result = ui->radioSystemPalette;
    switch(type) {
    case FusionLight:
        result = ui->radioFusionLight;
        break;
    case FusionDark:
        result = ui->radioFusionDark;
        break;
    case SystemPalette:
    default:
        result = ui->radioSystemPalette;
    }
    return result;
}

void PreferencesDialog::validate()
{
    setDirty(
        paletteTypeFromButtons() != Settings::instance()->paletteType() ||
        ui->spinFontSize->value() != Settings::instance()->fontSize() ||
        ui->credentialsWidget->credentials() != Settings::instance()->credentials(ui->credentialsWidget->credentials().name())
    );
    setValid(true);
}

void PreferencesDialog::applyClicked()
{
    _credentials = ui->credentialsWidget->credentials();
    Settings::instance()->saveCredentials(_credentials);
    Settings::instance()->setFontSize(ui->spinFontSize->value());
    Settings::instance()->setPaletteType(paletteTypeFromButtons());
    emit preferencesChanged();
}

void PreferencesDialog::okClicked()
{
    applyClicked();
}

void PreferencesDialog::onSidebarItemClicked(int entityType)
{
    switch(entityType) {
    case GitEntities::Display:
        ui->stackedWidget->setCurrentWidget(ui->pageDisplay);
        break;
    case GitEntities::Credentials:
        ui->stackedWidget->setCurrentWidget(ui->pageCredentials);
        break;
    default:
        break;
    }
}

