#include "rendersettings.h"
#include "ui_rendersettings.h"

#include <renderer/DICOMRenderManager.h>

RenderSettings::RenderSettings(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::RenderSettings)
{
    ui->setupUi(this);

    ui->displayFrame->setChecked(DicomRenderManager::getInstance().getDisplayFrameShapes());
    ui->displaydetectionbox->setChecked(DicomRenderManager::getInstance().getDisplayFrameDetectionBox());
    ui->displayboundingbox->setChecked(DicomRenderManager::getInstance().getDisplayBoundingBox());
    ui->displayelectrodes->setChecked(DicomRenderManager::getInstance().getDisplayElectrodes());

    ui->performanceslider->setValue((DicomRenderManager::getInstance().getPerformanceValue())*1000.0f);
    ui->blendingslider->setValue(DicomRenderManager::getInstance().getBlendValue()*10000.0f);

    setFloating(true);
    show();
}

RenderSettings::~RenderSettings()
{
    delete ui;
}

void RenderSettings::on_displayFrame_clicked()
{
    DicomRenderManager::getInstance().setDisplayFrameShapes(ui->displayFrame->isChecked());
    DicomRenderManager::getInstance().forceRepaint();
}

void RenderSettings::on_displaydetectionbox_clicked()
{
    DicomRenderManager::getInstance().setDisplayFrameDetectionBox(ui->displaydetectionbox->isChecked());
    DicomRenderManager::getInstance().forceRepaint();
}

void RenderSettings::on_displayboundingbox_clicked()
{
    DicomRenderManager::getInstance().setDisplayBoundingBox(ui->displayboundingbox->isChecked());
    DicomRenderManager::getInstance().forceRepaint();
}

void RenderSettings::on_displayelectrodes_clicked()
{
    DicomRenderManager::getInstance().setDisplayElectrodes(ui->displayelectrodes->isChecked());
    DicomRenderManager::getInstance().forceRepaint();
}

void RenderSettings::on_performanceslider_valueChanged(int value)
{
    DicomRenderManager::getInstance().setPerformanceValue( ((float)value/1000.0f)  );
    DicomRenderManager::getInstance().forceRepaint();
}

void RenderSettings::on_blendingslider_valueChanged(int value)
{
    DicomRenderManager::getInstance().setBlendValue((float)value/10000.0f);
    DicomRenderManager::getInstance().forceRepaint();
}

void RenderSettings::on_orthonormalThreeD_clicked(bool checked)
{
    DicomRenderManager::getInstance().setOrthonormalThreeD(checked);
    DicomRenderManager::getInstance().forceRepaint();
}
