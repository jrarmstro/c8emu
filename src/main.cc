#include <QApplication>
#include "window.h"

int main(int argc, char* argv[]) {
    QApplication app{argc, argv};

    const char* appname = nullptr;
    if (argc > 1) appname = argv[1];

    Window window{nullptr, appname};
    window.setWindowTitle("c8emu");
    window.show();

    return app.exec();
}
