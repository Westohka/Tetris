#include "game.h"

namespace tetris {
    /// Конструкторы
    game::game() {
        _score = 0;
        _lvl = 1;
        _count = 0;
        _swap = true;
        _lose = false;
        _show = true;
        _selected = true;

        _figure = (FigureType)(rand() % 7);
        _figureNext = (FigureType)(rand() % 7);
        if (_figure._type == _figureNext) _figureNext = (FigureType)(rand() % 7);

        _field = new Color [200];
        for (int i = 0; i < 200; i++)
            _field[i] = (Color)0;
    }
    game::game(const game &A) {
        _score = A._score;
        _lvl = A._lvl;
        _swap = A._swap;
        _lose = A._lose;
        _count = A._count;
        _show = A._show;
        _selected = A._selected;

        _figure = A._figure;
        _figureNext = A._figureNext;

        _field = new Color [200];
        for (int i = 0; i < 200; i++)
            _field[i] = A._field[i];
    }

    void game::moveLeft() {
        if (checkFree(_figure._x - 1, _figure._y, _figure._rotation)) _figure._x--;
    }
    void game::moveRight() {
        if (checkFree(_figure._x + 1, _figure._y, _figure._rotation)) _figure._x++;
    }
    void game::moveDown() {
        if (!checkFree(_figure._x, _figure._y + 1, _figure._rotation)) figureStop();
            else _figure._y++;
    }
    void game::rotate() {
        if (_figure._type == FigureType::O) return;

        if (!checkFree(_figure._x, _figure._y, _figure._rotation + 90)) {
            for (int i = 1; i < _figure.width(_figure._rotation + 90); i++) {
                if (checkFree(_figure._x - i, _figure._y, _figure._rotation + 90)) {
                    _figure._x -= i;
                    break;
                } else if (i == (_figure.width(_figure._rotation + 90) - 1)) return;
            }
        }

        if (_figure._type == FigureType::I || _figure._type == FigureType::S || _figure._type == FigureType::Z) {
            if (_figure._rotation == 0) _figure._rotation = 90;
                else _figure._rotation = 0;
        } else if (_figure._type != FigureType::O) {
            _figure._rotation += 90;
            if (_figure._rotation >= 360) _figure._rotation = 0;
        }
    }

