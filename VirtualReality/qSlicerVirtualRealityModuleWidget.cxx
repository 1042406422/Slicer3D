/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#include "qSlicerVirtualRealityModule.h"
#include "qSlicerVirtualRealityModuleWidget.h"
#include "ui_qSlicerVirtualRealityModuleWidget.h"

#include "vtkSlicerVirtualRealityLogic.h"
#include "vtkMRMLVirtualRealityViewNode.h"
#include "qMRMLVirtualRealityView.h"

#include <QDebug>
#include <ctkDoubleSpinBox.h>

//-----------------------------------------------------------------------------
class qSlicerVirtualRealityModuleWidgetPrivate: public Ui_qSlicerVirtualRealityModuleWidget
{
public:
  qSlicerVirtualRealityModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
qSlicerVirtualRealityModuleWidgetPrivate::qSlicerVirtualRealityModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
qSlicerVirtualRealityModuleWidget::qSlicerVirtualRealityModuleWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVirtualRealityModuleWidgetPrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerVirtualRealityModuleWidget::~qSlicerVirtualRealityModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::setup()
{
  Q_D(qSlicerVirtualRealityModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  connect(d->ConnectCheckBox, SIGNAL(toggled(bool)), this, SLOT(setStereoConnected(bool)));
  connect(d->RenderingEnabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(setStereoActive(bool)));
  connect(d->TwoSidedLightingCheckBox, SIGNAL(toggled(bool)), this, SLOT(setTwoSidedLighting(bool)));
  connect(d->BackLightsCheckBox, SIGNAL(toggled(bool)), this, SLOT(setBackLights(bool)));
  connect(d->ReferenceViewNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(setReferenceViewNode(vtkMRMLNode*)));
  connect(d->UpdateViewFromReferenceViewCameraButton, SIGNAL(clicked()), this, SLOT(updateViewFromReferenceViewCamera()));

  connect(d->DesiredUpdateRateSlider, SIGNAL(valueChanged(double)), this, SLOT(onDesiredUpdateRateChanged(double)));
  connect(d->MotionSensitivitySlider, SIGNAL(valueChanged(double)), this, SLOT(onMotionSensitivityChanged(double)));
  connect(d->MagnificationSlider, SIGNAL(valueChanged(double)), this, SLOT(onMagnificationChanged(double)));

  this->updateWidgetFromMRML();

  qvtkConnect(logic(), vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()));
}

//--------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerVirtualRealityModuleWidget);
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();

  bool wasBlocked = d->ConnectCheckBox->blockSignals(true);
  d->ConnectCheckBox->setChecked(vrViewNode != nullptr && vrViewNode->GetVisibility());
  d->ConnectCheckBox->blockSignals(wasBlocked);

  QString errorText;
  if (vrViewNode && vrViewNode->HasError())
  {
    errorText = vrViewNode->GetError().c_str();
  }
  d->ConnectionStatusLabel->setText(errorText);

  wasBlocked = d->RenderingEnabledCheckBox->blockSignals(true);
  d->RenderingEnabledCheckBox->setChecked(vrViewNode != nullptr && vrViewNode->GetActive());
  d->RenderingEnabledCheckBox->blockSignals(wasBlocked);

  wasBlocked = d->DesiredUpdateRateSlider->blockSignals(true);
  d->DesiredUpdateRateSlider->setValue(vrViewNode != nullptr ? vrViewNode->GetDesiredUpdateRate() : 0);
  d->DesiredUpdateRateSlider->setEnabled(vrViewNode != nullptr);
  d->DesiredUpdateRateSlider->blockSignals(wasBlocked);

  wasBlocked = d->MagnificationSlider->blockSignals(true);
  d->MagnificationSlider->setValue(vrViewNode != nullptr
                                   ? this->getPowerFromMagnification(vrViewNode->GetMagnification())
                                   : 1.0);
  d->MagnificationValueLabel->setText(vrViewNode != nullptr
                                      ? QString("%1x").arg(vrViewNode->GetMagnification(), 3, 'f', 2)
                                      : "10.0");
  d->MagnificationSlider->setEnabled(vrViewNode != nullptr);
  d->MagnificationSlider->blockSignals(wasBlocked);

  wasBlocked = d->MotionSensitivitySlider->blockSignals(true);
  d->MotionSensitivitySlider->setValue(vrViewNode != nullptr ? vrViewNode->GetMotionSensitivity() * 100.0 : 0);
  d->MotionSensitivitySlider->setEnabled(vrViewNode != nullptr);
  d->MotionSensitivitySlider->blockSignals(wasBlocked);

