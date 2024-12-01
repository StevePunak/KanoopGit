#include "gitgraphpalette.h"

#include <settings.h>

namespace Colors = QColorConstants::Svg;


QColor GitGraphPalette::textColor() const
{
    return Colors::black;
}

QColor GitGraphPalette::branchOrTagColor() const
{
    return Colors::lightseagreen;
}

QColor GitGraphPalette::mergeDotColor() const
{
    return Colors::blue;
}

QColor GitGraphPalette::commitDotBorderColor() const
{
    return Colors::darkgreen;
}

QColor GitGraphPalette::commitDotFillColor() const
{
    return Colors::darkorange;
}

QColor GitGraphPalette::graphLineColor() const
{
    return Settings::instance()->isDarkMode()
            ? Colors::lightblue
            : Colors::blue;
}

QColor GitGraphPalette::headCommitLineColor() const
{
    return branchOrTagColor();
}

QColor GitGraphPalette::stashBorderColor() const
{
    return Colors::darkmagenta;
}

QColor GitGraphPalette::stashFillColor() const
{
    return Colors::white;
}

QColor GitGraphPalette::leftSidebarTitleColor() const
{
    return Settings::instance()->isDarkMode()
            ? Colors::lightblue
            : Colors::blue;
}

QColor GitGraphPalette::submoduleNeedsRefreshColor() const
{
    return Settings::instance()->isDarkMode()
            ? Colors::orange
            : Colors::darkorange;
}

QColor GitGraphPalette::submoduleUninitializedColor() const
{
    return Settings::instance()->isDarkMode()
            ? Colors::red
            : Colors::darkred;
}

QColor GitGraphPalette::diffPlusColor() const
{
    return Settings::instance()->isDarkMode()
            ? Colors::darkgreen
            : Colors::lightgreen;
}

QColor GitGraphPalette::diffMinusColor() const
{
    return Settings::instance()->isDarkMode()
            ? Colors::darksalmon
            : Colors::lightsalmon;
}
