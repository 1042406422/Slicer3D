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

#include "vtkSlicerVirtualRealityLogic.h"
#include "vtkMRMLVirtualRealityViewNode.h"

#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSegmentationDisplayNode.h>

#include <vtkSlicerVolumeRenderingLogic.h>

#include <vtkIntArray.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerVirtualRealityLogic);
vtkCxxSetObjectMacro(vtkSlicerVirtualRealityLogic, VolumeRenderingLogic, vtkSlicerVolumeRenderingLogic);

//----------------------------------------------------------------------------
vtkSlicerVirtualRealityLogic::vtkSlicerVirtualRealityLogic()
  : ActiveViewNode(nullptr)
  , VolumeRenderingLogic(nullptr)
{
}

//----------------------------------------------------------------------------
vtkSlicerVirtualRealityLogic::~vtkSlicerVirtualRealityLogic()
{
  this->SetActiveViewNode(nullptr);
  this->SetVolumeRenderingLogic(nullptr);
}

//----------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::SetMRMLSceneInternal(vtkMRMLScene* newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  events->InsertNextValue(vtkMRMLScene::EndImportEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
  this->GetMRMLScene()->RegisterNodeClass((vtkSmartPointer<vtkMRMLVirtualRealityViewNode>::New()));
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::UpdateFromMRMLScene()
{
  vtkMRMLVirtualRealityViewNode* vrViewNode = nullptr;
  if (this->GetMRMLScene())
  {
    vrViewNode = vtkMRMLVirtualRealityViewNode::SafeDownCast(
                   this->GetMRMLScene()->GetSingletonNode("Active", "vtkMRMLVirtualRealityViewNode"));
  }
  this->SetActiveViewNode(vrViewNode);
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  vtkMRMLVirtualRealityViewNode* vrViewNode = vtkMRMLVirtualRealityViewNode::SafeDownCast(node);
  if (!vrViewNode)
  {
    return;
  }
  if (vrViewNode->GetSingletonTag() &&
      strcmp(vrViewNode->GetSingletonTag(), "Active") == 0)
  {
    this->SetActiveViewNode(vrViewNode);
  }
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
  vtkMRMLVirtualRealityViewNode* deletedVrViewNode = vtkMRMLVirtualRealityViewNode::SafeDownCast(node);
  if (!deletedVrViewNode)
  {
    return;
  }
  if (deletedVrViewNode == this->ActiveViewNode)
  {
    this->SetActiveViewNode(nullptr);
  }
}

//----------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::OnMRMLSceneEndImport()
{
  if (this->ActiveViewNode != nullptr && this->ActiveViewNode->GetActive())
  {
    this->ActiveViewNode->SetActive(0);
    this->ActiveViewNode->SetVisibility(0);
  }

  this->Modified();
}

//----------------------------------------------------------------------------
vtkMRMLVirtualRealityViewNode* vtkSlicerVirtualRealityLogic::GetVirtualRealityViewNode()
{
  return this->ActiveViewNode;
}

//---------------------------------------------------------------------------
vtkMRMLVirtualRealityViewNode* vtkSlicerVirtualRealityLogic::AddVirtualRealityViewNode()
{
  vtkMRMLScene* scene = this->GetMRMLScene();
  if (!scene)
  {
    vtkErrorMacro("AddVirtualRealityViewNode: Invalid MRML scene");
    return nullptr;
  }

  if (this->ActiveViewNode)
  {
    return this->ActiveViewNode;
  }

  vtkSmartPointer<vtkMRMLVirtualRealityViewNode> vrViewNode = vtkSmartPointer<vtkMRMLVirtualRealityViewNode>::Take(
        vtkMRMLVirtualRealityViewNode::SafeDownCast(scene->CreateNodeByClass("vtkMRMLVirtualRealityViewNode")));
  vrViewNode->SetSingletonTag("Active");
  vrViewNode = vtkMRMLVirtualRealityViewNode::SafeDownCast(scene->AddNode(vrViewNode));

  return vrViewNode;
}

//----------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::SetActiveViewNode(vtkMRMLVirtualRealityViewNode* vrViewNode)
{
  if (this->ActiveViewNode == vrViewNode)
  {
    return;
  }

  if (!this->GetMRMLScene())
  {
    return;
  }

  this->GetMRMLNodesObserverManager()->SetAndObserveObject(vtkObjectPointer(&this->ActiveViewNode), vrViewNode);

  this->Modified();
}

//-----------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* vtkNotUsed(callData))
{
  if (caller == this->ActiveViewNode && event == vtkCommand::ModifiedEvent)
  {
    this->Modified();
  }
}

