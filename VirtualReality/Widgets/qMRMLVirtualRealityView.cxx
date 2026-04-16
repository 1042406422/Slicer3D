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

#include "vtkMRMLVirtualRealityConfigure.h"

#include <vtkSlicerVersionConfigureMinimal.h>

#include "vtkSlicerVirtualRealityLogic.h"
#include "vtkMRMLVirtualRealityViewNode.h"
#include "qMRMLVirtualRealityView_p.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QToolButton>
#include <QTimer>
#include <QSettings>
#include <QMainWindow>
#include <QDockWidget>
#include <QScreen>
#include <QGuiApplication>

#include <ctkPimpl.h>

#include <qSlicerApplication.h>
#include <vtkSlicerCamerasModuleLogic.h>

#include <vtkMRMLAbstractDisplayableManager.h>
#include <vtkMRMLDisplayableManagerGroup.h>
#include <vtkMRMLVirtualRealityViewDisplayableManagerFactory.h>

#include <vtkMRMLCameraNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLScene.h>

#include <vtkCamera.h>
#include <vtkCollection.h>
#include <vtkCullerCollection.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkNew.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkTimerLog.h>
#include <vtkTransform.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>

#include <vtkVersion.h>

#if VTK_MAJOR_VERSION >= 9
#include <vtkGenericOpenGLRenderWindow.h>
#endif

namespace
{
}

//--------------------------------------------------------------------------
// qMRMLVirtualRealityViewPrivate methods

//---------------------------------------------------------------------------
qMRMLVirtualRealityViewPrivate::qMRMLVirtualRealityViewPrivate(qMRMLVirtualRealityView& object)
  : q_ptr(&object)
  , CamerasLogic(nullptr)
{
  this->MRMLVirtualRealityViewNode = nullptr;
}

//---------------------------------------------------------------------------
qMRMLVirtualRealityViewPrivate::~qMRMLVirtualRealityViewPrivate()
{
}

//---------------------------------------------------------------------------
void qMRMLVirtualRealityViewPrivate::init()
{
  QObject::connect(&this->StereoRenderTimer, SIGNAL(timeout()), this, SLOT(doStereoRendering()));
}

//----------------------------------------------------------------------------
CTK_SET_CPP(qMRMLVirtualRealityView, vtkSlicerCamerasModuleLogic*, setCamerasLogic, CamerasLogic);
CTK_GET_CPP(qMRMLVirtualRealityView, vtkSlicerCamerasModuleLogic*, camerasLogic, CamerasLogic);

//----------------------------------------------------------------------------
CTK_SET_CPP(qMRMLVirtualRealityView, vtkSlicerVirtualRealityLogic*, setVirtualRealityLogic, VirtualRealityLogic);
CTK_GET_CPP(qMRMLVirtualRealityView, vtkSlicerVirtualRealityLogic*, virtualRealityLogic, VirtualRealityLogic);

