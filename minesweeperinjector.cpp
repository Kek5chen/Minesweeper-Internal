#include "minesweeperinjector.h"
#include "ui_minesweeperinjector.h"

#define PSAPI_VERSION 1

#include <QPropertyAnimation>
#include <QTimer>
#include <Windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <fstream>
#include <QFile>
#include <QDir>

#define UNUSED_PARAMETER(x) (void)(x)
#define OPACITY_UNHOVERED .5
#define OPACITY_HOVERED 1
#define OPACITY_ANIMATION_SPEED 300

MinesweeperInjector::MinesweeperInjector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MinesweeperInjector)
{
    this->setAttribute(Qt::WA_Hover, true);
    this->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    ui->setupUi(this);
    this->setFixedSize(this->size());
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MinesweeperInjector::timer_tick);
    timer->start(500);
}

MinesweeperInjector::~MinesweeperInjector()
{
    delete ui;
}

void MinesweeperInjector::hoverEnter(QHoverEvent *event) {
    UNUSED_PARAMETER(event);
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(OPACITY_ANIMATION_SPEED);
    animation->setStartValue(this->windowOpacity());
    animation->setEndValue(OPACITY_HOVERED);
    animation->start();
}

void MinesweeperInjector::hoverLeave(QHoverEvent *event) {
    UNUSED_PARAMETER(event);
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(OPACITY_ANIMATION_SPEED);
    animation->setStartValue(this->windowOpacity());
    animation->setEndValue(OPACITY_UNHOVERED);
    animation->start();
}

bool MinesweeperInjector::event(QEvent * e)
{
    switch(e->type())
    {
    case QEvent::HoverEnter:
        hoverEnter((QHoverEvent*)(e));
        return true;
        break;
    case QEvent::HoverLeave:
        hoverLeave((QHoverEvent*)(e));
        return true;
        break;
    default:
        break;
    }
    return QWidget::event(e);
}

bool injected = false, init = false;

void MinesweeperInjector::disableInject() {
    this->ui->btn_inject->setEnabled(false);
    injected = false;
}

void MinesweeperInjector::disableDll() {
    this->ui->lbl_dll->setText("No");
    this->ui->lbl_dll->setStyleSheet("color: red;");
    disableInject();
}

void MinesweeperInjector::disableHandle() {
    this->ui->lbl_handle->setText("No");
    this->ui->lbl_handle->setStyleSheet("color: red;");
    disableInject();
    init = false;
}

void MinesweeperInjector::disablePid() {
    this->ui->lbl_pid->setText("PID: 0");
    disableInject();
}

void MinesweeperInjector::disableHwnd() {
    this->ui->lbl_status->setText("No");
    this->ui->lbl_status->setStyleSheet("color: red;");
    this->ui->lbl_injected->setText("No");
    this->ui->lbl_injected->setStyleSheet("color: red;");
    disablePid();
    disableHandle();
    disableInject();
}

HANDLE hProc;
HWND hwndMSWP;
QString filePath;

void MinesweeperInjector::timer_tick() {

    if((injected || !init) && hProc) {
        HMODULE hModules[1024];
        DWORD cbNeeded;
        if(EnumProcessModules(hProc, hModules, sizeof(hModules), &cbNeeded)) {
            bool isLoaded = false;
            for(uint i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                char szModName[MAX_PATH];
                GetModuleFileNameExA(hProc, hModules[i], szModName, MAX_PATH);
                if(filePath.compare(szModName) == 0) {
                    isLoaded = true;
                    break;
                }
            }
            injected = isLoaded;
            if(!injected) {
                this->ui->lbl_injected->setText("No");
                this->ui->lbl_injected->setStyleSheet("color: red;");
            }
        }
    }

    bool enableInject = true;
    QFile file("MinesweeperInternal.dll");

    if(!file.exists()) {
        disableDll();
        enableInject = false;
    } else {
        filePath = QDir("MinesweeperInternal.dll").absolutePath();
        filePath.replace("/", "\\");
        this->ui->lbl_dll->setText("Yes");
        this->ui->lbl_dll->setStyleSheet("color: green;");
    }

    // FIND WINDOW

    hwndMSWP = FindWindowA(0, "Minesweeper");
    if(!hwndMSWP) {
        disableHwnd();
        hProc = 0;
        enableInject = false;

    } else {
        this->ui->lbl_status->setText("Yes");
        this->ui->lbl_status->setStyleSheet("color: green;");

        // FIND PID

        ulong pid;
        GetWindowThreadProcessId(hwndMSWP, &pid);
        if(!pid) {
            disablePid();
            enableInject = false;
        } else {
            this->ui->lbl_pid->setText("PID: " + QString::number(pid));

            // OPEN HANDLE

            if(!hProc || hProc == INVALID_HANDLE_VALUE)
                hProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
            else
                init = true;
            if(!hProc || hProc == INVALID_HANDLE_VALUE) {
                disableHandle();
                enableInject = false;
                hProc = 0;
            } else {
                this->ui->lbl_handle->setText("Yes");
                this->ui->lbl_handle->setStyleSheet("color: green;");
            }
        }
    }
    if(enableInject)
        this->ui->btn_inject->setEnabled(true);
    else
        disableInject();
}

void clean(LPVOID addr) {
    //VirtualFreeEx(hProc, addr, 0, MEM_RELEASE);
}

void MinesweeperInjector::on_btn_inject_clicked()
{
    if(hwndMSWP && hProc && hProc != INVALID_HANDLE_VALUE && filePath.size() > 20) {
        if(injected) {
            MessageBoxA((HWND)winId(), "Already injected!", "Already injected", 0);
            return;
        }
        QByteArray const path = filePath.toLocal8Bit(); // < filePath definitely has the right path with backslashes
        LPVOID const allocMemAddr = VirtualAllocEx(hProc, 0, path.size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if(!allocMemAddr) {
            MessageBoxA((HWND)winId(), "ERROR: Couldn't allocate memory!", "Injection Error", 0);
            return;
        }
        if(!WriteProcessMemory(hProc, allocMemAddr, (LPVOID)path.constData(), path.size() + 1, 0)) {
            MessageBoxA((HWND)winId(), "ERROR: Couldn't write memory!", "Injection Error", 0);
            clean(allocMemAddr);
            return;
        }

        FARPROC libraryAddress = GetProcAddress(LoadLibraryA("kernel32"), "LoadLibraryA");
        if(!libraryAddress) {
            MessageBoxA((HWND)winId(), "ERROR: Couldn't get kernel32 export function address!", "Injection Error", 0);
            clean(allocMemAddr);
            return;
        }
        DWORD dwThreadId;
        HANDLE hRemoteThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)libraryAddress, allocMemAddr, 0, &dwThreadId);
        if(!hRemoteThread || hRemoteThread == INVALID_HANDLE_VALUE) {
            dwThreadId = 0;
            clean(allocMemAddr);
            MessageBoxA((HWND)winId(), "ERROR: Couldn't create remote thread!", "Injection Error", 0);
            return;
        }
        WaitForSingleObject(hRemoteThread, INFINITE);
        CloseHandle(hRemoteThread);
        clean(allocMemAddr);
        this->ui->lbl_injected->setText("Yes");
        this->ui->lbl_injected->setStyleSheet("color: green;");
        this->ui->lbl_injected->update();
        injected = true;
        MessageBoxA((HWND)winId(), "Injected successfully!", "Injection Successfull", 0);
    }
}

