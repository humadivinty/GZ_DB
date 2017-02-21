#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbcontrl.h"
#include "customresultlist.h"
#include "thread_connect_camera.h"
#include "thread_db_local.h"
#include "thread_db_remote.h"
#include "thread_db_savestatus.h"
#include "thread_readcacheresult.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
    long getFileSize(const char *FileName);

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    DBContrl* m_DbContrl;
    CustomResultList m_LocalResultList;
    CustomResultList m_RemoteResultList;
    Camera6467* m_CameraGroup[MAX_CAMERA_COUNT];
    CameraInfo* m_CameraInfoGroup[MAX_CAMERA_COUNT];

    Thread_DB_Local* m_pThread_dbLocal;
    Thread_DB_Remote* m_pThread_dbRemote;
    Thread_Connect_Camera* m_ConnectThread;
    Thread_DB_SaveStatus* m_pThread_Status;
    Thread_ReadCacheResult* m_pThread_ReadLocalCache;
    Thread_ReadCacheResult* m_pThread_ReadRemoteCache;
};

#endif // MAINWINDOW_H
