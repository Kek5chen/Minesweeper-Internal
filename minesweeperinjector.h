#pragma comment(lib, “psapi.lib”)

#ifndef MINESWEEPERINJECTOR_H
#define MINESWEEPERINJECTOR_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MinesweeperInjector; }
QT_END_NAMESPACE

class MinesweeperInjector : public QMainWindow
{
    Q_OBJECT

public:
    MinesweeperInjector(QWidget *parent = nullptr);
    ~MinesweeperInjector();

private slots:
    void on_btn_inject_clicked();

private:
    Ui::MinesweeperInjector *ui;
    void hoverEnter(QHoverEvent *event);
    void hoverLeave(QHoverEvent *event);
    bool event(QEvent * e);
    void timer_tick();
    void disableInject();
    void disableHwnd();
    void disablePid();
    void disableHandle();
    void disableDll();
};
#endif // MINESWEEPERINJECTOR_H