//---------------------------------------------------------------------------
void qMRMLVirtualRealityViewPrivate::createRenderWindows()
{
  Q_Q(qMRMLVirtualRealityView);

  if (this->MRMLVirtualRealityViewNode == nullptr)
  {
    qCritical() << Q_FUNC_INFO << " failed: MRMLVirtualRealityViewNode is not set";
    return;
  }
  if (this->VirtualRealityLogic == nullptr)
  {
    qCritical() << Q_FUNC_INFO << " failed: VirtualRealityLogic is not set";
    return;
  }
  vtkSlicerApplicationLogic* appLogic = qSlicerApplication::application()->applicationLogic();
  if (!appLogic)
  {
    qCritical() << Q_FUNC_INFO << " failed: Application logic not available";
    return;
  }

  this->LastViewUpdateTime = vtkSmartPointer<vtkTimerLog>::New();
  this->LastViewUpdateTime->StartTimer();
  this->LastViewUpdateTime->StopTimer();
  this->LastViewDirection[0] = 0.0;
  this->LastViewDirection[1] = 0.0;
  this->LastViewDirection[2] = 1.0;
  this->LastViewUp[0] = 0.0;
  this->LastViewUp[1] = 1.0;
  this->LastViewUp[2] = 0.0;
  this->LastViewPosition[0] = 0.0;
  this->LastViewPosition[1] = 0.0;
  this->LastViewPosition[2] = 0.0;

  this->LeftEyeRenderer = vtkSmartPointer<vtkRenderer>::New();
  this->RightEyeRenderer = vtkSmartPointer<vtkRenderer>::New();

  this->LeftEyeCamera = vtkSmartPointer<vtkCamera>::New();
  this->RightEyeCamera = vtkSmartPointer<vtkCamera>::New();
  this->CenterCamera = vtkSmartPointer<vtkCamera>::New();

  this->LeftEyeRenderer->SetActiveCamera(this->LeftEyeCamera);
  this->RightEyeRenderer->SetActiveCamera(this->RightEyeCamera);

  this->LeftEyeRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  this->RightEyeRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

  this->LeftEyeRenderWindow->AddRenderer(this->LeftEyeRenderer);
  this->RightEyeRenderWindow->AddRenderer(this->RightEyeRenderer);

  this->LeftEyeRenderWindow->SetMultiSamples(0);
  this->RightEyeRenderWindow->SetMultiSamples(0);

  this->LeftEyeRenderWindow->SetWindowName("Left Eye View");
  this->RightEyeRenderWindow->SetWindowName("Right Eye View");

  vtkNew<vtkInteractorStyleTrackballCamera> leftStyle;
  vtkNew<vtkInteractorStyleTrackballCamera> rightStyle;

  this->LeftEyeInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  this->RightEyeInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

  this->LeftEyeInteractor->SetInteractorStyle(leftStyle);
  this->RightEyeInteractor->SetInteractorStyle(rightStyle);

  this->LeftEyeRenderWindow->SetInteractor(this->LeftEyeInteractor);
  this->RightEyeRenderWindow->SetInteractor(this->RightEyeInteractor);

  vtkMRMLVirtualRealityViewDisplayableManagerFactory* factory
    = vtkMRMLVirtualRealityViewDisplayableManagerFactory::GetInstance();
  factory->SetMRMLApplicationLogic(appLogic);

  QStringList displayableManagers;
  displayableManagers
      << "vtkMRMLModelDisplayableManager"
      << "vtkMRMLThreeDReformatDisplayableManager"
      << "vtkMRMLCrosshairDisplayableManager3D"
      << "vtkMRMLMarkupsDisplayableManager"
      << "vtkMRMLSegmentationsDisplayableManager3D"
#if ((Slicer_VERSION_MAJOR == 5 && Slicer_VERSION_MINOR >= 7) || (Slicer_VERSION_MAJOR > 5))
      << "vtkMRMLLinearTransformsDisplayableManager"
#else
      << "vtkMRMLLinearTransformsDisplayableManager3D"
#endif
      << "vtkMRMLVolumeRenderingDisplayableManager"
      ;

  foreach (const QString& displayableManager, displayableManagers)
  {
    if (!factory->IsDisplayableManagerRegistered(displayableManager.toLatin1()))
    {
      factory->RegisterDisplayableManager(displayableManager.toLatin1());
    }
  }

  this->DisplayableManagerGroup = vtkSmartPointer<vtkMRMLDisplayableManagerGroup>::Take(
                                    factory->InstantiateDisplayableManagers(this->LeftEyeRenderer));
  this->DisplayableManagerGroup->SetMRMLDisplayableNode(this->MRMLVirtualRealityViewNode);

  this->LeftEyeRenderer->RemoveCuller(this->LeftEyeRenderer->GetCullers()->GetLastItem());
  this->RightEyeRenderer->RemoveCuller(this->RightEyeRenderer->GetCullers()->GetLastItem());

  this->LeftEyeRenderer->SetBackground(0.0, 0.0, 0.0);
  this->RightEyeRenderer->SetBackground(0.0, 0.0, 0.0);

  this->Lights = vtkSmartPointer<vtkLightCollection>::New();
  {
    vtkNew<vtkLight> light;
    light->SetPosition(0.0, 1.0, 0.0);
    light->SetIntensity(1.0);
    light->SetLightTypeToSceneLight();
    this->Lights->AddItem(light);
  }
  {
    vtkNew<vtkLight> light;
    light->SetPosition(0.8, -0.2, 0.0);
    light->SetIntensity(0.8);
    light->SetLightTypeToSceneLight();
    this->Lights->AddItem(light);
  }
  {
    vtkNew<vtkLight> light;
    light->SetPosition(-0.3, -0.2, 0.7);
    light->SetIntensity(0.6);
    light->SetLightTypeToSceneLight();
    this->Lights->AddItem(light);
  }
  {
    vtkNew<vtkLight> light;
    light->SetPosition(-0.3, -0.2, -0.7);
    light->SetIntensity(0.4);
    light->SetLightTypeToSceneLight();
    this->Lights->AddItem(light);
  }
  this->LeftEyeRenderer->SetLightCollection(this->Lights);
  this->RightEyeRenderer->SetLightCollection(this->Lights);

  q->updateViewFromReferenceViewCamera();

  this->LeftEyeInteractor->Initialize();
  this->RightEyeInteractor->Initialize();

  qDebug() << "";
  qDebug() << "Dual projection stereo view initialized";
  qDebug() << "";
  qDebug() << "Number of registered displayable manager:" << this->DisplayableManagerGroup->GetDisplayableManagerCount();
  qDebug() << "Registered displayable managers:";
  for (int idx=0; idx < this->DisplayableManagerGroup->GetDisplayableManagerCount(); idx++)
  {
    qDebug() << " " << this->DisplayableManagerGroup->GetNthDisplayableManager(idx)->GetClassName();
  }
}

