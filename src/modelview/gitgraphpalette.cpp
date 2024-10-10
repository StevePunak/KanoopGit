#include "gitgraphpalette.h"

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
    return Colors::blue;
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
