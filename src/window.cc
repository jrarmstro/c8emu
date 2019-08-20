#include "window.h"
#include <QKeyEvent>
#include <QTimer>
#include "ui_window.h"

Window::Window(QWidget *parent, const char* app) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);

    c8.load_application(app);

    auto timer = new QTimer(parent);
    connect(timer, &QTimer::timeout, this, [this]{
        c8.step();
        if (c8.need_redraw_) {
            QImage img {64, 32, QImage::Format_Mono};
            img.setColorCount(2);
            img.setColor(0, qRgba(0, 0, 0, 255));
            img.setColor(1, qRgba(255, 255, 255, 255));
            for (auto row = 0; row < 32; ++row) {
                for (auto col = 0; col < 64; ++col) {
                    img.setPixel(col, row, c8.gfx_[row * 64 + col]);
                }
            }
            scene = new QGraphicsScene(this);
            image = QPixmap::fromImage(img);
            image = image.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
            scene->addPixmap(image);
            delete ui->mainImage->scene();
            ui->mainImage->setScene(scene);

            c8.need_redraw_ = false;
        }
    });
    timer->start(1);
}

Window::~Window()
{
    delete ui;
}

void Window::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_1:
            c8.set_key(0x1, true);
            break;
        case Qt::Key_2:
            c8.set_key(0x2, true);
            break;
        case Qt::Key_3:
            c8.set_key(0x3, true);
            break;
        case Qt::Key_4:
            c8.set_key(0xC, true);
            break;
        case Qt::Key_Q:
            c8.set_key(0x4, true);
            break;
        case Qt::Key_W:
            c8.set_key(0x5, true);
            break;
        case Qt::Key_E:
            c8.set_key(0x6, true);
            break;
        case Qt::Key_R:
            c8.set_key(0xD, true);
            break;
        case Qt::Key_A:
            c8.set_key(0x7, true);
            break;
        case Qt::Key_S:
            c8.set_key(0x8, true);
            break;
        case Qt::Key_D:
            c8.set_key(0x9, true);
            break;
        case Qt::Key_F:
            c8.set_key(0xE, true);
            break;
        case Qt::Key_Z:
            c8.set_key(0xA, true);
            break;
        case Qt::Key_X:
            c8.set_key(0x0, true);
            break;
        case Qt::Key_C:
            c8.set_key(0xB, true);
            break;
        case Qt::Key_V:
            c8.set_key(0xF, true);
            break;
    }
}

void Window::keyReleaseEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_1:
            c8.set_key(0x1, false);
            break;
        case Qt::Key_2:
            c8.set_key(0x2, false);
            break;
        case Qt::Key_3:
            c8.set_key(0x3, false);
            break;
        case Qt::Key_4:
            c8.set_key(0xC, false);
            break;
        case Qt::Key_Q:
            c8.set_key(0x4, false);
            break;
        case Qt::Key_W:
            c8.set_key(0x5, false);
            break;
        case Qt::Key_E:
            c8.set_key(0x6, false);
            break;
        case Qt::Key_R:
            c8.set_key(0xD, false);
            break;
        case Qt::Key_A:
            c8.set_key(0x7, false);
            break;
        case Qt::Key_S:
            c8.set_key(0x8, false);
            break;
        case Qt::Key_D:
            c8.set_key(0x9, false);
            break;
        case Qt::Key_F:
            c8.set_key(0xE, false);
            break;
        case Qt::Key_Z:
            c8.set_key(0xA, false);
            break;
        case Qt::Key_X:
            c8.set_key(0x0, false);
            break;
        case Qt::Key_C:
            c8.set_key(0xB, false);
            break;
        case Qt::Key_V:
            c8.set_key(0xF, false);
            break;
    }
}