//-----------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::SetStereoConnected(bool connect)
{
  if (!this->ActiveViewNode)
  {
    vtkErrorMacro("SetStereoConnected: Invalid ActiveViewNode");
    return;
  }
  this->ActiveViewNode->SetVisibility(connect ? 1 : 0);
}

//-----------------------------------------------------------------------------
bool vtkSlicerVirtualRealityLogic::GetStereoConnected()
{
  if (!this->ActiveViewNode)
  {
    return false;
  }
  return (this->ActiveViewNode->GetVisibility() != 0);
}

//-----------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::SetStereoActive(bool activate)
{
  if (activate)
  {
    this->SetStereoConnected(true);
    if (this->ActiveViewNode)
    {
      this->ActiveViewNode->SetActive(1);
    }
    else
    {
      vtkErrorMacro("vtkSlicerVirtualRealityLogic::SetStereoActive failed: view node is not available");
    }
  }
  else
  {
    if (this->ActiveViewNode != nullptr)
    {
      this->ActiveViewNode->SetActive(0);
    }
  }
}

//-----------------------------------------------------------------------------
bool vtkSlicerVirtualRealityLogic::GetStereoActive()
{
  if (!this->ActiveViewNode)
  {
    return false;
  }
  return (this->ActiveViewNode->GetVisibility() != 0 && this->ActiveViewNode->GetActive() != 0);
}

