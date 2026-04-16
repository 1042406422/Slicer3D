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

#ifndef __qMRMLVirtualRealityView_h
#define __qMRMLVirtualRealityView_h

class vtkSlicerVirtualRealityLogic;
class vtkMRMLVirtualRealityViewNode;

#include "qSlicerVirtualRealityModuleWidgetsExport.h"
class qMRMLVirtualRealityViewPrivate;

#include <QString>
#include <QWidget>

#include <ctkVTKObject.h>

class vtkSlicerCamerasModuleLogic;
class vtkRenderer;
class vtkRenderWindow;
class vtkCamera;
class vtkCollection;

class Q_SLICER_QTMODULES_VIRTUALREALITY_WIDGETS_EXPORT qMRMLVirtualRealityView : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:
  typedef QWidget Superclass;

  explicit qMRMLVirtualRealityView(QWidget* parent = nullptr);
  ~qMRMLVirtualRealityView() override;

  void addDisplayableManager(const QString& displayableManager);
  Q_INVOKABLE void getDisplayableManagers(vtkCollection *displayableManagers);

  void setCamerasLogic(vtkSlicerCamerasModuleLogic* camerasLogic);
  vtkSlicerCamerasModuleLogic* camerasLogic()const;

  void setVirtualRealityLogic(vtkSlicerVirtualRealityLogic* camerasLogic);
  vtkSlicerVirtualRealityLogic* virtualRealityLogic()const;

  Q_INVOKABLE vtkMRMLVirtualRealityViewNode* mrmlVirtualRealityViewNode()const;

  vtkRenderer* leftEyeRenderer()const;
  vtkRenderer* rightEyeRenderer()const;
  vtkRenderWindow* leftEyeRenderWindow()const;
  vtkRenderWindow* rightEyeRenderWindow()const;
  vtkCamera* leftEyeCamera()const;
  vtkCamera* rightEyeCamera()const;
  vtkCamera* centerCamera()const;

  Q_INVOKABLE void updateViewFromReferenceViewCamera();

signals:
  void stereoViewModified();

public slots:
  void setMRMLVirtualRealityViewNode(vtkMRMLVirtualRealityViewNode* newViewNode);

protected:

  QScopedPointer<qMRMLVirtualRealityViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLVirtualRealityView);
  Q_DISABLE_COPY(qMRMLVirtualRealityView);
};

#endif