//---------------------------------------------------------------------------
void qMRMLVirtualRealityViewPrivate::destroyRenderWindows()
{
  this->StereoRenderTimer.stop();

  if (this->LeftEyeInteractor != nullptr)
  {
    this->LeftEyeInteractor->SetRenderWindow(nullptr);
  }
  if (this->RightEyeInteractor != nullptr)
  {
    this->RightEyeInteractor->SetRenderWindow(nullptr);
  }

  if (this->LeftEyeWindow)
  {
    this->LeftEyeWindow->close();
    delete this->LeftEyeWindow;
    this->LeftEyeWindow = nullptr;
  }
  if (this->RightEyeWindow)
  {
    this->RightEyeWindow->close();
    delete this->RightEyeWindow;
    this->RightEyeWindow = nullptr;
  }

  this->LeftEyeInteractor = nullptr;
  this->RightEyeInteractor = nullptr;
  this->DisplayableManagerGroup = nullptr;
  this->LeftEyeRenderer = nullptr;
  this->RightEyeRenderer = nullptr;
  this->LeftEyeCamera = nullptr;
  this->RightEyeCamera = nullptr;
  this->CenterCamera = nullptr;
  this->Lights = nullptr;
  this->LeftEyeRenderWindow = nullptr;
  this->RightEyeRenderWindow = nullptr;
}

//---------------------------------------------------------------------------
void qMRMLVirtualRealityViewPrivate::setupStereoCameras()
{
  if (!this->CenterCamera || !this->LeftEyeCamera || !this->RightEyeCamera)
  {
    return;
  }

  double interaxial = this->MRMLVirtualRealityViewNode->GetInteraxialDistance();
  double halfInteraxial = interaxial * 0.5;

  double position[3];
  double focalPoint[3];
  double viewUp[3];

  this->CenterCamera->GetPosition(position);
  this->CenterCamera->GetFocalPoint(focalPoint);
  this->CenterCamera->GetViewUp(viewUp);

  double viewDir[3];
  viewDir[0] = focalPoint[0] - position[0];
  viewDir[1] = focalPoint[1] - position[1];
  viewDir[2] = focalPoint[2] - position[2];
  vtkMath::Normalize(viewDir);

  double right[3];
  vtkMath::Cross(viewDir, viewUp, right);
  vtkMath::Normalize(right);

  this->LeftEyeCamera->SetPosition(
    position[0] - halfInteraxial * right[0],
    position[1] - halfInteraxial * right[1],
    position[2] - halfInteraxial * right[2]);

  this->LeftEyeCamera->SetFocalPoint(focalPoint);
  this->LeftEyeCamera->SetViewUp(viewUp);

  this->RightEyeCamera->SetPosition(
    position[0] + halfInteraxial * right[0],
    position[1] + halfInteraxial * right[1],
    position[2] + halfInteraxial * right[2]);

  this->RightEyeCamera->SetFocalPoint(focalPoint);
  this->RightEyeCamera->SetViewUp(viewUp);

  double zeroParallaxDistance = this->MRMLVirtualRealityViewNode->GetZeroParallaxDistance();

  double leftFocalPoint[3];
  leftFocalPoint[0] = focalPoint[0] - halfInteraxial * right[0] * (zeroParallaxDistance / vtkMath::Norm(viewDir));
  leftFocalPoint[1] = focalPoint[1] - halfInteraxial * right[1] * (zeroParallaxDistance / vtkMath::Norm(viewDir));
  leftFocalPoint[2] = focalPoint[2] - halfInteraxial * right[2] * (zeroParallaxDistance / vtkMath::Norm(viewDir));

  double rightFocalPoint[3];
  rightFocalPoint[0] = focalPoint[0] + halfInteraxial * right[0] * (zeroParallaxDistance / vtkMath::Norm(viewDir));
  rightFocalPoint[1] = focalPoint[1] + halfInteraxial * right[1] * (zeroParallaxDistance / vtkMath::Norm(viewDir));
  rightFocalPoint[2] = focalPoint[2] + halfInteraxial * right[2] * (zeroParallaxDistance / vtkMath::Norm(viewDir));

  this->LeftEyeCamera->SetClippingRange(0.1, 100000.0);
  this->RightEyeCamera->SetClippingRange(0.1, 100000.0);
}

