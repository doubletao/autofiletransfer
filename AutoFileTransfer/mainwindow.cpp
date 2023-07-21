#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commonfunc/commonfunc.h"
#include <QTextBlock>
#include <QMessageBox>
#include <QSettings>
#include <thread>
#include "configwidget.h"
#include <QDir>
#include <QTextStream>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_nAutoScanTimeGap(1000)
{
    ui->setupUi(this);

    connect(this, &MainWindow::sigAppendLog, this, &MainWindow::slotAppendLog, Qt::QueuedConnection);
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::OnTimerTimeout);

    commonfunc::Inst().SetLogTextEdit(this);

    this->ui->textEdit_log->setMaximumBlockCount(30);

    InitTray();
    ReadConfig();

    m_bProcessRunning = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotAppendLog(QString strLog)
{
    this->ui->textEdit_log->appendPlainText(strLog);
}


void MainWindow::OnTimerTimeout()
{
    commonfunc::Inst().commonfunc::WriteLog("正在执行定时检测...", true);
    ExcuteTranferAction();
}

void MainWindow::on_btn_switch_clicked()
{
    if (m_timer.isActive())
    {
        m_timer.stop();
        commonfunc::Inst().commonfunc::WriteLog("定时传输已停止!", true);
        ui->btn_switch->setText("开始");
        this->ui->btn_config->setEnabled(true);
    }
    else
    {
        m_timer.start(m_nAutoScanTimeGap);
        commonfunc::Inst().commonfunc::WriteLog("开始定时传输!", true);

        ui->btn_switch->setText("停止");
        this->ui->btn_config->setEnabled(false);
    }
}

void MainWindow::exit()
{
    // 在退出应用程序之前，隐藏托盘图标
    trayIcon->hide();
    qApp->quit(); // 退出应用程序
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    // 检查触发的原因是否为双击事件
    if (reason == QSystemTrayIcon::DoubleClick) {
        // 显示或还原主窗口
        showNormal();
        this->activateWindow();
    }
}

void MainWindow::InitTray()
{
    // 设置窗口标志为Qt::Tool，从任务栏上移除
    setWindowFlags(Qt::Tool);

    // 创建系统托盘图标
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/res/prefix/autoupload.ico")); // 设置托盘图标
    trayIcon->setVisible(true); // 显示托盘图标

    // 创建托盘菜单
    QMenu* trayMenu = new QMenu(this);
    QAction* exitAction = new QAction("退出", this);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exit);

    trayMenu->addAction(exitAction);

    // 将托盘菜单关联到托盘图标
    trayIcon->setContextMenu(trayMenu);

    // 连接activated()信号到自定义槽函数
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayActivated);
}

void MainWindow::ReadConfig()
{
    //read ini file
    std::string strConfigPath = QCoreApplication::applicationDirPath().toStdString();
    strConfigPath.append("/config.ini");
    QSettings settings(strConfigPath.c_str(), QSettings::IniFormat);
    m_nAutoScanTimeGap = settings.value("autoscan/timeGap", 30000).toInt();
    m_strSrcDir = settings.value("path/srcDir", "").toString();
    m_strDesDir = settings.value("path/desDir", "").toString();
    m_bDeleteAfterCopy = settings.value("action/delAfterTransfer", false).toBool();

    QString strConfigOverview = QString("扫描间隔：%1\n扫描目录：%2\n目标路径：%3\n拷贝完成后删除：%4")
            .arg(m_nAutoScanTimeGap).arg(m_strSrcDir).arg(m_strDesDir).arg(m_bDeleteAfterCopy ? "是" : "否");
    this->ui->textEdit_config->setPlainText(strConfigOverview);
}

void MainWindow::WriteConfig()
{
    std::string strConfigPath = QCoreApplication::applicationDirPath().toStdString();
    strConfigPath.append("/config.ini");
    QSettings settings(strConfigPath.c_str(), QSettings::IniFormat);
    settings.setValue("autoscan/timeGap", m_nAutoScanTimeGap);
    settings.setValue("path/srcDir", m_strSrcDir);
    settings.setValue("path/desDir", m_strDesDir);
    settings.setValue("action/delAfterTransfer", m_bDeleteAfterCopy);
}

