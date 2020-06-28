#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QWidget>
#include <QPainter>
#include "mainwindow.h"



namespace Ui {
class WelcomeWindow;
}

class WelcomeWindow : public QWidget
{
    Q_OBJECT


private slots:
    void start();
    void quit();

public:
    explicit WelcomeWindow(QWidget *parent = nullptr);
    ~WelcomeWindow();
    MainWindow *mainwindow;

private:
    Ui::WelcomeWindow *ui;
};

#endif // WELCOMEWINDOW_H
