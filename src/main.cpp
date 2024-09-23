#include "gitassets.h"
#include "gitentities.h"
#include "gitmainwindow.h"

#include <QApplication>

#include "testwidgetsettings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GuiSettings::setGlobalInstance(TestWidgetSettings::instance());

    GitAssets::registerAssets();
    GitEntities::registerEntityTypes();

    GitMainWindow w;
    w.show();
    return a.exec();
}
