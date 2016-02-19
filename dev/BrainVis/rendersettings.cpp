#include "rendersettings.h"
#include "ui_rendersettings.h"

#include <renderer/DICOMRenderer/DICOMRenderManager.h>

RenderSettings::RenderSettings(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::RenderSettings)
{
    ui->setupUi(this);

    DicomRenderManager::getInstance().setDisplayFrameShapes(ui->displayFrame->isChecked());
    DicomRenderManager::getInstance().setDisplayFrameDetectionBox(ui->displaydetectionbox->isChecked());
    DicomRenderManager::getInstance().setDisplayBoundingBox(ui->displayboundingbox->isChecked());
    DicomRenderManager::getInstance().setDisplayElectrodes(ui->displayelectrodes->isChecked());
    DicomRenderManager::getInstance().setPerformanceValue((float) ui->performanceslider->value() /1000.0f);
    DicomRenderManager::getInstance().setBlendValue((float)ui->blendingslider->value()/10000.0f);

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
}

void RenderSettings::on_displaydetectionbox_clicked()
{
    DicomRenderManager::getInstance().setDisplayFrameDetectionBox(ui->displaydetectionbox->isChecked());
}

void RenderSettings::on_displayboundingbox_clicked()
{
    DicomRenderManager::getInstance().setDisplayBoundingBox(ui->displayboundingbox->isChecked());
}

void RenderSettings::on_displayelectrodes_clicked()
{
    DicomRenderManager::getInstance().setDisplayElectrodes(ui->displayelectrodes->isChecked());
}

void RenderSettings::on_performanceslider_valueChanged(int value)
{
    DicomRenderManager::getInstance().setPerformanceValue( 1.0f/((float)value/1000.0f)  );
}

void RenderSettings::on_blendingslider_valueChanged(int value)
{
    DicomRenderManager::getInstance().setBlendValue((float)value/10000.0f);
}

void RenderSettings::on_orthonormalThreeD_clicked(bool checked)
{
    DicomRenderManager::getInstance().setOrthonormalThreeD(checked);
}
