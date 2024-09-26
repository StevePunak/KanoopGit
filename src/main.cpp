#include "gitassets.h"
#include "gitentities.h"
#include "gitmainwindow.h"

#include <QApplication>

#include "kanoopgitsettings.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("Kanoop");
    app.setApplicationDisplayName("Kanoop Git");

    GuiSettings::setGlobalInstance(KanoopGitSettings::instance());

    GitAssets::registerAssets();
    GitEntities::registerEntityTypes();

    GitMainWindow w;
    w.show();
    return app.exec();
}
