#ifndef LOCALBRANCHLABELWIDGET_H
#define LOCALBRANCHLABELWIDGET_H

#include <QWidget>
#include <git2qt.h>

class Label;
class LocalBranchLabelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LocalBranchLabelWidget(GIT::Repository* repo, const GIT::Reference& reference, QWidget *parent = nullptr);

private:
    GIT::Reference _reference;

    Label* _nameLabel = nullptr;
    Label* _rightLabel = nullptr;

signals:

};

#endif // LOCALBRANCHLABELWIDGET_H
