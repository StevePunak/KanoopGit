#ifndef DIFFSCROLLBAR_H
#define DIFFSCROLLBAR_H

#include <QProxyStyle>
#include <QScrollBar>

#include <modelview/difflinegroup.h>

class DiffScrollProxyStyle;
class DiffScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    explicit DiffScrollBar(QWidget *parent = nullptr);

    void setLineGroups(const QList<DiffLineGroup>& value, int rowCount);

private:
    DiffScrollProxyStyle* _proxyStyle;

signals:
};

class DiffScrollProxyStyle : public QProxyStyle
{
public:
    DiffScrollProxyStyle(QStyle* style = nullptr) :
        QProxyStyle(style),
        _rowCount(0) {}

    void setLineGroups(const QList<DiffLineGroup>& value, int rowCount);

    virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const override;

private:
    QList<DiffLineGroup> _lineGroups;
    int _rowCount;
};

#endif // DIFFSCROLLBAR_H
