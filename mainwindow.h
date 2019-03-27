#include <QMainWindow>
#include <QApplication>
#include <QTimer>

#include "newrecord.h"
#include "scoreboard.h"
#include "game.h"

namespace Ui {
    class MainWindow;
}

namespace tetris {
    class MainWindow : public QMainWindow {
        Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent = 0);

        bool event(QEvent *event);
        void paintEvent(QPaintEvent *);

        ~MainWindow();

    private slots:
        void anim_0();
        void anim_1();
        void anim_2();
        void anim_3();

        void lose();
        void figureDown();

    private:
        Ui::MainWindow *ui;

        scoreboard _scoreboard;
        NewRecord _record;
        game _game;

        bool _pause, _gameStatus, _color;
        int _posX, _menuSelect, _time;
        double _opacity, _opacity0;

        QTimer *_anim, *_anim1, *_timer;

        bool pointInRectangle(double rx, double ry, double x1, double y1, double x2, double y2);

        void startGame();
        void scoreboard();
        void quit();
    };
}
