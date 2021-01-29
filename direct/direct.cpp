#include "direct.h"
#include <QMessageBox>

direct::direct(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    initDirect();
    initParameter();
    getCamera();
    display();
    connectConfig();
}

void direct::connectConfig()
{
    connect(ui.selectCamera, qOverload<int>(&QComboBox::currentIndexChanged), this, &direct::chooseCamera);

    connect(ui.brightnessSlider, &QSlider::valueChanged, this, [=](int value) {
        ui.brightnessLabel->setText(QString("%1%").arg(value));
        setFilter(VideoProcAmp_Brightness, value);
        });
    
    connect(ui.contrastSlider, &QSlider::valueChanged, this, [=](int value) {
        ui.contrastLabel->setText(QString("%1%").arg(value));
        setFilter(VideoProcAmp_Contrast, value);
        });

    connect(ui.saturationSlider, &QSlider::valueChanged, this, [=](int value) {
        ui.saturationLabel->setText(QString("%1%").arg(value));
        setFilter(VideoProcAmp_Saturation, value);
        });

    connect(ui.sharpenSlider, &QSlider::valueChanged, this, [=](int value) {
        ui.sharpenLabel->setText(QString("%1%").arg(value));
        setFilter(VideoProcAmp_Sharpness, value);
        });

    connect(ui.wbSlider, &QSlider::valueChanged, this, [=](int value) {
        ui.wbLabel->setText(QString("%1%").arg(value));
        setFilter(VideoProcAmp_WhiteBalance, value);
        });

    connect(ui.wbAutoBox, &QCheckBox::clicked, this, [=](bool isOn) {
        ui.wbSlider->setDisabled(isOn);
        setFilter(VideoProcAmp_WhiteBalance, isOn);
        });

    connect(ui.plfBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [=](int index) {
        setFilter(KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY, index + 1);
        });

    connect(ui.resetButton, &QPushButton::clicked, this, [=]() {
        int ret = QMessageBox::question(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�����ָ�Ĭ�����ã��Ƿ������"), 
            QString::fromLocal8Bit("ȷ��"), QString::fromLocal8Bit("ȡ��"));

        if (ret == 0)
        {
            initParameter(true);
        }
        });
}

void direct::initParameter(bool isDefault)
{
    // ������Ƶ��
    long val = getFilter(KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY, isDefault);
    ui.plfBox->setCurrentIndex(val - 1);

    // ����
    Property property = getFilter(VideoProcAmp_Brightness, isDefault);
    ui.brightnessSlider->setValue(property.m_pos);
    ui.brightnessLabel->setText(QString("%1%").arg(property.m_pos));

    // �Աȶ�
    property = getFilter(VideoProcAmp_Contrast, isDefault);
    ui.contrastSlider->setValue(property.m_pos);
    ui.contrastLabel->setText(QString("%1%").arg(property.m_pos));

    // ���Ͷ�
    property = getFilter(VideoProcAmp_Saturation, isDefault);
    ui.saturationSlider->setValue(property.m_pos);
    ui.saturationLabel->setText(QString("%1%").arg(property.m_pos));

    // ���
    property = getFilter(VideoProcAmp_Sharpness, isDefault);
    ui.sharpenSlider->setValue(property.m_pos);
    ui.sharpenLabel->setText(QString("%1%").arg(property.m_pos));

    // ��ƽ��
    property = getFilter(VideoProcAmp_WhiteBalance, isDefault);
    ui.wbSlider->setValue(property.m_pos);
    ui.wbLabel->setText(QString("%1%").arg(property.m_pos));
    
    // ��ƽ���Զ���ť
    bool isOn = property.m_flags == VideoProcAmp_Flags_Auto ? true : false;
    ui.wbAutoBox->setChecked(isOn);
    ui.wbSlider->setDisabled(isOn);
    setFilter(VideoProcAmp_WhiteBalance, isOn);
}

void direct::initDirect()
{
    CoInitialize(nullptr);

    m_ghwndApp = 0;
    m_pProcAmp = nullptr;

    // ö����Ƶ�豸
    ICreateDevEnum *pDevEnum = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC, IID_ICreateDevEnum, (void **)&pDevEnum);

    if (FAILED(hr))
    {
        return;
    }

    IEnumMoniker *pClassEnum = nullptr;

    // Ϊָ����Ŀ¼����ö����
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);

    if (hr == S_OK)
    {
        // ʹ�� IEnumMoniker �ӿ�ö�����е��豸��ʶ
        IMoniker *pMoniker = nullptr;
        ULONG cFetched;

        while (pClassEnum->Next(1, &pMoniker, &cFetched) == S_OK)
        {
            IBaseFilter *pSrc = nullptr;
            pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&pSrc);
            m_pSrcList.push_back(pSrc);
            pMoniker->Release();
        }
    }

    pClassEnum->Release();

    hr = m_pSrcList[0]->QueryInterface(IID_IAMVideoProcAmp, (void **)&m_pProcAmp);
}

