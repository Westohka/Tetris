#include "newrecord.h"
#include "ui_newrecord.h"

namespace tetris {
    NewRecord::NewRecord(QWidget *parent) : QWidget(parent) {
        ui = new Ui::NewRecord;
        ui->setupUi(this);

        QBrush *brush = new QBrush;
        QPalette *palette = new QPalette;

        brush->setTextureImage(QImage(":/images/image_1"));
        palette->setBrush(QPalette::Window, *brush);
        this->setPalette(*palette);

        _score = 0;
    }

    void NewRecord::getScore(unsigned long long score) {
        _score = score;
    }

    void NewRecord::on_pushButton_pressed() {
        if (ui->lineEdit->text().length() > 0) {
            std::ofstream file("scoreboard.txt", std::ios_base::app);
            file << ui->lineEdit->text().toUtf8().data() << " " << _score << "\n";
            file.close();

            this->hide();
        }
    }


    NewRecord::~NewRecord() {
        delete ui;
    }
}
