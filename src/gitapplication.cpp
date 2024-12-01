#include "gitapplication.h"

#include "settings.h"

#include <Kanoop/gui/palette.h>
#include <QStyle>

GitApplication::GitApplication(int& argc, char* argv[]) :
    Application(argc, argv, "KanoopGit")
{
    setOrganizationName("Kanoop");
    setApplicationDisplayName("Kanoop Git");
    setApplicationVersion(QT_STRINGIFY(KANOOPGIT_VERSION));

    onPreferencesChanged();
}

void GitApplication::onPreferencesChanged()
{
    switch(Settings::instance()->paletteType())
    {
    case FusionDark:
        setPalette(Palette::fusionDark());
        break;
    case FusionLight:
        setPalette(Palette::fusionLight());
        break;
    default:
        setPalette(style()->standardPalette());
        break;
    }
}
