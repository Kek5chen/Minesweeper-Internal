#include "minesweeperinjector.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MinesweeperInjector w;
    w.show();
    return a.exec();
}
