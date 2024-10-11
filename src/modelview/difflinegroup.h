#ifndef DIFFLINEGROUP_H
#define DIFFLINEGROUP_H

#include <QColor>

class DiffLineGroup
{
public:
    DiffLineGroup() :
        _startRow(0), _count(0)  {}
    DiffLineGroup(int row, const QChar& origin, const QColor& color) :
        _startRow(row), _origin(origin), _color(color), _count(1) {}

    int startRow() const { return _startRow; }
    QChar origin() const { return _origin; }
    QColor color() const { return _color; }
    int count() const { return _count; }

    void incrementCount() { _count++; }
    bool isValid() const { return _count > 0; }

private:
    int _startRow;
    QChar _origin;
    QColor _color;
    int _count;
};


#endif // DIFFLINEGROUP_H
