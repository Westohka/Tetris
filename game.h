#include "figure.h"

namespace tetris {

    enum class Color : int {
        None = 0, Yellow = 1, Cyan = 2, Blue = 3, Orange = 4, Green = 5, Red = 6, Magenta = 7, Black = 8
    };

    class game {
    public:
        int _lvl, _count;
        unsigned long long _score;
        bool _swap, _lose, _show, _selected;

        figure _figure;
        FigureType _figureNext;

        Color *_field;

        game();
        game(const game &A);

        void moveLeft();
        void moveRight();
        void moveDown();
        void rotate();

        bool checkFree(int x, int y, int rotation) const;
        void clearField(int y, int multi);
        void fieldDown(int y);
        void figureStop();

        Color *operator[] (int index);
        const Color *operator[] (int index) const;
        game operator = (const game &A);

        ~game();
    };
}
