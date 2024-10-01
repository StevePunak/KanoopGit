#include "gitassets.h"
#include "gitentities.h"
#include "gitmainwindow.h"
#include "splashscreen.h"

#include <QApplication>
#include <QTimer>

#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("Kanoop");
    app.setApplicationDisplayName("Kanoop Git");
    app.setApplicationVersion(QT_STRINGIFY(KANOOPGIT_VERSION));

    GuiSettings::setGlobalInstance(Settings::instance());

    GitAssets::registerAssets();
    GitEntities::registerEntityTypes();

    SplashScreen *splash = new SplashScreen;
    splash->show();

    GitMainWindow mainWindow;
    QTimer::singleShot(1000, splash, &SplashScreen::close);
    QTimer::singleShot(1000, &mainWindow, &GitMainWindow::show);

    return app.exec();
}
