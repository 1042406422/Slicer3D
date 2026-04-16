/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qMRMLVirtualRealityView_p_h
#define __qMRMLVirtualRealityView_p_h

#include "vtkMRMLVirtualRealityViewNode.h"

class vtkMRMLDisplayableManagerGroup;
class vtkMRMLTransformNode;

#include "qMRMLVirtualRealityView.h"

#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
class vtkLightCollection;
class vtkObject;
class vtkTimerLog;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkCamera;
class vtkGenericOpenGLRenderWindow;

#include <ctkVTKObject.h>

#include <QObject>
#include <QString>
#include <QTimer>
#include <QMainWindow>

class QDockWidget;

class qMRMLVirtualRealityViewPrivate: public QObject
{
  Q_OBJECT
  QVTK_OBJECT
  Q_DECLARE_PUBLIC(qMRMLVirtualRealityView);
protected:
  qMRMLVirtualRealityView* const q_ptr;
public:
  qMRMLVirtualRealityViewPrivate(qMRMLVirtualRealityView& object);
  ~qMRMLVirtualRealityViewPrivate() override;

  virtual void init();

  double desiredUpdateRate();
  double stillUpdateRate();

public slots:
  void updateWidgetFromMRML();
  void doStereoRendering();

protected:
  void updateWidgetFromMRMLNoModify();

  void createRenderWindows();
  void destroyRenderWindows();

  void setupStereoCameras();
  void updateStereoCameras();

  vtkSlicerCamerasModuleLogic* CamerasLogic;
  vtkSmartPointer<vtkSlicerVirtualRealityLogic> VirtualRealityLogic;

  vtkSmartPointer<vtkMRMLDisplayableManagerGroup> DisplayableManagerGroup;

  vtkWeakPointer<vtkMRMLVirtualRealityViewNode> MRMLVirtualRealityViewNode;

  vtkSmartPointer<vtkRenderer> LeftEyeRenderer;
  vtkSmartPointer<vtkRenderer> RightEyeRenderer;
  vtkSmartPointer<vtkRenderWindow> LeftEyeRenderWindow;
  vtkSmartPointer<vtkRenderWindow> RightEyeRenderWindow;
  vtkSmartPointer<vtkRenderWindowInteractor> LeftEyeInteractor;
  vtkSmartPointer<vtkRenderWindowInteractor> RightEyeInteractor;
  vtkSmartPointer<vtkCamera> LeftEyeCamera;
  vtkSmartPointer<vtkCamera> RightEyeCamera;
  vtkSmartPointer<vtkCamera> CenterCamera;

  vtkSmartPointer<vtkLightCollection> Lights;

  vtkSmartPointer<vtkTimerLog> LastViewUpdateTime;
  double LastViewDirection[3];
  double LastViewUp[3];
  double LastViewPosition[3];

  bool IsUpdatingWidgetFromMRML{false};

  QTimer StereoRenderTimer;

  QMainWindow* LeftEyeWindow{nullptr};
  QMainWindow* RightEyeWindow{nullptr};
  QDockWidget* LeftEyeDock{nullptr};
  QDockWidget* RightEyeDock{nullptr};
};

#endif
