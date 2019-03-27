#include "scoreboard.h"
#include "ui_scoreboard.h"

using namespace std;

namespace tetris {
    scoreboard::scoreboard(QWidget *parent) : QWidget(parent) {
        ui = new Ui::scoreboard;
        ui->setupUi(this);

        _names = new QList<QString>;
        _score = new QList<unsigned long long int>;

        QBrush *brush = new QBrush;
        QPalette *palette = new QPalette;

        brush->setTextureImage(QImage(":/images/image_1"));
        palette->setBrush(QPalette::Window, *brush);
        this->setPalette(*palette);
    }

    /// Загрузка таблицы из файла
    void scoreboard::loadScore() {
        char *n = new char [100];
        QString name;
        unsigned long long score;
        int count = 0;

        _names->clear();
        _score->clear();

        fstream file("scoreboard.txt", ios_base::in);

        while (file >> n >> score) {
            name = n;
            count++;

            if (name.length() > 20) name.resize(20);
            addUser(name, score);
        }

        file.close();

        if (count > 10) {
            file.open("scoreboard.txt", ios_base::out);
            for (int i = 0; i < 10; i++) file << _names->at(i).toUtf8().data() << " " << (int)_score->at(i) << "\n";
            file.close();
        }
    }

    /// Добавление новой записи
    void scoreboard::addUser(QString name, unsigned long long score) {
        if (_score->size() == 10 && score <= _score->at(9)) return;

        if (_names->isEmpty()) {
            _names->push_front(name);
            _score->push_front(score);
            return;
        }

        for (int i = _score->size() - 1; i >= 0; i--) {
            if (i == 9) {
                _names->replace(9, name);
                _score->replace(9, score);
            } else {
                if (score > _score->at(i)) {
                    if (i == _score->size() - 1) {
                        _names->insert(i, name);
                        _score->insert(i, score);
                    } else {
                        _names->swap(i, i + 1);
                        _score->swap(i, i + 1);
                    }
                } else if (i == _score->size() - 1) {
                    _names->push_back(name);
                    _score->push_back(score);
                    return;
                } else return;
            }
        }
    }

    /// Отрисовка самой таблицы
    void scoreboard::paintEvent(QPaintEvent *) {
        QPainter painter(this);
        painter.setPen(QPen (Qt::white));
        painter.setFont(QFont ("Times", 16, QFont::Bold));

        painter.drawText(QPoint(130, 50), QString("Таблица лидеров:"));

        painter.setFont(QFont ("Times", 12, QFont::Normal));
        for (int i = 1; i < 11; i++) {
            painter.drawText(QPoint(40, 100 + ((i - 1) * 26)), QString::number(i));

            if (_names->size() < i) {
                painter.drawText(QPoint(100, 100 + ((i - 1) * 26)), "- - -");
                painter.drawText(QPoint(300, 100 + ((i - 1) * 26)), "- - -");
            } else {
                painter.drawText(QPoint(100, 100 + ((i - 1) * 26)), _names->at(i - 1));
                painter.drawText(QRect(240, 90 + ((i - 1) * 26), 160, 16), Qt::AlignCenter, QString::number(_score->at(i - 1)));

                painter.setPen(QPen (Qt::gray));
                painter.drawLine(QPointF(40, 110 + ((i - 1) * 26)), QPointF(380, 110 + ((i - 1) * 26)));
                painter.setPen(QPen (Qt::white));
            }
        }
    }

    bool scoreboard::event(QEvent *event) {
        /// Обработка нажатия клавиш клавиатуры
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *key = (QKeyEvent*)event;
            if (key->key() == Qt::Key_Escape) {
                this->setDisabled(true);
                this->hide();

                return true;
            }
        }

        return QWidget::event(event);
    }

    void scoreboard::on_bScoreClear_pressed() {
        QMessageBox::StandardButton bScoreClear = QMessageBox::question(this, "Очистка счета", "Вы действительно хотите очистить весь счет?", QMessageBox::Yes | QMessageBox::No);
        if (bScoreClear == QMessageBox::Yes) {
            _names->clear();
            _score->clear();

            ofstream file("scoreboard.txt");
            file.close();

            repaint();
        }
    }

    scoreboard::~scoreboard() {
        delete ui;
    }
}