//---------------------------------------------------------------------------
void qMRMLVirtualRealityViewPrivate::updateStereoCameras()
{
  this->setupStereoCameras();
}

// --------------------------------------------------------------------------
void qMRMLVirtualRealityViewPrivate::updateWidgetFromMRML()
{
  if (this->IsUpdatingWidgetFromMRML)
    {
    return;
    }
  this->IsUpdatingWidgetFromMRML = true;

  this->updateWidgetFromMRMLNoModify();

  this->IsUpdatingWidgetFromMRML = false;
}

// --------------------------------------------------------------------------
void qMRMLVirtualRealityViewPrivate::updateWidgetFromMRMLNoModify()
{
  if (!this->MRMLVirtualRealityViewNode || !this->MRMLVirtualRealityViewNode->GetVisibility())
  {
    this->destroyRenderWindows();
    if (this->MRMLVirtualRealityViewNode)
    {
      this->MRMLVirtualRealityViewNode->ClearError();
    }
    return;
  }

  if (!this->LeftEyeRenderWindow)
  {
    this->createRenderWindows();
  }

  if (!this->LeftEyeRenderWindow || !this->RightEyeRenderWindow)
  {
    return;
  }

  if (this->DisplayableManagerGroup->GetMRMLDisplayableNode() != this->MRMLVirtualRealityViewNode.GetPointer())
  {
    this->DisplayableManagerGroup->SetMRMLDisplayableNode(this->MRMLVirtualRealityViewNode);
  }

  if (this->MRMLVirtualRealityViewNode->GetLeftEyeWindowVisible())
  {
    this->LeftEyeRenderer->SetGradientBackground(1);
    this->LeftEyeRenderer->SetBackground(this->MRMLVirtualRealityViewNode->GetBackgroundColor());
    this->LeftEyeRenderer->SetBackground2(this->MRMLVirtualRealityViewNode->GetBackgroundColor2());
  }

  if (this->MRMLVirtualRealityViewNode->GetRightEyeWindowVisible())
  {
    this->RightEyeRenderer->SetGradientBackground(1);
    this->RightEyeRenderer->SetBackground(this->MRMLVirtualRealityViewNode->GetBackgroundColor());
    this->RightEyeRenderer->SetBackground2(this->MRMLVirtualRealityViewNode->GetBackgroundColor2());
  }

  this->LeftEyeRenderer->SetTwoSidedLighting(this->MRMLVirtualRealityViewNode->GetTwoSidedLighting());
  this->RightEyeRenderer->SetTwoSidedLighting(this->MRMLVirtualRealityViewNode->GetTwoSidedLighting());

  bool switchOnAllLights = this->MRMLVirtualRealityViewNode->GetBackLights();
  for (int i = 2; i < this->Lights->GetNumberOfItems(); i++)
  {
    vtkLight* light = vtkLight::SafeDownCast(this->Lights->GetItemAsObject(i));
    light->SetSwitch(switchOnAllLights);
  }

  this->LeftEyeRenderer->SetUseDepthPeeling(this->MRMLVirtualRealityViewNode->GetUseDepthPeeling() != 0);
  this->LeftEyeRenderer->SetUseDepthPeelingForVolumes(this->MRMLVirtualRealityViewNode->GetUseDepthPeeling() != 0);
  this->RightEyeRenderer->SetUseDepthPeeling(this->MRMLVirtualRealityViewNode->GetUseDepthPeeling() != 0);
  this->RightEyeRenderer->SetUseDepthPeelingForVolumes(this->MRMLVirtualRealityViewNode->GetUseDepthPeeling() != 0);

  this->updateStereoCameras();

  if (this->MRMLVirtualRealityViewNode->GetActive())
  {
    this->StereoRenderTimer.start(0);
  }
  else
  {
    this->StereoRenderTimer.stop();
  }
}

