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

#ifndef __vtkSlicerVirtualRealityLogic_h
#define __vtkSlicerVirtualRealityLogic_h

#include "vtkSlicerVirtualRealityModuleLogicExport.h"
#include "vtkMRMLVirtualRealityViewNode.h"

#include <vtkSlicerModuleLogic.h>
class vtkSlicerVolumeRenderingLogic;

#include <cstdlib>


class VTK_SLICER_VIRTUALREALITY_MODULE_LOGIC_EXPORT vtkSlicerVirtualRealityLogic :
  public vtkSlicerModuleLogic
{
public:
  static vtkSlicerVirtualRealityLogic* New();
  vtkTypeMacro(vtkSlicerVirtualRealityLogic, vtkSlicerModuleLogic);
  typedef vtkSlicerVirtualRealityLogic Self;
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkMRMLVirtualRealityViewNode* AddVirtualRealityViewNode();
  vtkMRMLVirtualRealityViewNode* GetVirtualRealityViewNode();
  vtkMRMLVirtualRealityViewNode* GetDefaultVirtualRealityViewNode();

  void SetStereoConnected(bool connect);
  bool GetStereoConnected();

  void SetStereoActive(bool activate);
  bool GetStereoActive();

  void SetDefaultReferenceView();

  void OptimizeSceneForVirtualReality();

  void SetVolumeRenderingLogic(vtkSlicerVolumeRenderingLogic* volumeRenderingLogic);

  static bool ShouldConsiderQuickViewMotion(
      double motionSensitivity, double physicalScale, double elapsedTimeInSec,
      double lastViewPos[3], double lastViewDir[3], double lastViewUp[3],
      double viewPos[3], double viewDir[3], double viewUp[3]);

  void InitializeActiveViewNode();

protected:
  vtkSlicerVirtualRealityLogic();
  ~vtkSlicerVirtualRealityLogic() override;

  void SetActiveViewNode(vtkMRMLVirtualRealityViewNode* vrViewNode);

  void SetMRMLSceneInternal(vtkMRMLScene* newScene) override;
  void RegisterNodes() override;
  void UpdateFromMRMLScene() override;
  void OnMRMLSceneNodeAdded(vtkMRMLNode* node) override;
  void OnMRMLSceneNodeRemoved(vtkMRMLNode* node) override;
  void OnMRMLSceneEndImport() override;
  void ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData) override;

protected:
  vtkMRMLVirtualRealityViewNode* ActiveViewNode;
  vtkSlicerVolumeRenderingLogic* VolumeRenderingLogic;

  bool ModuleInstalled{false};

private:
  vtkSlicerVirtualRealityLogic(const vtkSlicerVirtualRealityLogic&);
  void operator=(const vtkSlicerVirtualRealityLogic&);
};

#endif
