#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_direct.h"
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QVector>
#include <dshow.h>
#include <windows.h>
#include <Ks.h>
#include <Ksmedia.h>

struct Property
{
    int m_pos;
    long m_flags;
};

class direct : public QMainWindow
{
    Q_OBJECT

public:
    direct(QWidget *parent = Q_NULLPTR);

private:
    void initDirect();
    void initParameter(bool isDefault = false);
    void getCamera();
    void display();
    void connectConfig();

    Ui::directClass ui;

    QCamera *m_camera;
    QCameraViewfinder *m_viewfinder;
    QVector<QCameraInfo> m_camera_info;
    HWND m_ghwndApp;
    QList<IBaseFilter *> m_pSrcList;
    IAMVideoProcAmp *m_pProcAmp;

private slots:
    void chooseCamera(int index);
    void showFilter();
    void setFilter(VideoProcAmpProperty procAmp, int pos);
    void setFilter(KSPROPERTY_VIDCAP_VIDEOPROCAMP procAmp, int index);
    void setFilter(VideoProcAmpProperty procAmp, bool isOn);
    Property getFilter(VideoProcAmpProperty procAmp, bool isDefault);
    long getFilter(KSPROPERTY_VIDCAP_VIDEOPROCAMP procAmp, bool isDefault);
};
