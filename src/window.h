#ifndef WINDOW_H
#define WINDOW_H
#include <QGraphicsScene>
#include <QMainWindow>
#include "chip8.h"


namespace Ui {
class Window;
}

class Window : public QMainWindow {
    Q_OBJECT
public:

    Window(QWidget *parent, const char* app);

    ~Window();

protected:

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private:

    Ui::Window *ui;

    QGraphicsScene *scene;
    QPixmap image;

    Chip8 c8;

};

#endif
