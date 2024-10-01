#ifndef BRANCHLABELWIDGET_H
#define BRANCHLABELWIDGET_H

#include <QWidget>
#include <git2qt.h>

#include <modelview/gitgraphpalette.h>

class ComboBox;
class BranchLabelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BranchLabelWidget(GIT::Repository* repo, const GIT::Reference::List& references, QWidget *parent = nullptr);

    QString currentReferenceFriendlyName() const;
    QString firstReferenceName() const;

private:
    void createPixmaps();

    GIT::Repository* _repo;
    GIT::Reference::List _references;
    GitGraphPalette _graphPalette;

    ComboBox* _combo;
    QPixmap _cloudPixmap;
    QPixmap _computerPixmap;
    QPixmap _linePixmap;
    QPixmap _noLinePixmap;

signals:
};

#endif // BRANCHLABELWIDGET_H