void direct::getCamera()
{
    int flag = 1;

    while (m_camera_info.count() == 0)
    {
        m_camera_info.clear();
        ui.selectCamera->clear();

        foreach(QCameraInfo info, QCameraInfo::availableCameras())
        {
            m_camera_info.push_back(info);
            ui.selectCamera->addItem(info.description());
            flag = 0;
            qDebug() << info.availableCameras();
        }

        if (flag)
        {
            int ret = QMessageBox::question(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ��⵽����ͷ�����������ͷ"), 
                QString::fromLocal8Bit("ȷ��"), QString::fromLocal8Bit("ȡ��"));

            if (ret == 1)
            {
                exit(0);
            }
        }
    }
}

void direct::display()
{
    m_camera = new QCamera(m_camera_info.at(ui.selectCamera->currentIndex()));
    m_viewfinder = new QCameraViewfinder(ui.displayWidget);
    m_camera->setViewfinder(m_viewfinder);
    m_camera->start();
}

void direct::chooseCamera(int index)
{
    m_camera->stop();
    delete m_camera;
    m_camera = new QCamera(m_camera_info.at(index));
    m_camera->setViewfinder(m_viewfinder);
    m_camera->start();
    m_pSrcList[index]->QueryInterface(IID_IAMVideoProcAmp, (void **)&m_pProcAmp);
    initParameter();
}

void direct::showFilter()
{
    // �ɵ� directshow ԭ�����ý���
    //ISpecifyPropertyPages *pSpec;
    //CAUUID cauuid;
    //HRESULT hr = m_pSrcList[ui.selectCamera->currentIndex()]->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);

    //if (hr == S_OK)
    //{
    //    hr = pSpec->GetPages(&cauuid);

    //    hr = OleCreatePropertyFrame(m_ghwndApp, 30, 30, NULL, 1,       // ��������
    //        (IUnknown **)&m_pSrcList[ui.selectCamera->currentIndex()], cauuid.cElems,
    //        (GUID *)cauuid.pElems, 0, 0, NULL);

    //    CoTaskMemFree(cauuid.pElems);     // �رղ�������
    //    pSpec->Release();
    //}
}

void direct::setFilter(VideoProcAmpProperty procAmp, int pos)
{
    long Min = 0;
    long Max = 0;
    long Step = 0;
    long Default = 0;
    long Flags = 0;
    long Val = 0;

    // Min����Сֵ Max�����ֵ Step������� Default��Ĭ��ֵ Flags���Զ�/�ֶ���־λ���˴� Flags Ϊ 3 ������Զ���Ϊ 2 ��ֻ���ֶ���
    m_pProcAmp->GetRange(procAmp, &Min, &Max, &Step, &Default, &Flags);   // ��ȡ��������
    Val = Min + (Max - Min) * pos / 100;
    m_pProcAmp->Set(procAmp, Val, Flags);
}

void direct::setFilter(KSPROPERTY_VIDCAP_VIDEOPROCAMP procAmp, int index)
{
    m_pProcAmp->Set(procAmp, index, 0);
}

void direct::setFilter(VideoProcAmpProperty procAmp, bool isOn)
{
    long Min = 0;
    long Max = 0;
    long Step = 0;
    long Default = 0;
    long Flags = 0;
    long Val = 0;

    m_pProcAmp->GetRange(procAmp, &Min, &Max, &Step, &Default, &Flags);   // ��ȡ��������
    m_pProcAmp->Get(procAmp, &Val, &Flags);
    Flags = isOn ? VideoProcAmp_Flags_Auto : VideoProcAmp_Flags_Manual;
    m_pProcAmp->Set(procAmp, Val, Flags);
}

Property direct::getFilter(VideoProcAmpProperty procAmp, bool isDefault)
{
    long Min = 0;
    long Max = 0;
    long Step = 0;
    long Default = 0;
    long Flags = 0;
    long Val = 0;

    m_pProcAmp->GetRange(procAmp, &Min, &Max, &Step, &Default, &Flags);
    m_pProcAmp->Get(procAmp, &Val, &Flags);

    if (isDefault)    // �ָ�Ĭ������
    {
        Val = (double)(Default - Min) / (double)(Max - Min) * 100;
    }
    else
    {
        Val = (double)(Val - Min) / (double)(Max - Min) * 100;
    }

    Property ret = {Val, Flags};
    return ret;
}

long direct::getFilter(KSPROPERTY_VIDCAP_VIDEOPROCAMP procAmp, bool isDefault)
{
    long Min = 0;
    long Max = 0;
    long Step = 0;
    long Val = 0;
    long Default = 0;
    long Flags = 0;
    m_pProcAmp->GetRange(procAmp, &Min, &Max, &Step, &Default, &Flags);
    m_pProcAmp->Get(procAmp, &Val, &Flags);

    if (isDefault)    // �ָ�Ĭ������
    {
        return Default;
    }

    return Val;
}