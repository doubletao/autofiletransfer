#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <list>
#include <QSystemTrayIcon>
#include <atomic>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
signals:
    void sigAppendLog(QString);
public slots:
    void slotAppendLog(QString);
    void OnTimerTimeout();

private slots:
    void on_btn_switch_clicked();
    void exit();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void InitTray();
    void ReadConfig();
    void WriteConfig();

    void ExcuteTranferAction();
private:
    Ui::MainWindow *ui;

    QSystemTrayIcon* trayIcon;

    QTimer m_timer;
    std::list<QString> m_lstLog;
    int m_nAutoScanTimeGap;
    QString m_strSrcDir;
    QString m_strDesDir;
    bool m_bDeleteAfterCopy;
    std::atomic_bool m_bProcessRunning;
};
#endif // MAINWINDOW_H
