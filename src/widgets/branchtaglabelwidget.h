#ifndef BRANCHTAGLABELWIDGET_H
#define BRANCHTAGLABELWIDGET_H

#include <QWidget>
#include <git2qt.h>

#include <modelview/gitgraphpalette.h>

class QLabel;
class ComboBox;
class BranchTagLabelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BranchTagLabelWidget(GIT::Repository* repo, const GIT::Reference::List& references, QWidget *parent = nullptr);

    QString currentReferenceFriendlyName() const;
    QString firstReferenceName() const;

private:
    void createPixmaps();
    QWidget* labelWidget() const;

    GIT::Repository* _repo;
    GIT::Reference::List _references;
    GitGraphPalette _graphPalette;

    ComboBox* _combo = nullptr;
    QLabel* _nameLabel = nullptr;
    QLabel* _lineLabel = nullptr;

    QPixmap _cloudPixmap;
    QPixmap _computerPixmap;
    QPixmap _linePixmap;
    QPixmap _noLinePixmap;

signals:
};

#endif // BRANCHTAGLABELWIDGET_H