//---------------------------------------------------------------------------
double qMRMLVirtualRealityViewPrivate::desiredUpdateRate()
{
  double rate = this->MRMLVirtualRealityViewNode->GetDesiredUpdateRate();

  const double defaultStaticViewUpdateRate = 0.0001;
  if (rate < defaultStaticViewUpdateRate)
  {
    rate = defaultStaticViewUpdateRate;
  }

  return rate;
}

//---------------------------------------------------------------------------
double qMRMLVirtualRealityViewPrivate::stillUpdateRate()
{
  return 0.0001;
}

// --------------------------------------------------------------------------
void qMRMLVirtualRealityViewPrivate::doStereoRendering()
{
  if (this->LeftEyeRenderer == nullptr || this->RightEyeRenderer == nullptr)
  {
    return;
  }
  if (this->LeftEyeRenderWindow == nullptr || this->RightEyeRenderWindow == nullptr)
  {
    return;
  }

  this->updateStereoCameras();

  if (this->MRMLVirtualRealityViewNode->GetLeftEyeWindowVisible())
  {
    this->LeftEyeRenderWindow->Render();
  }

  if (this->MRMLVirtualRealityViewNode->GetRightEyeWindowVisible())
  {
    this->RightEyeRenderWindow->Render();
  }

  this->LastViewUpdateTime->StopTimer();
  if (this->LastViewUpdateTime->GetElapsedTime() > 0.0)
  {
    this->CenterCamera->GetViewPlaneNormal(this->LastViewDirection);
    this->CenterCamera->GetViewUp(this->LastViewUp);
    this->CenterCamera->GetPosition(this->LastViewPosition);

    this->LastViewUpdateTime->StartTimer();
  }
}


// --------------------------------------------------------------------------
// qMRMLVirtualRealityView methods

// --------------------------------------------------------------------------
qMRMLVirtualRealityView::qMRMLVirtualRealityView(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new qMRMLVirtualRealityViewPrivate(*this))
{
  Q_D(qMRMLVirtualRealityView);
  d->init();
}

// --------------------------------------------------------------------------
qMRMLVirtualRealityView::~qMRMLVirtualRealityView()
{
}

//------------------------------------------------------------------------------
void qMRMLVirtualRealityView::addDisplayableManager(const QString& displayableManagerName)
{
  Q_D(qMRMLVirtualRealityView);
  vtkSmartPointer<vtkMRMLAbstractDisplayableManager> displayableManager;
  displayableManager.TakeReference(
    vtkMRMLDisplayableManagerGroup::InstantiateDisplayableManager(
      displayableManagerName.toLatin1()));
  d->DisplayableManagerGroup->AddDisplayableManager(displayableManager);
}

//------------------------------------------------------------------------------
void qMRMLVirtualRealityView::setMRMLVirtualRealityViewNode(vtkMRMLVirtualRealityViewNode* newViewNode)
{
  Q_D(qMRMLVirtualRealityView);
  if (d->MRMLVirtualRealityViewNode == newViewNode)
  {
    return;
  }

  d->qvtkReconnect(
    d->MRMLVirtualRealityViewNode, newViewNode,
    vtkCommand::ModifiedEvent, d, SLOT(updateWidgetFromMRML()));

  d->MRMLVirtualRealityViewNode = newViewNode;

  d->updateWidgetFromMRML();

  this->setEnabled(newViewNode != nullptr);
}

