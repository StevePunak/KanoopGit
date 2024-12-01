#include "gitassets.h"
#include "gitentities.h"
#include "gitmainwindow.h"
#include "splashscreen.h"

#include <QTimer>

#include "gitapplication.h"
#include "repoconfig.h"
#include "settings.h"

#include <widgets/testdialog.h>

void registerMetaTypes()
{
    qRegisterMetaType<RepoConfig>();
}

int main(int argc, char *argv[])
{
    GitApplication app(argc, argv);

    registerMetaTypes();

    GuiSettings::setGlobalInstance(Settings::instance());

    GitAssets::registerAssets();
    GitEntities::registerEntityTypes();

    SplashScreen *splash = new SplashScreen;
    splash->show();

    GitMainWindow mainWindow;
    QObject::connect(&mainWindow, &GitMainWindow::preferencesChanged, &app, &GitApplication::onPreferencesChanged);
    QTimer::singleShot(1000, splash, &SplashScreen::close);
    QTimer::singleShot(1000, &mainWindow, &GitMainWindow::show);

    // TestDialog dlg;
    // dlg.show();

    return app.exec();
}

