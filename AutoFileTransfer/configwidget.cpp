#include "configwidget.h"
#include "ui_configwidget.h"
#include "mainwindow.h"
#include <QFileDialog>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

ConfigWidget::ConfigWidget(MainWindow *parent)
    : QDialog(parent)
    , ui(new Ui::ConfigWidget)
    , m_pMainWindow(parent)
{
    ui->setupUi(this);

    this->ui->lineEdit_timegap->setText(QString::number(m_pMainWindow->m_nAutoScanTimeGap));
    this->ui->lineEdit_srcpath->setText(m_pMainWindow->m_strSrcDir);
    this->ui->lineEdit_despath->setText(m_pMainWindow->m_strDesDir);
    this->ui->checkBox_delsrc->setChecked(m_pMainWindow->m_bDeleteAfterCopy);
    this->ui->checkBox_autostart->setChecked(m_pMainWindow->m_bAutoStartScan);
    this->ui->checkBox_startwhenpoweron->setChecked(m_pMainWindow->m_bStartWhenPowerOn);
}

ConfigWidget::~ConfigWidget()
{
    delete ui;
}

void ConfigWidget::on_pushButton_ok_clicked()
{
    m_pMainWindow->m_nAutoScanTimeGap = this->ui->lineEdit_timegap->text().toInt();
    m_pMainWindow->m_strSrcDir = this->ui->lineEdit_srcpath->text();
    m_pMainWindow->m_strDesDir = this->ui->lineEdit_despath->text();
    m_pMainWindow->m_bDeleteAfterCopy = this->ui->checkBox_delsrc->isChecked();
    m_pMainWindow->m_bAutoStartScan = this->ui->checkBox_autostart->isChecked();
    m_pMainWindow->m_bStartWhenPowerOn = this->ui->checkBox_startwhenpoweron->isChecked();

    accept();
}

void ConfigWidget::on_pushButton_cancel_clicked()
{
    reject();
}

void ConfigWidget::on_pushButton_src_clicked()
{
    QString currentPath = this->ui->lineEdit_srcpath->text();

    // 打开路径选择对话框，将路径定位到当前路径
    QString newPath = QFileDialog::getExistingDirectory(this, "选择路径", currentPath);

    // 如果用户选择了一个新路径，则保存到 lineEdit_srcpath
    if (!newPath.isEmpty()) {
        this->ui->lineEdit_srcpath->setText(newPath);
    }
}

void ConfigWidget::on_pushButton_des_clicked()
{
    QString currentPath = this->ui->lineEdit_despath->text();

    // 打开路径选择对话框，将路径定位到当前路径
    QString newPath = QFileDialog::getExistingDirectory(this, "选择路径", currentPath);

    // 如果用户选择了一个新路径，则保存到 lineEdit_srcpath
    if (!newPath.isEmpty()) {
        this->ui->lineEdit_despath->setText(newPath);
    }
}
