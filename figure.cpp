#include "figure.h"

namespace tetris {
    figure::figure() {
        _x = 4;
        _y = -1;
        _rotation = 90;
        _type = (FigureType)(rand() % 7);
    }
    figure::figure(FigureType type) : figure() {
        _type = type;
    }
    figure::figure(const figure &A) {
        _x = A._x;
        _y = A._y;
        _rotation = A._rotation;
        _type = A._type;
    }

    int figure::width(int rotation) const {
        if (_type == FigureType::O ||
           ((_type == FigureType::L || _type == FigureType::J || _type == FigureType::T || _type == FigureType::S || _type == FigureType::Z) &&
           (rotation == 90 || rotation == 270))) return 2;
        if (_type == FigureType::I) {
            if (rotation == 0 || rotation == 180) return 4;
                else return 1;
        }
        return 3;
    }

    figure figure::operator = (const figure &A) {
        _x = A._x;
        _y = A._y;
        _rotation = A._rotation;
        _type = A._type;

        return (*this);
    }

    figure::~figure() {}
}