  wasBlocked = d->TwoSidedLightingCheckBox->blockSignals(true);
  d->TwoSidedLightingCheckBox->setChecked(vrViewNode != nullptr && vrViewNode->GetTwoSidedLighting());
  d->TwoSidedLightingCheckBox->setEnabled(vrViewNode != nullptr);
  d->TwoSidedLightingCheckBox->blockSignals(wasBlocked);

  wasBlocked = d->BackLightsCheckBox->blockSignals(true);
  d->BackLightsCheckBox->setChecked(vrViewNode != nullptr && vrViewNode->GetBackLights());
  d->BackLightsCheckBox->setEnabled(vrViewNode != nullptr);
  d->BackLightsCheckBox->blockSignals(wasBlocked);

  wasBlocked = d->ReferenceViewNodeComboBox->blockSignals(true);
  d->ReferenceViewNodeComboBox->setCurrentNode(vrViewNode != nullptr ? vrViewNode->GetReferenceViewNode() : nullptr);
  d->ReferenceViewNodeComboBox->blockSignals(wasBlocked);
  d->ReferenceViewNodeComboBox->setEnabled(vrViewNode != nullptr);

  d->UpdateViewFromReferenceViewCameraButton->setEnabled(vrViewNode != nullptr
      && vrViewNode->GetReferenceViewNode() != nullptr);
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::setStereoConnected(bool connect)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vrLogic->SetStereoConnected(connect);
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::setStereoActive(bool activate)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vrLogic->SetStereoActive(activate);
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::setTwoSidedLighting(bool active)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();
  if (vrViewNode)
  {
    vrViewNode->SetTwoSidedLighting(active);
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::setBackLights(bool active)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();
  if (vrViewNode)
  {
    vrViewNode->SetBackLights(active);
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::setReferenceViewNode(vtkMRMLNode* node)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();
  if (vrViewNode)
  {
    vtkMRMLViewNode* referenceViewNode = vtkMRMLViewNode::SafeDownCast(node);
    vrViewNode->SetAndObserveReferenceViewNode(referenceViewNode);
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::updateViewFromReferenceViewCamera()
{
  qSlicerVirtualRealityModule* vrModule = dynamic_cast<qSlicerVirtualRealityModule*>(this->module());
  if (!vrModule)
  {
    return;
  }
  qMRMLVirtualRealityView* vrView = vrModule->viewWidget();
  if (!vrView)
  {
    return;
  }
  vrView->updateViewFromReferenceViewCamera();
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::onDesiredUpdateRateChanged(double fps)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();
  if (vrViewNode)
  {
    vrViewNode->SetDesiredUpdateRate(fps);
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::onMotionSensitivityChanged(double percent)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();
  if (vrViewNode)
  {
    vrViewNode->SetMotionSensitivity(percent * 0.01);
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::onInteraxialDistanceChanged(double distance)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();
  if (vrViewNode)
  {
    vrViewNode->SetInteraxialDistance(distance);
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::onZeroParallaxDistanceChanged(double distance)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();
  if (vrViewNode)
  {
    vrViewNode->SetZeroParallaxDistance(distance);
  }
}

//----------------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::onMagnificationChanged(double powerOfTen)
{
  Q_D(qSlicerVirtualRealityModuleWidget);

  double magnification = this->getMagnificationFromPower(powerOfTen);

  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();
  if (vrViewNode)
  {
    vrViewNode->SetMagnification(magnification);
  }

  d->MagnificationValueLabel->setText(QString("%1x").arg(magnification, 3, 'f', 2));
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::setLeftEyeWindowVisible(bool visible)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();
  if (vrViewNode)
  {
    vrViewNode->SetLeftEyeWindowVisible(visible);
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualRealityModuleWidget::setRightEyeWindowVisible(bool visible)
{
  vtkSlicerVirtualRealityLogic* vrLogic = vtkSlicerVirtualRealityLogic::SafeDownCast(this->logic());
  vtkMRMLVirtualRealityViewNode* vrViewNode = vrLogic->GetVirtualRealityViewNode();
  if (vrViewNode)
  {
    vrViewNode->SetRightEyeWindowVisible(visible);
  }
}

//----------------------------------------------------------------------------------
double qSlicerVirtualRealityModuleWidget::getMagnificationFromPower(double powerOfTen)
{
  if (powerOfTen < -2.0)
  {
    powerOfTen = -2.0;
  }
  else if (powerOfTen > 2.0)
  {
    powerOfTen = 2.0;
  }

  return pow(10.0, powerOfTen);
}

//----------------------------------------------------------------------------------
double qSlicerVirtualRealityModuleWidget::getPowerFromMagnification(double magnification)
{
  if (magnification < 0.01)
  {
    magnification = 0.01;
  }
  else if (magnification > 100.0)
  {
    magnification = 100.0;
  }

  return log10(magnification);
}
