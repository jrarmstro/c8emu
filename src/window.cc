#include "window.h"
#include <QFileDialog>
#include <QKeyEvent>
#include <QTimer>
#include "chip8.h"
#include "ui_window.h"


Window::Window(QWidget* parent, const char* app)
        : QMainWindow{parent},
          ui{new Ui::Window},
          c8{nullptr},
          c8loop{nullptr},
          c8gfx{64, 32, QImage::Format_Mono}
{
    ui->setupUi(this);

    c8gfx.setColorCount(2);
    c8gfx.setColor(0, qRgba(0, 0, 0, 255));
    c8gfx.setColor(1, qRgba(255, 255, 255, 255));

    // Default key bindings:
    keymap.emplace(Qt::Key_1, 0x1);
    keymap.emplace(Qt::Key_2, 0x2);
    keymap.emplace(Qt::Key_3, 0x3);
    keymap.emplace(Qt::Key_4, 0xC);

    keymap.emplace(Qt::Key_Q, 0x4);
    keymap.emplace(Qt::Key_W, 0x5);
    keymap.emplace(Qt::Key_E, 0x6);
    keymap.emplace(Qt::Key_R, 0xD);

    keymap.emplace(Qt::Key_A, 0x7);
    keymap.emplace(Qt::Key_S, 0x8);
    keymap.emplace(Qt::Key_D, 0x9);
    keymap.emplace(Qt::Key_F, 0xE);

    keymap.emplace(Qt::Key_Z, 0xA);
    keymap.emplace(Qt::Key_X, 0x0);
    keymap.emplace(Qt::Key_C, 0xB);
    keymap.emplace(Qt::Key_V, 0xF);

    connect(ui->actionOpen_ROM, &QAction::triggered, this, &Window::open);

    if (app) load_rom(app);
}

Window::~Window() {
    delete ui;
}

void Window::load_rom(const char* filename) {
    delete c8;
    c8 = new Chip8{};
    if (!c8->load_application(filename)) exit(1);

    if (c8loop) disconnect(c8loop, &QTimer::timeout, nullptr, nullptr);
    delete c8loop;
    c8loop = new QTimer(parent());
    connect(c8loop, &QTimer::timeout, this, [this]{
        c8->step();
        if (c8->need_redraw()) {
            drawChip8(c8);
        }
    });
    c8loop->start(1);
}

void Window::drawChip8(Chip8* c8) {
    if (!c8) return;

    for (auto row = 0; row < 32; ++row) {
        for (auto col = 0; col < 64; ++col) {
            c8gfx.setPixel(col, row, c8->gfx_[row * 64 + col]);
        }
    }

    auto image = QPixmap::fromImage(c8gfx);
    image = image.scaled(this->width(), this->height(), Qt::AspectRatioMode::KeepAspectRatio);

    auto scene = new QGraphicsScene(this);
    scene->addPixmap(image);

    auto oldscene = ui->mainImage->scene();
    ui->mainImage->setScene(scene);
    delete oldscene;

    c8->need_redraw_ = false;
}

void Window::open() {
    QString filename = QFileDialog::getOpenFileName(this, "Select CHIP-8 ROM file");
    load_rom(QFile::encodeName(filename));
}

void Window::resizeEvent(QResizeEvent*) {
    drawChip8(c8);
}

void Window::keyPressEvent(QKeyEvent* event) {
    try {
        auto key = keymap.at(event->key());
        c8->set_key(key, true);
    } catch (std::out_of_range&) {
        // Key was not mapped; do nothing.
    }
}

void Window::keyReleaseEvent(QKeyEvent* event) {
    try {
        auto key = keymap.at(event->key());
        c8->set_key(key, false);
    } catch (std::out_of_range&) {
        // Key was not mapped; do nothing.
    }
}
