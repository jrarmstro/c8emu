#include <QApplication>
#include "window.h"

int main(int argc, char* argv[]) {
    QApplication app{argc, argv};

    Window window{nullptr, argv[1]};
    window.setWindowTitle("c8emu");
    window.show();

    return app.exec();
}
