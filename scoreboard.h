#include <QWidget>
#include <QMessageBox>
#include <QPainter>
#include <QRect>
#include <QEvent>
#include <QKeyEvent>
#include <fstream>

namespace Ui {
    class scoreboard;
}

namespace tetris {
    class scoreboard : public QWidget {
        Q_OBJECT

    public:
        explicit scoreboard(QWidget *parent = 0);

        void loadScore();
        void addUser(QString name, unsigned long long score);

        void paintEvent(QPaintEvent *);
        bool event(QEvent *event);

        ~scoreboard();

    private slots:
        void on_bScoreClear_pressed();

    private:
        Ui::scoreboard *ui;

        QList <QString> *_names;
        QList <unsigned long long> *_score;
    };
}
