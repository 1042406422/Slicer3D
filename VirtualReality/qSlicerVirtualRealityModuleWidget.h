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

#ifndef __qSlicerVirtualRealityModuleWidget_h
#define __qSlicerVirtualRealityModuleWidget_h

#include <qSlicerAbstractModuleWidget.h>

#include "qSlicerVirtualRealityModuleExport.h"

class qSlicerVirtualRealityModuleWidgetPrivate;
class vtkMRMLNode;

class Q_SLICER_QTMODULES_VIRTUALREALITY_EXPORT qSlicerVirtualRealityModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerVirtualRealityModuleWidget(QWidget *parent=nullptr);
  ~qSlicerVirtualRealityModuleWidget() override;

public slots:
  void setStereoConnected(bool connect);
  void setStereoActive(bool activate);
  void setTwoSidedLighting(bool);
  void setBackLights(bool);
  void setReferenceViewNode(vtkMRMLNode*);
  void updateViewFromReferenceViewCamera();
  void onDesiredUpdateRateChanged(double);
  void onMotionSensitivityChanged(double);
  void onInteraxialDistanceChanged(double);
  void onZeroParallaxDistanceChanged(double);
  void onMagnificationChanged(double);
  void setLeftEyeWindowVisible(bool);
  void setRightEyeWindowVisible(bool);

protected slots:
  void updateWidgetFromMRML();

protected:
  QScopedPointer<qSlicerVirtualRealityModuleWidgetPrivate> d_ptr;

  void setup() override;

  double getMagnificationFromPower(double powerOfTen);
  double getPowerFromMagnification(double magnification);

private:
  Q_DECLARE_PRIVATE(qSlicerVirtualRealityModuleWidget);
  Q_DISABLE_COPY(qSlicerVirtualRealityModuleWidget);
};

#endif
