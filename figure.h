#include <ostream>

namespace tetris {

    enum class FigureType : int {
        I = 0, L = 1, J = 2, O = 3, T = 4, Z = 5, S = 6
    };

    class figure {
    public:
        FigureType _type;
        int _rotation, _x, _y;

        figure();
        figure(FigureType type);
        figure(const figure &A);

        int width(int rotation) const;

        figure operator = (const figure &A);

        ~figure();
    };
}
