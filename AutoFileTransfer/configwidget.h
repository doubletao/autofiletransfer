#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QDialog>

namespace Ui {
class ConfigWidget;
}

class MainWindow;
class ConfigWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigWidget(MainWindow *parent);
    ~ConfigWidget();

private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_src_clicked();

    void on_pushButton_des_clicked();

private:
    Ui::ConfigWidget *ui;

    MainWindow * m_pMainWindow;
};

#endif // CONFIGWIDGET_H
