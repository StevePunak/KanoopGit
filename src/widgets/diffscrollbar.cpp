#include "diffscrollbar.h"

#include <QPaintEvent>
#include <QPainter>
#include <QStyleFactory>
#include <QStyleOptionSlider>
#include <Kanoop/log.h>
#include <Kanoop/geometry/rectangle.h>

DiffScrollBar::DiffScrollBar(QWidget *parent) :
    QScrollBar(Qt::Vertical, parent)
{
    setObjectName(DiffScrollBar::metaObject()->className());
    QStyle* newStyle = QStyleFactory::create(style()->name());
    if(newStyle == nullptr) {
        return;
    }

    _proxyStyle = new DiffScrollProxyStyle(newStyle);
    newStyle->setParent(this);
    _proxyStyle->setParent(this);
    setStyle(_proxyStyle);
}

void DiffScrollBar::setLineGroups(const QList<DiffLineGroup>& value, int rowCount)
{
    _proxyStyle->setLineGroups(value, rowCount);
}

void DiffScrollProxyStyle::setLineGroups(const QList<DiffLineGroup>& value, int rowCount)
{
    _lineGroups = value;
    _rowCount = rowCount;
}

void DiffScrollProxyStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt, QPainter* painter, const QWidget* widget) const
{
    QProxyStyle::drawComplexControl(cc, opt, painter, widget);
    const QStyleOptionSlider* option = qstyleoption_cast<const QStyleOptionSlider*>(opt);
    if(option == nullptr || _lineGroups.count() == 0) {
        return;
    }

    painter->save();

    Rectangle sliderRect = baseStyle()->subControlRect(QStyle::CC_ScrollBar, opt, QStyle::SC_ScrollBarSlider, (QWidget*)option->styleObject);
    Rectangle upButtonRect = baseStyle()->subControlRect(QStyle::CC_ScrollBar, opt, QStyle::SC_ScrollBarSubLine, (QWidget*)option->styleObject);
    Rectangle downButtonRect = baseStyle()->subControlRect(QStyle::CC_ScrollBar, opt, QStyle::SC_ScrollBarAddLine, (QWidget*)option->styleObject);
    Rectangle grooveRect = baseStyle()->subControlRect(QStyle::CC_ScrollBar, opt, QStyle::SC_ScrollBarGroove, (QWidget*)option->styleObject);

    double areaHeight = (grooveRect.height() - upButtonRect.height()) - downButtonRect.height();
    Rectangle areaRect(upButtonRect.x(), upButtonRect.bottom() + 1, grooveRect.width(), areaHeight);

    QRegion clipRegion(areaRect.toRect());
    clipRegion = clipRegion.subtracted(QRegion(sliderRect.toRect()));
    painter->setClipRegion(clipRegion);
    painter->setClipping(true);

    double rows = _rowCount;
    for(const DiffLineGroup& lineGroup : _lineGroups) {
        double startPercent = (double)lineGroup.startRow() / rows;
        double endPercent = (double)(lineGroup.startRow() + lineGroup.count()) / rows;

        double rectStartY = std::floor(areaRect.height() * startPercent) + areaRect.y();
        double rectHeight = std::ceil((areaRect.height() * endPercent) - rectStartY);
        Rectangle drawRect(areaRect.x(), rectStartY, areaRect.width(), rectHeight);
        painter->fillRect(drawRect, lineGroup.color());
    }

    painter->restore();
}