    bool game::checkFree(int x, int y, int rotation) const {
        if (y == 20 || x <= -1) return false;
        if (x + _figure.width(rotation) > 10) return false;

        if (_figure._type == FigureType::O &&
            ((y >= 0 && ((*this)[y][x] != Color::None || (*this)[y][x + 1] != Color::None)) ||
            (y > 0 && ((*this)[y - 1][x] != Color::None || (*this)[y - 1][x + 1] != Color::None))))
            return false;
        else if (_figure._type == FigureType::I) {
            if ((rotation == 0 || rotation == 180) && y >= 0 &&
                ((*this)[y][x] != Color::None || (*this)[y][x + 1] != Color::None || (*this)[y][x + 2] != Color::None || (*this)[y][x + 3] != Color::None))
                return false;
            if (rotation == 90 &&
                ((y >= 0 && (*this)[y][x] != Color::None) || (y >= 1 && (*this)[y - 1][x] != Color::None) ||
                (y >= 2 && (*this)[y - 2][x] != Color::None) || (y >= 3 && (*this)[y - 3][x] != Color::None)))
                return false;
        } else if (_figure._type == FigureType::S) {
            if ((rotation == 0 || rotation == 180) && y >= 0 &&
                (((*this)[y][x] != Color::None || (*this)[y][x + 1] != Color::None) ||
                (y > 0 && ((*this)[y - 1][x + 1] != Color::None || (*this)[y - 1][x + 2] != Color::None))))
                return false;
            if (rotation == 90 &&
                ((y >= 0 && (*this)[y][x + 1] != Color::None) || (y >= 2 && (*this)[y - 2][x] != Color::None) ||
                (y >= 1 && ((*this)[y - 1][x + 1] != Color::None || (*this)[y - 1][x] != Color::None))))
                return false;
        } else if (_figure._type == FigureType::Z) {
            if ((rotation == 0 || rotation == 180) && y >= 0 &&
                (((*this)[y][x + 2] != Color::None || (*this)[y][x + 1] != Color::None) ||
                (y > 0 && ((*this)[y - 1][x + 1] != Color::None || (*this)[y - 1][x] != Color::None))))
                return false;
            if (rotation == 90 &&
                ((y >= 0 && (*this)[y][x] != Color::None) || (y >= 2 && (*this)[y - 2][x + 1] != Color::None) ||
                (y >= 1 && ((*this)[y - 1][x + 1] != Color::None || (*this)[y - 1][x] != Color::None))))
                return false;
        } else if (_figure._type == FigureType::L) {
            if ((rotation == 0 || rotation == 360) && y >= 0 &&
                (((*this)[y][x] != Color::None || (*this)[y][x + 1] != Color::None || (*this)[y][x + 2] != Color::None) ||
                (y > 0 && (*this)[y - 1][x + 2] != Color::None)))
                return false;
            if (rotation == 180 &&
                ((y >= 1 && ((*this)[y - 1][x] != Color::None || (*this)[y - 1][x + 1] != Color::None || (*this)[y - 1][x + 2] != Color::None)) ||
                (y >= 0 && (*this)[y][x] != Color::None)))
                return false;
            if (rotation == 90 &&
                ((y >= 0 && ((*this)[y][x] != Color::None || (*this)[y][x + 1] != Color::None)) ||
                (y >= 1 && (*this)[y - 1][x] != Color::None) || (y >= 2 && (*this)[y - 2][x] != Color::None)))
                return false;
            if (rotation == 270 &&
                ((y >= 2 && ((*this)[y - 2][x] != Color::None || (*this)[y - 2][x + 1] != Color::None)) ||
                (y >= 0 && (*this)[y][x + 1] != Color::None) || (y >= 1 && (*this)[y - 1][x + 1] != Color::None)))
                return false;
        } else if (_figure._type == FigureType::J) {
            if ((rotation == 0 || rotation == 360) && y >= 0 &&
                (((*this)[y][x] != Color::None || (*this)[y][x + 1] != Color::None || (*this)[y][x + 2] != Color::None) ||
                (y > 0 && (*this)[y - 1][x] != Color::None)))
                return false;
            if (rotation == 180 &&
                ((y >= 1 &&  ((*this)[y - 1][x] != Color::None || (*this)[y - 1][x + 1] != Color::None || (*this)[y - 1][x + 2] != Color::None)) ||
                (y >= 0 && (*this)[y][x + 2] != Color::None)))
                return false;
            if (rotation == 90 &&
                ((y >= 2 && ((*this)[y - 2][x] != Color::None || (*this)[y - 2][x + 1] != Color::None)) ||
                (y >= 1 && (*this)[y - 1][x] != Color::None) || (y >= 0 && (*this)[y][x] != Color::None)))
                return false;
            if (rotation == 270 &&
                ((y >= 0 && ((*this)[y][x] != Color::None || (*this)[y][x + 1] != Color::None)) ||
                (y >= 2 && (*this)[y - 2][x + 1] != Color::None) || (y >= 1 && (*this)[y - 1][x + 1] != Color::None)))
                return false;
        } else if (_figure._type == FigureType::T) {
            if ((rotation == 0 || rotation == 360) && y >= 0 &&
                (((*this)[y][x] != Color::None || (*this)[y][x + 1] != Color::None || (*this)[y][x + 2] != Color::None) ||
                (y > 0 && (*this)[y - 1][x + 1] != Color::None)))
                return false;
            if (rotation == 180 &&
                ((y >= 1 &&  ((*this)[y - 1][x] != Color::None || (*this)[y - 1][x + 1] != Color::None || (*this)[y - 1][x + 2] != Color::None)) ||
                (y >= 0 && (*this)[y][x + 1] != Color::None)))
                return false;
            if (rotation == 90 &&
                ((y >= 1 && ((*this)[y - 1][x] != Color::None || (*this)[y - 1][x + 1] != Color::None)) ||
                (y >= 2 && (*this)[y - 2][x] != Color::None) || (y >= 0 && (*this)[y][x] != Color::None)))
                return false;
            if (rotation == 270 &&
                ((y >= 1 && ((*this)[y - 1][x] != Color::None || (*this)[y - 1][x + 1] != Color::None)) ||
                (y >= 2 && (*this)[y - 2][x + 1] != Color::None) || (y >= 0 && (*this)[y][x + 1] != Color::None)))
                return false;
        }

        return true;
    }

