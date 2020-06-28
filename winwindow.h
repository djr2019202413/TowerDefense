#ifndef WINWINDOW_H
#define WINWINDOW_H

#include <QWidget>

namespace Ui {
class WinWindow;
}

class WinWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WinWindow(QWidget *parent = nullptr);
    ~WinWindow();

private slots:
    void quit();

private:
    Ui::WinWindow *ui;
};

#endif // WINWINDOW_H
