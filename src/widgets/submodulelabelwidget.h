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

public slots:
    void setSpinning(bool value);

    void setSpinnerVisible(bool value);
    void hideSpinnerIn(const TimeSpan& delay);

    void setSpinnerValue(int value);

private:
    virtual void paintEvent(QPaintEvent* event) override;

    GIT::Repository* _submoduleRepo = nullptr;
    GIT::Submodule _submodule;
    int _commitsBehind = 0;

    SpinnerWidget* _spinner = nullptr;
    Label* _nameLabel = nullptr;
    Label* _rightLabel = nullptr;
    QIcon _icon;
};

#endif // SUBMODULELABELWIDGET_H
