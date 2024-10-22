#ifndef BRANCHTAGLABELWIDGET_H
#define BRANCHTAGLABELWIDGET_H

#include <QWidget>
#include <git2qt.h>

#include <modelview/gitgraphpalette.h>

class IconLabel;
class QLabel;
class ComboBox;
class BranchTagLabelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BranchTagLabelWidget(GIT::Repository* repo,
                                  const GIT::ReferenceList& references,
                                  const GIT::AnnotatedTag::List& annotatedTags,
                                  const GIT::LightweightTag::List& lightweightTags,
                                  QWidget *parent = nullptr);

    QString currentReferenceFriendlyName() const;
    QString firstReferenceName() const;

private:
    void createPixmaps();
    QWidget* labelWidget() const;

    GIT::Repository* _repo;
    GIT::ReferenceList _references;
    GIT::AnnotatedTag::List _annotatedTags;
    GIT::LightweightTag::List _lightweightTags;
    GitGraphPalette _graphPalette;

    ComboBox* _combo = nullptr;
    IconLabel* _nameLabel = nullptr;
    QLabel* _lineLabel = nullptr;

    QPixmap _cloudPixmap;
    QPixmap _computerPixmap;
    QPixmap _tagPixmap;
    QPixmap _linePixmap;
    QPixmap _noLinePixmap;

signals:
};

#endif // BRANCHTAGLABELWIDGET_H
