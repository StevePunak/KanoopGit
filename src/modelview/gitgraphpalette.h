#ifndef GITGRAPHPALETTE_H
#define GITGRAPHPALETTE_H
#include <QColor>

class GitGraphPalette
{
public:
    GitGraphPalette() {}

    QColor textColor() const;
    QColor branchOrTagColor() const;
    QColor mergeDotColor() const;
    QColor commitDotBorderColor() const;
    QColor commitDotFillColor() const;
    QColor graphLineColor() const;
    QColor headCommitLineColor() const;
    QColor stashBorderColor() const;
    QColor stashFillColor() const;
    int headCommitLineWidth() const { return 2; }

};

#endif // GITGRAPHPALETTE_H