    void game::figureStop() {
        if (_figure._y < 0) {
            _lose = true;
            return;
        }

        if (_figure._type == FigureType::O) {
            if (_figure._y < 1) {
                _lose = true;
                return;
            }
            (*this)[_figure._y][_figure._x] = Color::Yellow;
            (*this)[_figure._y][_figure._x + 1] = Color::Yellow;
            (*this)[_figure._y - 1][_figure._x] = Color::Yellow;
            (*this)[_figure._y - 1][_figure._x + 1] = Color::Yellow;
        } else if (_figure._type == FigureType::I) {
            if (_figure._rotation == 0) {
                (*this)[_figure._y][_figure._x] = Color::Cyan;
                (*this)[_figure._y][_figure._x + 1] = Color::Cyan;
                (*this)[_figure._y][_figure._x + 2] = Color::Cyan;
                (*this)[_figure._y][_figure._x + 3] = Color::Cyan;
            } else {
                if (_figure._y < 3) {
                    _lose = true;
                    return;
                }
                (*this)[_figure._y][_figure._x] = Color::Cyan;
                (*this)[_figure._y - 1][_figure._x] = Color::Cyan;
                (*this)[_figure._y - 2][_figure._x] = Color::Cyan;
                (*this)[_figure._y - 3][_figure._x] = Color::Cyan;
            }
        } else if (_figure._type == FigureType::S) {
            if ((_figure._rotation == 0 && _figure._y < 1) || (_figure._rotation == 90 && _figure._y < 2)) {
                _lose = true;
                return;
            }
            if (_figure._rotation == 0) {
                (*this)[_figure._y][_figure._x] = Color::Green;
                (*this)[_figure._y][_figure._x + 1] = Color::Green;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Green;
                (*this)[_figure._y - 1][_figure._x + 2] = Color::Green;
            } else {
                (*this)[_figure._y][_figure._x + 1] = Color::Green;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Green;
                (*this)[_figure._y - 1][_figure._x] = Color::Green;
                (*this)[_figure._y - 2][_figure._x] = Color::Green;
            }
        } else if (_figure._type == FigureType::Z) {
            if ((_figure._rotation == 0 && _figure._y < 1) || (_figure._rotation == 90 && _figure._y < 2)) {
                _lose = true;
                return;
            }
            if (_figure._rotation == 0) {
                (*this)[_figure._y - 1][_figure._x] = Color::Red;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Red;
                (*this)[_figure._y][_figure._x + 1] = Color::Red;
                (*this)[_figure._y][_figure._x + 2] = Color::Red;
            } else {
                (*this)[_figure._y][_figure._x] = Color::Red;
                (*this)[_figure._y - 1][_figure._x] = Color::Red;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Red;
                (*this)[_figure._y - 2][_figure._x + 1] = Color::Red;
            }
        } else if (_figure._type == FigureType::J) {
            if (((_figure._rotation == 0 || _figure._rotation == 180) && _figure._y < 1) ||
                ((_figure._rotation == 90 || _figure._rotation == 270) && _figure._y < 2)) {
                _lose = true;
                return;
            }
            if (_figure._rotation == 0) {
                (*this)[_figure._y][_figure._x] = Color::Blue;
                (*this)[_figure._y - 1][_figure._x] = Color::Blue;
                (*this)[_figure._y][_figure._x + 1] = Color::Blue;
                (*this)[_figure._y][_figure._x + 2] = Color::Blue;
            } else if (_figure._rotation == 90) {
                (*this)[_figure._y][_figure._x] = Color::Blue;
                (*this)[_figure._y - 1][_figure._x] = Color::Blue;
                (*this)[_figure._y - 2][_figure._x] = Color::Blue;
                (*this)[_figure._y - 2][_figure._x + 1] = Color::Blue;
            } else if (_figure._rotation == 180) {
                (*this)[_figure._y][_figure._x + 2] = Color::Blue;
                (*this)[_figure._y - 1][_figure._x] = Color::Blue;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Blue;
                (*this)[_figure._y - 1][_figure._x + 2] = Color::Blue;
            } else if (_figure._rotation == 270) {
                (*this)[_figure._y][_figure._x] = Color::Blue;
                (*this)[_figure._y][_figure._x + 1] = Color::Blue;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Blue;
                (*this)[_figure._y - 2][_figure._x + 1] = Color::Blue;
            }
        } else if (_figure._type == FigureType::L) {
            if (((_figure._rotation == 0 || _figure._rotation == 180) && _figure._y < 1) ||
                ((_figure._rotation == 90 || _figure._rotation == 270) && _figure._y < 2)) {
                _lose = true;
                return;
            }
            if (_figure._rotation == 0) {
                (*this)[_figure._y][_figure._x] = Color::Orange;
                (*this)[_figure._y - 1][_figure._x + 2] = Color::Orange;
                (*this)[_figure._y][_figure._x + 1] = Color::Orange;
                (*this)[_figure._y][_figure._x + 2] = Color::Orange;
            } else if (_figure._rotation == 90) {
                (*this)[_figure._y][_figure._x] = Color::Orange;
                (*this)[_figure._y - 1][_figure._x] = Color::Orange;
                (*this)[_figure._y - 2][_figure._x] = Color::Orange;
                (*this)[_figure._y][_figure._x + 1] = Color::Orange;
            } else if (_figure._rotation == 180) {
                (*this)[_figure._y][_figure._x] = Color::Orange;
                (*this)[_figure._y - 1][_figure._x] = Color::Orange;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Orange;
                (*this)[_figure._y - 1][_figure._x + 2] = Color::Orange;
            } else if (_figure._rotation == 270) {
                (*this)[_figure._y - 2][_figure._x] = Color::Orange;
                (*this)[_figure._y][_figure._x + 1] = Color::Orange;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Orange;
                (*this)[_figure._y - 2][_figure._x + 1] = Color::Orange;
            }
        } else if (_figure._type == FigureType::T) {
            if (((_figure._rotation == 0 || _figure._rotation == 180) && _figure._y < 1) ||
                ((_figure._rotation == 90 || _figure._rotation == 270) && _figure._y < 2)) {
                _lose = true;
                return;
            }
            if (_figure._rotation == 0) {
                (*this)[_figure._y][_figure._x] = Color::Magenta;
                (*this)[_figure._y][_figure._x + 1] = Color::Magenta;
                (*this)[_figure._y][_figure._x + 2] = Color::Magenta;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Magenta;
            } else if (_figure._rotation == 90) {
                (*this)[_figure._y][_figure._x] = Color::Magenta;
                (*this)[_figure._y - 1][_figure._x] = Color::Magenta;
                (*this)[_figure._y - 2][_figure._x] = Color::Magenta;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Magenta;
            } else if (_figure._rotation == 180) {
                (*this)[_figure._y][_figure._x + 1] = Color::Magenta;
                (*this)[_figure._y - 1][_figure._x] = Color::Magenta;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Magenta;
                (*this)[_figure._y - 1][_figure._x + 2] = Color::Magenta;
            } else if (_figure._rotation == 270) {
                (*this)[_figure._y - 1][_figure._x] = Color::Magenta;
                (*this)[_figure._y][_figure._x + 1] = Color::Magenta;
                (*this)[_figure._y - 1][_figure._x + 1] = Color::Magenta;
                (*this)[_figure._y - 2][_figure._x + 1] = Color::Magenta;
            }
        }

        _score += (20 - _figure._y) * _lvl;
        clearField(_figure._y, 1);

        if (_lvl < 11) {
            if (_score >= 1000 && _lvl == 1) _lvl = 2;
            if (_score >= 2000 && _lvl == 2) _lvl = 3;
            if (_score >= 4000 && _lvl == 3) _lvl = 4;
            if (_score >= 8000 && _lvl == 4) _lvl = 5;
            if (_score >= 16000 && _lvl == 5) _lvl = 6;
            if (_score >= 32000 && _lvl == 6) _lvl = 7;
            if (_score >= 64000 && _lvl == 7) _lvl = 8;
            if (_score >= 128000 && _lvl == 8) _lvl = 9;
            if (_score >= 256000 && _lvl == 9) _lvl = 10;
            if (_score >= 512000 && _lvl == 10) _lvl = 11;
        }

        _figure = figure(_figureNext);
        _figureNext = (FigureType)(rand() % 7);
        if (_figure._type == _figureNext) _figureNext = (FigureType)(rand() % 7);
        _swap = true;
    }
    void game::clearField(int y, int multi) {
        if (y < 0) return;

        int cols = 0;
        for (int i = 0; i < 10; i++) {
            if (_field[(y * 10) + i] != Color::None) cols++;
        }

        if (cols == 0) return;
        if (cols == 10) {
            _score += 100 * (multi * _lvl);
            fieldDown(y);
            clearField(y, multi + 1);

            return;
        }
        clearField(y - 1, 1);
    }
    void game::fieldDown(int y) {
        for (;; y--) {
            int cols = 0;
            if (y == 0) {
                for (int i = 0; i < 10; i++) _field[(y * 10) + i] = Color::None;
                return;
            }
            for (int i = 0; i < 10; i++) {
                _field[(y * 10) + i] = _field[((y - 1) * 10) + i];
                if (_field[(y * 10) + i] == Color::None) cols++;
            }
            if (cols == 10) return;
        }
    }

    /// Операторы
    Color *game::operator[] (int index) {
        return (_field + index * 10);
    }
    const Color *game::operator[] (int index) const {
        return (_field + index * 10);
    }
    game game::operator = (const game &A) {
        _score = A._score;
        _lvl = A._lvl;
        _swap = A._swap;
        _lose = A._lose;
        _count = A._count;
        _show = A._show;
        _selected = A._selected;

        _figure = A._figure;
        _figureNext = A._figureNext;

        if (!_field) _field = new Color [200];

        for (int i = 0; i < 200; i++)
            _field[i] = A._field[i];

        return (*this);
    }

    /// Деструктор
    game::~game() {
        if (_field) delete []_field;
    }
}
