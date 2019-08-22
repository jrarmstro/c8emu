#ifndef WINDOW_H
#define WINDOW_H
#include <map>
#include <QGraphicsScene>
#include <QMainWindow>


namespace Ui {
    class Window;
}

class Chip8;


class Window : public QMainWindow {
    Q_OBJECT
public:

    Window(QWidget* parent, const char* app);

    ~Window() override;

    void load_rom(const char* filename);

    void drawChip8(Chip8* c8);

private slots:

    void open();

protected:

    void resizeEvent(QResizeEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

    void keyReleaseEvent(QKeyEvent* event) override;

private:

    Ui::Window *ui;

    Chip8* c8;
    QTimer* c8loop;

    QImage c8gfx;
    std::map<int, int> keymap;

};

#endif
