#pragma once

#include <qwidget.h>

namespace Ui {
    class TitleBar;
}

class TitleBar : public QWidget {
public:
    explicit TitleBar(QWidget* parent = nullptr);
    ~TitleBar();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::TitleBar* ui;
};
