#ifndef BRANCHLABELWIDGET_H
#define BRANCHLABELWIDGET_H

#include <QWidget>
#include <git2qt.h>

#include <modelview/gitgraphpalette.h>

class QComboBox;
class BranchLabelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BranchLabelWidget(const GIT::Reference::List& references, QWidget *parent = nullptr);

private:
    GIT::Reference::List _references;
    GitGraphPalette _graphPalette;

    QComboBox* _combo;

signals:
};

#endif // BRANCHLABELWIDGET_H
