#ifndef SUBMODULELABELWIDGET_H
#define SUBMODULELABELWIDGET_H

#include <QWidget>
#include <git2qt.h>

class Label;
class SpinnerWidget;
class QLabel;
class SubmoduleLabelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SubmoduleLabelWidget(GIT::Repository* repo, const GIT::Submodule& submodule, QWidget *parent = nullptr);
    virtual ~SubmoduleLabelWidget();

    bool isSpinning() const;
    int spinnerValue() const;

    bool isSelected() const { return _selected; }

    bool paintComplete() const { return _paintComplete; }
    void setPaintComplete(bool value) { _paintComplete = value; }

public slots:
    void setSpinning(bool value);
    void setSelected(bool value);

    void setSpinnerVisible(bool value);
    void hideSpinnerIn(const TimeSpan& delay);

    void setSpinnerValue(int value);

private:
    void setForegroundColor(const QColor& color);
    void setBackgroundColor(const QColor& color);

    GIT::Repository* _submoduleRepo = nullptr;
    GIT::Submodule _submodule;
    bool _indexAddedButNotInHead = false;
    bool _selected = false;
    bool _paintComplete = false;

    SpinnerWidget* _spinner = nullptr;
    Label* _iconLabel = nullptr;
    Label* _nameLabel = nullptr;
    Label* _rightLabel = nullptr;
    QIcon _icon;
};

#endif // SUBMODULELABELWIDGET_H