//---------------------------------------------------------------------------
vtkMRMLVirtualRealityViewNode* qMRMLVirtualRealityView::mrmlVirtualRealityViewNode()const
{
  Q_D(const qMRMLVirtualRealityView);
  return d->MRMLVirtualRealityViewNode;
}

//------------------------------------------------------------------------------
void qMRMLVirtualRealityView::getDisplayableManagers(vtkCollection* displayableManagers)
{
  Q_D(qMRMLVirtualRealityView);

  if (!displayableManagers || !d->DisplayableManagerGroup)
  {
    return;
  }
  int num = d->DisplayableManagerGroup->GetDisplayableManagerCount();
  for (int n = 0; n < num; n++)
  {
    displayableManagers->AddItem(d->DisplayableManagerGroup->GetNthDisplayableManager(n));
  }
}

//------------------------------------------------------------------------------
vtkRenderer* qMRMLVirtualRealityView::leftEyeRenderer()const
{
  Q_D(const qMRMLVirtualRealityView);
  return d->LeftEyeRenderer;
}

//------------------------------------------------------------------------------
vtkRenderer* qMRMLVirtualRealityView::rightEyeRenderer()const
{
  Q_D(const qMRMLVirtualRealityView);
  return d->RightEyeRenderer;
}

//------------------------------------------------------------------------------
vtkRenderWindow* qMRMLVirtualRealityView::leftEyeRenderWindow()const
{
  Q_D(const qMRMLVirtualRealityView);
  return d->LeftEyeRenderWindow;
}

//------------------------------------------------------------------------------
vtkRenderWindow* qMRMLVirtualRealityView::rightEyeRenderWindow()const
{
  Q_D(const qMRMLVirtualRealityView);
  return d->RightEyeRenderWindow;
}

//------------------------------------------------------------------------------
vtkCamera* qMRMLVirtualRealityView::leftEyeCamera()const
{
  Q_D(const qMRMLVirtualRealityView);
  return d->LeftEyeCamera;
}

//------------------------------------------------------------------------------
vtkCamera* qMRMLVirtualRealityView::rightEyeCamera()const
{
  Q_D(const qMRMLVirtualRealityView);
  return d->RightEyeCamera;
}

//------------------------------------------------------------------------------
vtkCamera* qMRMLVirtualRealityView::centerCamera()const
{
  Q_D(const qMRMLVirtualRealityView);
  return d->CenterCamera;
}

//------------------------------------------------------------------------------
void qMRMLVirtualRealityView::updateViewFromReferenceViewCamera()
{
  Q_D(qMRMLVirtualRealityView);
  if (!d->MRMLVirtualRealityViewNode)
  {
    return;
  }
  vtkMRMLViewNode* referenceViewNode = d->MRMLVirtualRealityViewNode->GetReferenceViewNode();
  if (!referenceViewNode)
  {
    qWarning() << Q_FUNC_INFO << " failed: no reference view node is set";
    return;
  }
  if (!d->CamerasLogic)
  {
    qWarning() << Q_FUNC_INFO << " failed: cameras module logic is not set";
    return;
  }
  vtkMRMLCameraNode* cameraNode = d->CamerasLogic->GetViewActiveCameraNode(referenceViewNode);
  if (!cameraNode || !cameraNode->GetCamera())
  {
    qWarning() << Q_FUNC_INFO << " failed: camera node is not found";
    return;
  }
  if (!d->CenterCamera)
  {
    qWarning() << Q_FUNC_INFO << " failed: CenterCamera has not been created";
    return;
  }

  vtkCamera* sourceCamera = cameraNode->GetCamera();

  double* sourcePosition = sourceCamera->GetPosition();
  double* sourceFocalPoint = sourceCamera->GetFocalPoint();
  double* sourceViewUp = sourceCamera->GetViewUp();

  d->CenterCamera->SetPosition(sourcePosition);
  d->CenterCamera->SetFocalPoint(sourceFocalPoint);
  d->CenterCamera->SetViewUp(sourceViewUp);
  d->CenterCamera->SetClippingRange(sourceCamera->GetClippingRange());

  d->updateStereoCameras();

  if (d->LeftEyeRenderer)
  {
    d->LeftEyeRenderer->ResetCameraClippingRange();
  }
  if (d->RightEyeRenderer)
  {
    d->RightEyeRenderer->ResetCameraClippingRange();
  }
}