//-----------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::InitializeActiveViewNode()
{
  vtkMRMLScene* scene = this->GetMRMLScene();
  if (!scene)
  {
    vtkErrorMacro("InitializeActiveViewNode: Invalid MRML scene");
    return;
  }

  if (!this->ActiveViewNode)
  {
    if (scene->GetNumberOfNodesByClass("vtkMRMLVirtualRealityViewNode") > 0)
    {
      this->SetActiveViewNode(
        vtkMRMLVirtualRealityViewNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLVirtualRealityViewNode")));
    }
    else
    {
      vtkMRMLVirtualRealityViewNode* newViewNode = this->AddVirtualRealityViewNode();
      this->SetActiveViewNode(newViewNode);
    }
  }
  if (!this->ActiveViewNode)
  {
    vtkErrorMacro("Failed to create virtual reality view node");
  }
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::SetDefaultReferenceView()
{
  if (!this->ActiveViewNode)
  {
    return;
  }
  if (this->ActiveViewNode->GetReferenceViewNode() != nullptr)
  {
    return;
  }
  if (!this->GetMRMLScene())
  {
    return;
  }
  vtkSmartPointer<vtkCollection> nodes = vtkSmartPointer<vtkCollection>::Take(
      this->GetMRMLScene()->GetNodesByClass("vtkMRMLViewNode"));
  vtkMRMLViewNode* viewNode = nullptr;
  vtkCollectionSimpleIterator it;
  for (nodes->InitTraversal(it); (viewNode = vtkMRMLViewNode::SafeDownCast(
                                    nodes->GetNextItemAsObject(it)));)
  {
    if (viewNode->GetVisibility() && viewNode->IsMappedInLayout())
    {
      break;
    }
  }
  this->ActiveViewNode->SetAndObserveReferenceViewNode(viewNode);
}

//-----------------------------------------------------------------------------
vtkMRMLVirtualRealityViewNode* vtkSlicerVirtualRealityLogic::GetDefaultVirtualRealityViewNode()
{
  vtkMRMLScene* scene = this->GetMRMLScene();
  if (!scene)
  {
    vtkErrorMacro("GetDefaultVirtualRealityViewNode failed: invalid scene");
    return nullptr;
  }
  vtkMRMLNode* defaultNode = scene->GetDefaultNodeByClass("vtkMRMLVirtualRealityViewNode");
  if (!defaultNode)
  {
    defaultNode = scene->CreateNodeByClass("vtkMRMLVirtualRealityViewNode");
    scene->AddDefaultNode(defaultNode);
    defaultNode->Delete();
  }
  return vtkMRMLVirtualRealityViewNode::SafeDownCast(defaultNode);
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualRealityLogic::OptimizeSceneForVirtualReality()
{
  vtkMRMLScene* scene = this->GetMRMLScene();
  if (!scene)
  {
    vtkErrorMacro("OptimizeSceneForVirtualReality failed: Invalid scene");
    return;
  }

  if (this->VolumeRenderingLogic)
  {
    this->VolumeRenderingLogic->ChangeVolumeRenderingMethod("vtkMRMLGPURayCastVolumeRenderingDisplayNode");
  }
  else
  {
    vtkErrorMacro("OptimizeSceneForVirtualReality: Unable to access volume rendering logic");
  }

  std::vector<vtkMRMLNode*> modelDisplayNodes;
  scene->GetNodesByClass("vtkMRMLModelDisplayNode", modelDisplayNodes);
  for (std::vector<vtkMRMLNode*>::iterator mdIt = modelDisplayNodes.begin();
       mdIt != modelDisplayNodes.end(); ++mdIt)
  {
    vtkMRMLModelDisplayNode* modelDisplayNode = vtkMRMLModelDisplayNode::SafeDownCast(*mdIt);
    modelDisplayNode->SetBackfaceCulling(0);
    modelDisplayNode->SetVisibility2D(0);
  }

  std::vector<vtkMRMLNode*> segmentationDisplayNodes;
  scene->GetNodesByClass("vtkMRMLSegmentationDisplayNode", segmentationDisplayNodes);
  for (std::vector<vtkMRMLNode*>::iterator sdIt = segmentationDisplayNodes.begin();
       sdIt != segmentationDisplayNodes.end(); ++sdIt)
  {
    vtkMRMLSegmentationDisplayNode* segmentationDisplayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(*sdIt);
    segmentationDisplayNode->SetVisibility2DFill(0);
    segmentationDisplayNode->SetVisibility2DOutline(0);
  }

  vtkMRMLNode* defaultModelDisplayNode = scene->GetDefaultNodeByClass("vtkMRMLModelDisplayNode");
  if (!defaultModelDisplayNode)
  {
    defaultModelDisplayNode = scene->CreateNodeByClass("vtkMRMLModelDisplayNode");
    scene->AddDefaultNode(defaultModelDisplayNode);
    defaultModelDisplayNode->Delete();
  }
  vtkMRMLModelDisplayNode::SafeDownCast(defaultModelDisplayNode)->SetBackfaceCulling(0);
  vtkMRMLModelDisplayNode::SafeDownCast(defaultModelDisplayNode)->SetVisibility2D(0);

  vtkMRMLNode* defaultSegmentationDisplayNode = scene->GetDefaultNodeByClass("vtkMRMLSegmentationDisplayNode");
  if (!defaultSegmentationDisplayNode)
  {
    defaultSegmentationDisplayNode = scene->CreateNodeByClass("vtkMRMLSegmentationDisplayNode");
    scene->AddDefaultNode(defaultSegmentationDisplayNode);
    defaultSegmentationDisplayNode->Delete();
  }
  vtkMRMLSegmentationDisplayNode::SafeDownCast(defaultSegmentationDisplayNode)->SetVisibility2DFill(0);
  vtkMRMLSegmentationDisplayNode::SafeDownCast(defaultSegmentationDisplayNode)->SetVisibility2DOutline(0);
}

// --------------------------------------------------------------------------
bool vtkSlicerVirtualRealityLogic::ShouldConsiderQuickViewMotion(
    double motionSensitivity, double physicalScale, double elapsedTimeInSec,
    double lastViewPos[3], double lastViewDir[3], double lastViewUp[3],
    double viewPos[3], double viewDir[3], double viewUp[3])
{
  if (motionSensitivity > 0.999)
  {
    return true;
  }
  else if (motionSensitivity <= 0.001)
  {
    return false;
  }
  else if (elapsedTimeInSec < 3.0)
  {
    const double limitScale = pow(100, 0.5 - motionSensitivity);
    const double angularSpeedLimitRadiansPerSec = vtkMath::RadiansFromDegrees(5.0 * limitScale);
    const double translationSpeedLimitMmPerSec = 100.0 * limitScale;

    const double viewDirectionChangeSpeed = vtkMath::AngleBetweenVectors(lastViewDir, viewDir) / elapsedTimeInSec;
    const double viewUpChangeSpeed = vtkMath::AngleBetweenVectors(lastViewUp, viewUp) / elapsedTimeInSec;
    const double viewTranslationSpeedMmPerSec =
        physicalScale * 0.01 * sqrt(vtkMath::Distance2BetweenPoints(lastViewPos, viewPos)) / elapsedTimeInSec;

    if (viewDirectionChangeSpeed < angularSpeedLimitRadiansPerSec
        && viewUpChangeSpeed < angularSpeedLimitRadiansPerSec
        && viewTranslationSpeedMmPerSec  < translationSpeedLimitMmPerSec)
    {
      return false;
    }
  }

  return true;
}
