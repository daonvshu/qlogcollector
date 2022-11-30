#pragma once

#include <qwidget.h>

#include "framelesswindow.h"

namespace Ui {
    class QLogCollector;
}

class QLogCollector : public CFramelessWindow {

public:
    explicit QLogCollector(QWidget* parent = nullptr);
    ~QLogCollector();

private:
    Ui::QLogCollector* ui;
};
