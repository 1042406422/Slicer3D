/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLVirtualRealityViewNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.3 $

=========================================================================auto=*/

#include <vtkMRMLScene.h>
#include <vtkMRMLViewNode.h>

#include "vtkMRMLVirtualRealityViewNode.h"

#include <vtkObjectFactory.h>

#include <sstream>

const char* vtkMRMLVirtualRealityViewNode::ReferenceViewNodeReferenceRole = "ReferenceViewNodeRef";

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLVirtualRealityViewNode);

//----------------------------------------------------------------------------
vtkMRMLVirtualRealityViewNode::vtkMRMLVirtualRealityViewNode()
  : TwoSidedLighting(false)
  , BackLights(true)
  , DesiredUpdateRate(60.0)
  , Magnification(10.0)
  , MotionSensitivity(0.5)
  , InteraxialDistance(65.0)
  , ZeroParallaxDistance(2000.0)
  , FocalDistance(2000.0)
  , StereoEnabled(true)
  , LeftEyeWindowVisible(true)
  , RightEyeWindowVisible(true)
  , LeftEyeFullScreen(false)
  , RightEyeFullScreen(false)
  , LeftEyeMonitorIndex(0)
  , RightEyeMonitorIndex(1)
{
  this->Visibility = 0;
  this->BackgroundColor[0] = this->defaultBackgroundColor()[0];
  this->BackgroundColor[1] = this->defaultBackgroundColor()[1];
  this->BackgroundColor[2] = this->defaultBackgroundColor()[2];
  this->BackgroundColor2[0] = this->defaultBackgroundColor2()[0];
  this->BackgroundColor2[1] = this->defaultBackgroundColor2()[1];
  this->BackgroundColor2[2] = this->defaultBackgroundColor2()[2];

  this->LeftEyeBackgroundColor[0] = 0.0;
  this->LeftEyeBackgroundColor[1] = 0.0;
  this->LeftEyeBackgroundColor[2] = 0.0;

  this->RightEyeBackgroundColor[0] = 0.0;
  this->RightEyeBackgroundColor[1] = 0.0;
  this->RightEyeBackgroundColor[2] = 0.0;

  this->MarkupsOcclusionEnabled = false;
}

//----------------------------------------------------------------------------
vtkMRMLVirtualRealityViewNode::~vtkMRMLVirtualRealityViewNode()
{
}

//----------------------------------------------------------------------------
const char* vtkMRMLVirtualRealityViewNode::GetNodeTagName()
{
  return "VirtualRealityView";
}

