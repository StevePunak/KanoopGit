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

    void setBold(bool value);

    bool selected() const { return _selected; }
    void setSelected(bool value);

    GIT::Reference reference() const { return _reference; }

private:
    GIT::Reference _reference;

    Label* _nameLabel = nullptr;
    Label* _rightLabel = nullptr;
    bool _selected = false;

signals:

};

#endif // LOCALBRANCHLABELWIDGET_H
