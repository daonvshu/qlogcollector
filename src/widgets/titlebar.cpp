#include "titlebar.h"

#include "ui_titlebar.h"

#include <Windows.h>
#include <qevent.h>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
{
    ui = new Ui::TitleBar;
    ui->setupUi(this);

    ui->minisize->clickEvt += [&](bool) {
        parentWidget()->showMinimized();
    };

    ui->maxsize->clickEvt += [&](bool) {
        mouseDoubleClickEvent(nullptr);
    };

    ui->close->clickEvt += [&](bool) {
        parentWidget()->close();
    };

    parent->installEventFilter(this);
}

TitleBar::~TitleBar() {
    delete ui;
}

void TitleBar::mousePressEvent(QMouseEvent *event) {
    ReleaseCapture();
    SendMessage((HWND)parentWidget()->winId(), WM_SYSCOMMAND, 0xF012, 0);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event) {
    if (parentWidget()->isMaximized()) {
        parentWidget()->showNormal();
    } else {
        parentWidget()->showMaximized();
    }
}

bool TitleBar::eventFilter(QObject *watched, QEvent *event) {
    if (watched == parent()) {
        if (event->type() == QEvent::WindowStateChange) {
            if (parentWidget()->windowState() == Qt::WindowMaximized) {
                ui->maxsize->icon = QPixmap(":/ic/resources/maxmize2.png");
            } else if (parentWidget()->windowState() == Qt::WindowNoState) {
                ui->maxsize->icon = QPixmap(":/ic/resources/maxmize.png");
            }
        }
    }
    return QObject::eventFilter(watched, event);
}