bool MainWindow::copyFile(const QString& sourceFilePath, const QString& destinationDirectory)
{
    QFileInfo sourceFileInfo(sourceFilePath);
    QString destinationFilePath = QDir(destinationDirectory).filePath(sourceFileInfo.fileName());

    if (QFile::exists(destinationFilePath))
    {
        commonfunc::Inst().commonfunc::WriteLog("目标文件夹存在同名文件，拷贝放弃...", true);
        return true;
    }

    if (!QFile::copy(sourceFilePath, destinationFilePath))
    {
        commonfunc::Inst().commonfunc::WriteLog("拷贝失败，请确保目标文件夹可访问且程序有写入权限...", true);
        return false;
    }

    return true;
}

void MainWindow::ExcuteTranferAction()
{
    if (m_bProcessRunning)
    {
        commonfunc::Inst().commonfunc::WriteLog("上次任务未完成，放弃本次任务...", true);
        return;
    }
    m_bProcessRunning = true;
    std::thread th([this]()
    {
        //扫描源文件夹，发现文件
        std::set<QString> fileList = scanFilesRecursive(m_strSrcDir);
        commonfunc::Inst().commonfunc::WriteLog(QString("%1 个文件被发现").arg(fileList.size()), true);

        QString strTransferedFileListPath = m_strDesDir + "/transfer.list";

        //从目标文件夹里边读取已传输列表
        QStringList lstTransferFile = loadStringListFromFile(strTransferedFileListPath);
        commonfunc::Inst().commonfunc::WriteLog(QString("%1 个文件已传输").arg(lstTransferFile.size()), true);

        //比对已传输列表，过滤文件
        for (auto it : lstTransferFile)
        {
            fileList.erase(it);
        }
        commonfunc::Inst().commonfunc::WriteLog(QString("%1 个文件需要传输").arg(fileList.size()), true);

        //传输文件
        for (auto it : fileList)
        {
            commonfunc::Inst().commonfunc::WriteLog(QString("正在传输 %1").arg(it), true);

            copyFile(it, m_strDesDir);

            lstTransferFile.append(it);
            commonfunc::Inst().commonfunc::WriteLog(QString("%1 传输完成").arg(it), true);

            if (m_bDeleteAfterCopy)
            {
                if (QFile::remove(it))
                {
                    commonfunc::Inst().commonfunc::WriteLog(QString("%1 删除成功").arg(it), true);
                }
                else
                {
                    commonfunc::Inst().commonfunc::WriteLog(QString("%1 删除失败").arg(it), true);
                }
            }
        }

        //保存传输完成列表
        saveStringListToFile(lstTransferFile, strTransferedFileListPath);

        m_bProcessRunning = false;
    });
    th.detach();
}

std::set<QString> MainWindow::scanFilesRecursive(const QString &path)
{
    std::set<QString> filesList;
    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList fileInfoList = dir.entryInfoList();

    // 遍历当前目录中的文件和文件夹
    foreach (const QFileInfo& fileInfo, fileInfoList) {
        if (fileInfo.isDir()) {
            // 如果是文件夹，则递归调用自身进行文件夹下的文件扫描
            QString subDirPath = fileInfo.absoluteFilePath();
            std::set<QString> subDirFiles = scanFilesRecursive(subDirPath);
            filesList.insert(subDirFiles.begin(), subDirFiles.end());
        } else {
            // 如果是文件，则将文件路径添加到列表中
            QString filePath = fileInfo.absoluteFilePath();
            filesList.insert(filePath);
        }
    }

    return filesList;
}


void MainWindow::on_btn_config_clicked()
{
    ConfigWidget ConfigWid(this);
    if (ConfigWid.exec() == QDialog::Accepted)
    {
        commonfunc::Inst().commonfunc::WriteLog("配置已保存", true);
        WriteConfig();
        ReadConfig();
    }
}

void MainWindow::saveStringListToFile(const QStringList& stringList, const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString& str : stringList) {
            out << str << '\n';
        }
        file.close();
    }
}

QStringList MainWindow::loadStringListFromFile(const QString& filePath) {
    QStringList stringList;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            stringList.append(line);
        }
        file.close();
    }
    return stringList;
}

