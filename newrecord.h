#include <QWidget>
#include <QMessageBox>
#include <QPainter>
#include <fstream>

namespace Ui {
    class NewRecord;
}

namespace tetris {
    class NewRecord : public QWidget {
        Q_OBJECT

    public:
        explicit NewRecord(QWidget *parent = 0);
        void getScore(unsigned long long score);
        ~NewRecord();

    private slots:
        void on_pushButton_pressed();

    private:
        Ui::NewRecord *ui;

        unsigned long long _score;
    };
}
