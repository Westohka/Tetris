#include "mainwindow.h"
#include "ui_mainwindow.h"

/// Макросы для остановки и удаления таймеров
#define timerClear(timer) {\
    if (timer) {\
        timer->stop();\
        delete timer;\
        timer = 0;\
    }\
}
/// Макросы для старта таймеров
#define timerStart(timer, time, slot) {\
    timerClear(timer);\
    timer = new QTimer();\
    timer->setInterval(time);\
    connect(timer, SIGNAL(timeout()), this, SLOT(slot()));\
    timer->start();\
}

namespace tetris {
    MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
        ui = new Ui::MainWindow;
        ui->setupUi(this);

        QBrush *brush = new QBrush;
        QPalette *palette = new QPalette;

        brush->setTextureImage(QImage(":/images/image_1"));
        palette->setBrush(QPalette::Window, *brush);
        this->setPalette(*palette);

        _gameStatus = false;
        _posX = 0;
        _opacity = 1;
        _opacity0 = 0;
        _menuSelect = 0;
        _time = 0;

        _pause = false;
        _color = true;

        _timer = 0;
        _anim1 = 0;
        _anim = 0;
        timerStart(_anim, 1, anim_0);
    }

    /// Обработка событий
    bool MainWindow::event(QEvent *event) {
        /// Проигрыш
        if (_game._lose && !_anim && _gameStatus) {
            if (_game._count == 0) timerStart(_anim, 1, lose);
            if (_game._show && _record.isHidden() && _game._count == 200) {
                _game._show = false;
                _scoreboard.show();
                _scoreboard.loadScore();
            }
        }

        /// Деактивация окна
        if (event->type() == QEvent::WindowDeactivate && !_game._lose) {
            if (!(*this).isActiveWindow() && _gameStatus && !_pause) {
                _pause = true;

                if (_timer) {
                    _time = _timer->remainingTime();
                    if (_time <= 50) _time = 51;
                    timerClear(_timer);
                }
                timerStart(_anim1, 100, anim_3);
                repaint();
            }
        }
        if (_scoreboard.isHidden() && _menuSelect == 1 && !_anim) {
            _posX = 0;
            _opacity = 1;
        }

        /// Обработка нажатия клавиш клавиатуры
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *key = (QKeyEvent*)event;

            /// ESC
            if (key->key() == Qt::Key_Escape) {
                if (!_gameStatus) {
                    if (!_anim) {
                        QMessageBox::StandardButton bQuit = QMessageBox::question(this, "Выход", "Вы действительно хотите выйти?", QMessageBox::Yes | QMessageBox::No);
                        if (bQuit == QMessageBox::Yes) QApplication::quit();
                    }
                } else {
                    if (!_pause && !_game._lose) {
                        _pause = true;

                        if (_timer) {
                            _time = _timer->remainingTime();
                            if (_time <= 50) _time = 51;
                            timerClear(_timer);
                        }
                        timerStart(_anim1, 100, anim_3);
                        repaint();
                    }
                    QMessageBox::StandardButton bQuit;

                    if (!_game._lose || _anim) bQuit = QMessageBox::question(this, "Выход", "Вы действительно хотите вернуться в главное меню?\n\nСчет будет потерян!", QMessageBox::Yes | QMessageBox::No);
                        else bQuit = QMessageBox::question(this, "Выход", "Вы действительно хотите вернуться в главное меню?", QMessageBox::Yes | QMessageBox::No);
                    if (bQuit == QMessageBox::Yes) {
                        ui->wMainMenu->show();

                        _gameStatus = false;
                        _posX = 0;
                        _opacity = 1;
                        _opacity0 = 0;

                        _pause = false;

                        timerClear(_anim1);
                        timerClear(_anim);
                        timerClear(_timer);
                        timerStart(_anim, 1, anim_0);
                    }
                }

                return true;
            }

            /// Управление в главном меню
            if (!_gameStatus && !_anim) {
                if (key->key() == Qt::Key_Down) {
                    if (_menuSelect == 2) _menuSelect = 0;
                        else _menuSelect++;
                    repaint();

                    return true;
                }

                if (key->key() == Qt::Key_Up) {
                    if (_menuSelect == 0) _menuSelect = 2;
                        else _menuSelect--;
                    repaint();

                    return true;
                }

                if (key->key() == Qt::Key_Space || key->key() == Qt::Key_Enter || key->key() == Qt::Key_Return) {
                    timerStart(_anim, 1, anim_2);
                    return true;
                }
            }

            /// Управление в игре
            if (_gameStatus && !_game._lose) {
                /// Пауза
                if (key->key() == Qt::Key_P) {
                    if (_pause) {
                        _pause = false;
                        timerClear(_anim1);
                        if (_time > 0) timerStart(_timer, _time, figureDown);
                    } else {
                        _pause = true;
                        if (_timer) {
                            _time = _timer->remainingTime();
                            if (_time <= 50) _time = 51;
                            timerClear(_timer);
                        }
                        timerStart(_anim1, 100, anim_3);
                    }
                    repaint();
                }
                if (_timer) {
                    /// Изменение уровня игры
                    if (key->key() == Qt::Key_E && _game._lvl < 11) {
                        _game._lvl++;
                        repaint();
                        return true;
                    }
                    if (key->key() == Qt::Key_Q && _game._lvl > 1) {
                        _game._lvl--;
                        repaint();
                        return true;
                    }

                    /// Перемещение фигуры
                    if (key->key() == Qt::Key_A || key->key() == Qt::Key_Left) {
                        _game.moveLeft();
                        repaint();
                        return true;
                    }
                    if (key->key() == Qt::Key_D || key->key() == Qt::Key_Right) {
                        _game.moveRight();
                        repaint();
                        return true;
                    }
                    if (key->key() == Qt::Key_Shift) {
                        _game.rotate();
                        repaint();
                        return true;
                    }

                    /// Смена фигуры
                    if (key->key() == Qt::Key_Space && _game._swap) {
                        _game._swap = false;
                        _game._figure = figure(_game._figureNext);
                        _game._figureNext = (FigureType)(rand() % 7);
                        if (_game._figure._type == _game._figureNext) _game._figureNext = (FigureType)(rand() % 7);
                        repaint();
                        return true;
                    }

                    /// Ускорение фигуры
                    if (_timer->interval() > 50 && (key->key() == Qt::Key_S || key->key() == Qt::Key_Down)) {
                        _timer->setInterval(50);
                        figureDown();
                        return true;
                    }
                }
            }
            if (_gameStatus && _game._lose && !_anim) {
                if (key->key() == Qt::Key_Down || key->key() == Qt::Key_Up) {
                    if (_game._selected) _game._selected = false;
                        else _game._selected = true;
                    repaint();

                    return true;
                }

                if (key->key() == Qt::Key_Space || key->key() == Qt::Key_Enter || key->key() == Qt::Key_Return) {
                    _gameStatus = false;
                    _posX = 0;
                    _opacity = 1;
                    _opacity0 = 0;
                    _pause = false;

                    if (_game._selected) startGame();
                    else {
                        ui->wMainMenu->show();
                        timerStart(_anim, 1, anim_0);
                    }
                    return true;
                }
            }
        }

        /// Отжатие клавиш
        if (event->type() == QEvent::KeyRelease && !_game._lose) {
            QKeyEvent *key = (QKeyEvent*)event;

            /// В игре
            if (_gameStatus) {
                /// Замеделение фигуры
                if (_timer && (key->key() == Qt::Key_S || key->key() == Qt::Key_Down)) {
                    _timer->setInterval((_game._lvl < 11) ? (1100 - (100 * _game._lvl)) : 60);
                    figureDown();
                    return true;
                }
            }
        }

        /// Обработка мыши
        if (!_gameStatus && !_anim) {
            QMouseEvent *mouse = (QMouseEvent*)event;

            if (pointInRectangle(mouse->x(), mouse->y(), 0, 320, 200, 349)) {
                _menuSelect = 0;
                repaint();
            }
            if (pointInRectangle(mouse->x(), mouse->y(), 0, 350, 200, 379)) {
                _menuSelect = 1;
                repaint();
            }
            if (pointInRectangle(mouse->x(), mouse->y(), 0, 380, 200, 410)) {
                _menuSelect = 2;
                repaint();
            }

            if (event->type() == QEvent::MouseButtonPress && mouse->button() == Qt::LeftButton && (pointInRectangle(mouse->x(), mouse->y(), 0, 320, 200, 349) || pointInRectangle(mouse->x(), mouse->y(), 0, 350, 200, 379) || pointInRectangle(mouse->x(), mouse->y(), 0, 380, 200, 410))) {
                timerStart(_anim, 1, anim_2);
                return true;
            }
        }

        return QWidget::event(event);
    }

    /// Графический интерфейс
    void MainWindow::paintEvent(QPaintEvent *) {
        QPainter painter(this);

        /// Главное меню
        if (!_gameStatus) {
            painter.setOpacity(_opacity0);

            painter.setPen(QPen (Qt::white));
            painter.setFont(QFont ("Times", 20, QFont::Normal));

            if (_menuSelect == 0) {
                painter.setOpacity(_opacity * _opacity0);
                painter.drawText(QPoint(30 + _posX, 330), QString("> Начать игру"));
                painter.setOpacity(_opacity0);
            } else painter.drawText(QPoint(30, 330), QString("Начать игру"));
            if (_menuSelect == 1) {
                painter.setOpacity(_opacity * _opacity0);
                painter.drawText(QPoint(30 + _posX, 360), QString("> Таблица лидеров"));
                painter.setOpacity(_opacity0);
            } else painter.drawText(QPoint(30, 360), QString("Таблица лидеров"));
            if (_menuSelect == 2) {
                painter.setOpacity(_opacity * _opacity0);
                painter.drawText(QPoint(30 + _posX, 390), QString("> Выйти"));
                painter.setOpacity(_opacity0);
            } else painter.drawText(QPoint(30, 390), QString("Выйти"));

            painter.drawText(QRect(0, 300, 700, 500), Qt::AlignCenter, QString("- Автор Westi -"));
        }

        /// Игра
        if (_gameStatus) {
            if (!_game._lose) painter.setOpacity(_opacity);
                else painter.setOpacity(1);

            painter.setBrush(QBrush (Qt::darkGray));
            painter.drawRect(0, 0, 700, 600);
            painter.setBrush(QBrush ());

            painter.setOpacity(_opacity);

            painter.setPen(QPen (Qt::white));
            painter.setFont(QFont ("Times", 20, QFont::Normal));
            painter.drawText(QPoint(400 + _posX, 110), QString("Счет:   ") + QString::number(_game._score));
            painter.drawText(QPoint(400 + _posX, 180), QString("Следующая фигура:"));
            painter.drawText(QPoint(460 + _posX, 400), QString("УРОВЕНЬ - ") + ((_game._lvl < 11) ? QString::number(_game._lvl) : QString("АД")));

            painter.setFont(QFont ("Times", 16, QFont::Normal));
            if (_game._swap) painter.drawText(QPoint(400 + _posX, 310), QString("Доступна смена фигуры - SPACE"));
            painter.drawText(QPoint(400 + _posX, 460), QString("Перемещение фигуры - A,D,S и стрелки"));
            painter.drawText(QPoint(400 + _posX, 480), QString("Поворот фигуры - SHIFT"));
            painter.drawText(QPoint(400 + _posX, 500), QString("Пауза - P"));
            painter.setPen(QPen (Qt::black));

            /// Следующая фигура
            if (_game._figureNext == FigureType::I) {
                painter.setBrush(QBrush (Qt::darkCyan));
                painter.drawRect(462 + _posX, 220, 22, 22);
                painter.drawRect(484 + _posX, 220, 22, 22);
                painter.drawRect(506 + _posX, 220, 22, 22);
                painter.drawRect(528 + _posX, 220, 22, 22);
            }
            if (_game._figureNext == FigureType::O) {
                painter.setBrush(QBrush (Qt::yellow));
                painter.drawRect(484 + _posX, 220, 22, 22);
                painter.drawRect(506 + _posX, 220, 22, 22);
                painter.drawRect(484 + _posX, 242, 22, 22);
                painter.drawRect(506 + _posX, 242, 22, 22);
            }
            if (_game._figureNext == FigureType::J) {
                painter.setBrush(QBrush (Qt::blue));
                painter.drawRect(473 + _posX, 242, 22, 22);
                painter.drawRect(495 + _posX, 242, 22, 22);
                painter.drawRect(517 + _posX, 242, 22, 22);
                painter.drawRect(473 + _posX, 220, 22, 22);
            }
            if (_game._figureNext == FigureType::L) {
                painter.setBrush(QBrush (QColor::fromRgb(240, 171, 0)));
                painter.drawRect(473 + _posX, 242, 22, 22);
                painter.drawRect(495 + _posX, 242, 22, 22);
                painter.drawRect(517 + _posX, 242, 22, 22);
                painter.drawRect(517 + _posX, 220, 22, 22);
            }
            if (_game._figureNext == FigureType::S) {
                painter.setBrush(QBrush (Qt::darkGreen));
                painter.drawRect(473 + _posX, 242, 22, 22);
                painter.drawRect(495 + _posX, 220, 22, 22);
                painter.drawRect(495 + _posX, 242, 22, 22);
                painter.drawRect(517 + _posX, 220, 22, 22);
            }
            if (_game._figureNext == FigureType::Z) {
                painter.setBrush(QBrush (Qt::darkRed));
                painter.drawRect(473 + _posX, 220, 22, 22);
                painter.drawRect(495 + _posX, 220, 22, 22);
                painter.drawRect(495 + _posX, 242, 22, 22);
                painter.drawRect(517 + _posX, 242, 22, 22);
            }
            if (_game._figureNext == FigureType::T) {
                painter.setBrush(QBrush (Qt::darkMagenta));
                painter.drawRect(473 + _posX, 242, 22, 22);
                painter.drawRect(495 + _posX, 220, 22, 22);
                painter.drawRect(495 + _posX, 242, 22, 22);
                painter.drawRect(517 + _posX, 242, 22, 22);
            }

            if (_game._lose) painter.setOpacity(1);

            /// Отрисовка текущей фигуры
            if (_game._figure._type == FigureType::O) {
                if (_game._figure._y > -1) {
                    painter.setBrush(QBrush (Qt::yellow));
                    painter.drawRect(80 + (_game._figure._x * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                    painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                }
                if (_game._figure._y > 0) {
                    painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                    painter.drawRect(80 + (_game._figure._x * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                }
            }

            if (_game._figure._type == FigureType::I) {
                if (_game._figure._y >= 0) {
                    painter.setBrush(QBrush (Qt::darkCyan));

                    painter.drawRect(80 + (_game._figure._x * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                    if (_game._figure._rotation == 0) {
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 3) * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                    }
                }
                if (_game._figure._rotation == 90) {
                    if (_game._figure._y > 0) painter.drawRect(80 + (_game._figure._x * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                    if (_game._figure._y > 1) painter.drawRect(80 + (_game._figure._x * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                    if (_game._figure._y > 2) painter.drawRect(80 + (_game._figure._x * 22) - _posX, 90 + ((_game._figure._y - 3) * 22), 22, 22);
                }
            }

            if (_game._figure._type == FigureType::Z) {
                if (_game._figure._y >= 0) {
                    painter.setBrush(QBrush (Qt::darkRed));
                    if (_game._figure._rotation == 0) {
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) {
                            painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        }
                    }
                    if (_game._figure._rotation == 90) {
                        painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) {
                            painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        }
                        if (_game._figure._y > 1) painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                    }
                }
            }
            if (_game._figure._type == FigureType::S) {
                if (_game._figure._y >= 0) {
                    painter.setBrush(QBrush (Qt::darkGreen));
                    if (_game._figure._rotation == 0) {
                        painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) {
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        }
                    }
                    if (_game._figure._rotation == 90) {
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) {
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        }
                        if (_game._figure._y > 1) painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                    }
                }
            }

            if (_game._figure._type == FigureType::L) {
                if (_game._figure._y >= 0) {
                    painter.setBrush(QBrush (QColor::fromRgb(240, 171, 0)));
                    if (_game._figure._rotation == 0) {
                        painter.drawRect(80 + (_game._figure._x * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                        if (_game._figure._y > 0) painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                    }
                    if (_game._figure._rotation == 90) {
                        painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        if (_game._figure._y > 1) painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                    }
                    if (_game._figure._rotation == 180) {
                        painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) {
                            painter.drawRect(80 + (_game._figure._x * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        }
                    }
                    if (_game._figure._rotation == 270) {
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        if (_game._figure._y > 1) {
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                        }
                    }
                }
            }

            if (_game._figure._type == FigureType::J) {
                if (_game._figure._y >= 0) {
                    painter.setBrush(QBrush (Qt::blue));
                    if (_game._figure._rotation == 0) {
                        painter.drawRect(80 + (_game._figure._x * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + (_game._figure._y * 22), 22, 22);
                        if (_game._figure._y > 0) painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                    }
                    if (_game._figure._rotation == 90) {
                        painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        if (_game._figure._y > 1) {
                            painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                        }
                    }
                    if (_game._figure._rotation == 180) {
                        painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) {
                            painter.drawRect(80 + (_game._figure._x * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        }
                    }
                    if (_game._figure._rotation == 270) {
                        painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        if (_game._figure._y > 1) painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                    }
                }
            }

            if (_game._figure._type == FigureType::T) {
                if (_game._figure._y >= 0) {
                    painter.setBrush(QBrush (Qt::darkMagenta));
                    if (_game._figure._rotation == 0) {
                        painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                    }
                    if (_game._figure._rotation == 90) {
                        painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) {
                            painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        }
                        if (_game._figure._y > 1) painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                    }
                    if (_game._figure._rotation == 180) {
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) {
                            painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 2) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        }
                    }
                    if (_game._figure._rotation == 270) {
                        painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y) * 22), 22, 22);
                        if (_game._figure._y > 0) {
                            painter.drawRect(80 + ((_game._figure._x) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                            painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 1) * 22), 22, 22);
                        }
                        if (_game._figure._y > 1) painter.drawRect(80 + ((_game._figure._x + 1) * 22) - _posX, 90 + ((_game._figure._y - 2) * 22), 22, 22);
                    }
                }
            }

            /// Игровое поле
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 20; j++) {
                    if (_game[j][i] == Color::None) painter.setBrush(QBrush ());
                    else if (_game[j][i] == Color::Blue) painter.setBrush(QBrush (Qt::blue));
                    else if (_game[j][i] == Color::Cyan) painter.setBrush(QBrush (Qt::darkCyan));
                    else if (_game[j][i] == Color::Green) painter.setBrush(QBrush (Qt::darkGreen));
                    else if (_game[j][i] == Color::Magenta) painter.setBrush(QBrush (Qt::darkMagenta));
                    else if (_game[j][i] == Color::Red) painter.setBrush(QBrush (Qt::darkRed));
                    else if (_game[j][i] == Color::Yellow) painter.setBrush(QBrush (Qt::yellow));
                    else if (_game[j][i] == Color::Orange) painter.setBrush(QBrush (QColor::fromRgb(240, 171, 0)));
                    else painter.setBrush(QBrush (QColor::fromRgb(100, 100, 100)));

                    painter.drawRect(80 + (i * 22) - _posX, 90 + (j * 22), 22, 22);
                }
            }

            if (_game._lose) {
                painter.setPen(QPen (Qt::white));
                painter.setFont(QFont ("Times", 20, QFont::Normal));
                painter.setOpacity(((double)_game._count / 100));
                painter.drawText(QRect(0, 20, 700, 20), Qt::AlignCenter, "Вы проиграли!");
                painter.drawText(QRect(0, 50, 700, 20), Qt::AlignCenter, "Ваш счет: " + QString::number(_game._score));

                if (_game._selected && !_anim) painter.drawText(QPoint(30, 330), QString("> Начать заново"));
                    else painter.drawText(QPoint(30, 330), QString("Начать заново"));
                if (!_game._selected && !_anim) painter.drawText(QPoint(30, 360), QString("> Выйти"));
                    else painter.drawText(QPoint(30, 360), QString("Выйти"));
            }

            painter.setOpacity(1);

            /// Пауза
            if (_pause) {
                if (_color) painter.setPen(QPen (Qt::white));
                    else painter.setPen(QPen (Qt::black));

                painter.setFont(QFont ("Times", 40, QFont::Normal));
                painter.drawText(QRect(0, 0, 700, 600), Qt::AlignCenter, QString("ПАУЗА"));
            }
        }
    }

    /// Скорость игры
    void MainWindow::figureDown() {
        if (!_pause && !_game._lose) {
            _game.moveDown();
            repaint();

            if (_timer->interval() > 50) _timer->setInterval((_game._lvl < 11) ? (1100 - (100 * _game._lvl)) : 60);
        }
        if (_game._lose) timerClear(_timer);
    }
    /// Проигрыш
    void MainWindow::lose() {
        if (_game._count < 200) {
            if (_posX > -140) _posX -= 5;
            _opacity -= 0.05;

            _game._field[199 - _game._count] = Color::Black;
            _game._count++;
            repaint();
        } else {
            timerClear(_anim);
            _record.show();
            _record.getScore(_game._score);
        }
    }

    /// Анимации
    void MainWindow::anim_0() {
        _opacity0 += 0.01;
        if (_opacity0 >= 1) {
            timerClear(_anim);
            _opacity0 = 1;
        }
        repaint();
    }
    void MainWindow::anim_1() {
        if (!_pause) {
            _posX -= 5;
            _opacity += 0.05;

            if (_posX <= 0) {
                timerClear(_anim);
                timerStart(_timer, 1000, figureDown);
                _opacity = 1;
            }
            repaint();
        }
    }
    void MainWindow::anim_2() {
        _posX += 2;
        _opacity -= 0.05;
        if (_opacity < 0) {
            timerClear(_anim);
            _opacity = 0;

            if (_menuSelect == 0) startGame();
            if (_menuSelect == 1) scoreboard();
            if (_menuSelect == 2) quit();
        }
        repaint();
    }
    void MainWindow::anim_3() {
        if (_color) _color = false;
            else _color = true;
        repaint();
    }

    /// Вспомогательные ф-ции
    bool MainWindow::pointInRectangle(double rx, double ry, double x1, double y1, double x2, double y2) {
        return (rx >= x1 && rx <= x2 && ry >= y1 && ry <= y2);
    }
    void MainWindow::startGame() {
        _gameStatus = true;
        _game = game();
        _posX = 350;

        ui->wMainMenu->hide();

        timerStart(_anim, 1, anim_1);
    }
    void MainWindow::scoreboard() {
        _scoreboard.loadScore();
        _scoreboard.show();
    }
    void MainWindow::quit() {
        QMessageBox::StandardButton bQuit = QMessageBox::question(this, "Выход", "Вы действительно хотите выйти?", QMessageBox::Yes | QMessageBox::No);
        if (bQuit == QMessageBox::Yes) QApplication::quit();
        else {
            _posX = 0;
            _opacity = 1;
        }
    }

    /// Деструктор
    MainWindow::~MainWindow() {
        delete ui;

        timerClear(_anim);
        timerClear(_anim1);
        timerClear(_timer);
    }
}