//----------------------------------------------------------------------------
void vtkMRMLVirtualRealityViewNode::WriteXML(ostream& of, int nIndent)
{
  this->Superclass::WriteXML(of, nIndent);

  vtkMRMLWriteXMLBeginMacro(of);
  vtkMRMLWriteXMLBooleanMacro(twoSidedLighting, TwoSidedLighting);
  vtkMRMLWriteXMLBooleanMacro(backLights, BackLights);
  vtkMRMLWriteXMLFloatMacro(desiredUpdateRate, DesiredUpdateRate);
  vtkMRMLWriteXMLFloatMacro(magnification, Magnification);
  vtkMRMLWriteXMLFloatMacro(motionSensitivity, MotionSensitivity);
  vtkMRMLWriteXMLFloatMacro(interaxialDistance, InteraxialDistance);
  vtkMRMLWriteXMLFloatMacro(zeroParallaxDistance, ZeroParallaxDistance);
  vtkMRMLWriteXMLFloatMacro(focalDistance, FocalDistance);
  vtkMRMLWriteXMLBooleanMacro(stereoEnabled, StereoEnabled);
  vtkMRMLWriteXMLBooleanMacro(leftEyeWindowVisible, LeftEyeWindowVisible);
  vtkMRMLWriteXMLBooleanMacro(rightEyeWindowVisible, RightEyeWindowVisible);
  vtkMRMLWriteXMLBooleanMacro(leftEyeFullScreen, LeftEyeFullScreen);
  vtkMRMLWriteXMLBooleanMacro(rightEyeFullScreen, RightEyeFullScreen);
  vtkMRMLWriteXMLIntMacro(leftEyeMonitorIndex, LeftEyeMonitorIndex);
  vtkMRMLWriteXMLIntMacro(rightEyeMonitorIndex, RightEyeMonitorIndex);
  vtkMRMLWriteXMLEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLVirtualRealityViewNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  this->Superclass::ReadXMLAttributes(atts);

  vtkMRMLReadXMLBeginMacro(atts);
  vtkMRMLReadXMLBooleanMacro(twoSidedLighting, TwoSidedLighting);
  vtkMRMLReadXMLBooleanMacro(backLights, BackLights);
  vtkMRMLReadXMLFloatMacro(desiredUpdateRate, DesiredUpdateRate);
  vtkMRMLReadXMLFloatMacro(magnification, Magnification);
  vtkMRMLReadXMLFloatMacro(motionSensitivity, MotionSensitivity);
  vtkMRMLReadXMLFloatMacro(interaxialDistance, InteraxialDistance);
  vtkMRMLReadXMLFloatMacro(zeroParallaxDistance, ZeroParallaxDistance);
  vtkMRMLReadXMLFloatMacro(focalDistance, FocalDistance);
  vtkMRMLReadXMLBooleanMacro(stereoEnabled, StereoEnabled);
  vtkMRMLReadXMLBooleanMacro(leftEyeWindowVisible, LeftEyeWindowVisible);
  vtkMRMLReadXMLBooleanMacro(rightEyeWindowVisible, RightEyeWindowVisible);
  vtkMRMLReadXMLBooleanMacro(leftEyeFullScreen, LeftEyeFullScreen);
  vtkMRMLReadXMLBooleanMacro(rightEyeFullScreen, RightEyeFullScreen);
  vtkMRMLReadXMLIntMacro(leftEyeMonitorIndex, LeftEyeMonitorIndex);
  vtkMRMLReadXMLIntMacro(rightEyeMonitorIndex, RightEyeMonitorIndex);
  vtkMRMLReadXMLEndMacro();

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLVirtualRealityViewNode::Copy(vtkMRMLNode* anode)
{
  int disabledModify = this->StartModify();

  this->Superclass::Copy(anode);

  vtkMRMLCopyBeginMacro(anode);
  vtkMRMLCopyBooleanMacro(TwoSidedLighting);
  vtkMRMLCopyBooleanMacro(BackLights);
  vtkMRMLCopyFloatMacro(DesiredUpdateRate);
  vtkMRMLCopyFloatMacro(Magnification);
  vtkMRMLCopyFloatMacro(MotionSensitivity);
  vtkMRMLCopyFloatMacro(InteraxialDistance);
  vtkMRMLCopyFloatMacro(ZeroParallaxDistance);
  vtkMRMLCopyFloatMacro(FocalDistance);
  vtkMRMLCopyBooleanMacro(StereoEnabled);
  vtkMRMLCopyBooleanMacro(LeftEyeWindowVisible);
  vtkMRMLCopyBooleanMacro(RightEyeWindowVisible);
  vtkMRMLCopyBooleanMacro(LeftEyeFullScreen);
  vtkMRMLCopyBooleanMacro(RightEyeFullScreen);
  vtkMRMLCopyIntMacro(LeftEyeMonitorIndex);
  vtkMRMLCopyIntMacro(RightEyeMonitorIndex);
  vtkMRMLCopyEndMacro();

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLVirtualRealityViewNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  vtkMRMLPrintBeginMacro(os, indent);
  vtkMRMLPrintBooleanMacro(TwoSidedLighting);
  vtkMRMLPrintBooleanMacro(BackLights);
  vtkMRMLPrintFloatMacro(DesiredUpdateRate);
  vtkMRMLPrintFloatMacro(Magnification);
  vtkMRMLPrintFloatMacro(MotionSensitivity);
  vtkMRMLPrintFloatMacro(InteraxialDistance);
  vtkMRMLPrintFloatMacro(ZeroParallaxDistance);
  vtkMRMLPrintFloatMacro(FocalDistance);
  vtkMRMLPrintBooleanMacro(StereoEnabled);
  vtkMRMLPrintBooleanMacro(LeftEyeWindowVisible);
  vtkMRMLPrintBooleanMacro(RightEyeWindowVisible);
  vtkMRMLPrintBooleanMacro(LeftEyeFullScreen);
  vtkMRMLPrintBooleanMacro(RightEyeFullScreen);
  vtkMRMLPrintIntMacro(LeftEyeMonitorIndex);
  vtkMRMLPrintIntMacro(RightEyeMonitorIndex);
  vtkMRMLPrintEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLVirtualRealityViewNode::SetSceneReferences()
{
  if (!this->Scene)
  {
    vtkErrorMacro( << "SetSceneReferences: Scene is expected to be non NULL.");
    return;
  }

  this->SetAndObserveParentLayoutNode(this);
}

//------------------------------------------------------------------------------
double* vtkMRMLVirtualRealityViewNode::defaultBackgroundColor()
{
  static double backgroundColor[3] = {0.7568627450980392,
                                      0.7647058823529412,
                                      0.9098039215686275
                                     };
  return backgroundColor;
}

//------------------------------------------------------------------------------
double* vtkMRMLVirtualRealityViewNode::defaultBackgroundColor2()
{
  static double backgroundColor2[3] = {0.4549019607843137,
                                       0.4705882352941176,
                                       0.7450980392156863
                                      };
  return backgroundColor2;
}

//------------------------------------------------------------------------------
double* vtkMRMLVirtualRealityViewNode::defaultLeftEyeBackgroundColor()
{
  static double color[3] = {0.0, 0.0, 0.0};
  return color;
}

//------------------------------------------------------------------------------
double* vtkMRMLVirtualRealityViewNode::defaultRightEyeBackgroundColor()
{
  static double color[3] = {0.0, 0.0, 0.0};
  return color;
}

//----------------------------------------------------------------------------
vtkMRMLViewNode* vtkMRMLVirtualRealityViewNode::GetReferenceViewNode()
{
  return vtkMRMLViewNode::SafeDownCast(this->GetNodeReference(vtkMRMLVirtualRealityViewNode::ReferenceViewNodeReferenceRole));
}

//----------------------------------------------------------------------------
void vtkMRMLVirtualRealityViewNode::SetAndObserveReferenceViewNodeID(const char* viewNodeId)
{
  this->SetAndObserveNodeReferenceID(vtkMRMLVirtualRealityViewNode::ReferenceViewNodeReferenceRole, viewNodeId);
}

//----------------------------------------------------------------------------
bool vtkMRMLVirtualRealityViewNode::SetAndObserveReferenceViewNode(vtkMRMLViewNode* node)
{
  if (node == nullptr)
  {
    this->SetAndObserveReferenceViewNodeID(nullptr);
    return true;
  }
  if (this->Scene != node->GetScene() || node->GetID() == nullptr)
  {
    vtkErrorMacro("SetAndObserveReferenceViewNode: The referenced and referencing node are not in the same scene");
    return false;
  }
  this->SetAndObserveReferenceViewNodeID(node->GetID());
  return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLVirtualRealityViewNode::HasError()
{
  return !this->LastErrorMessage.empty();
}

//----------------------------------------------------------------------------
void vtkMRMLVirtualRealityViewNode::ClearError()
{
  this->SetError("");
}

//----------------------------------------------------------------------------
void vtkMRMLVirtualRealityViewNode::SetError(const std::string& errorText)
{
  if (this->LastErrorMessage == errorText)
  {
    return;
  }
  this->LastErrorMessage = errorText;
  this->Modified();
}

//----------------------------------------------------------------------------
std::string vtkMRMLVirtualRealityViewNode::GetError() const
{
  return this->LastErrorMessage;
}
