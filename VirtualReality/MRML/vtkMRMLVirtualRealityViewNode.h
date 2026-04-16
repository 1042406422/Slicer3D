/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVirtualRealityViewNode.h,v $
  Date:      $Date: 2006/03/19 17:12:28 $
  Version:   $Revision: 1.6 $

=========================================================================auto=*/

#ifndef __vtkMRMLVirtualRealityViewNode_h
#define __vtkMRMLVirtualRealityViewNode_h

#include <vtkMRMLViewNode.h>
#include <vtkMRMLLinearTransformNode.h>

#include <vtkEventData.h>

#include "vtkSlicerVirtualRealityModuleMRMLExport.h"

class VTK_SLICER_VIRTUALREALITY_MODULE_MRML_EXPORT vtkMRMLVirtualRealityViewNode
  : public vtkMRMLViewNode
{
public:
  static vtkMRMLVirtualRealityViewNode* New();
  vtkTypeMacro(vtkMRMLVirtualRealityViewNode, vtkMRMLViewNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkMRMLNode* CreateNodeInstance() override;

  void ReadXMLAttributes(const char** atts) override;
  void WriteXML(ostream& of, int indent) override;
  void Copy(vtkMRMLNode* node) override;
  const char* GetNodeTagName() override;
  void SetSceneReferences() override;

  static double* defaultBackgroundColor();
  static double* defaultBackgroundColor2();

  vtkMRMLViewNode* GetReferenceViewNode();
  void SetAndObserveReferenceViewNodeID(const char* layoutNodeId);
  bool SetAndObserveReferenceViewNode(vtkMRMLViewNode* node);

  static double* defaultLeftEyeBackgroundColor();
  static double* defaultRightEyeBackgroundColor();

  vtkGetMacro(InteraxialDistance, double);
  vtkSetMacro(InteraxialDistance, double);

  vtkGetMacro(ZeroParallaxDistance, double);
  vtkSetMacro(ZeroParallaxDistance, double);

  vtkGetMacro(FocalDistance, double);
  vtkSetMacro(FocalDistance, double);

  vtkGetMacro(StereoEnabled, bool);
  vtkSetMacro(StereoEnabled, bool);
  vtkBooleanMacro(StereoEnabled, bool);

  vtkGetMacro(LeftEyeWindowVisible, bool);
  vtkSetMacro(LeftEyeWindowVisible, bool);
  vtkBooleanMacro(LeftEyeWindowVisible, bool);

  vtkGetMacro(RightEyeWindowVisible, bool);
  vtkSetMacro(RightEyeWindowVisible, bool);
  vtkBooleanMacro(RightEyeWindowVisible, bool);

  vtkGetMacro(TwoSidedLighting, bool);
  vtkSetMacro(TwoSidedLighting, bool);
  vtkBooleanMacro(TwoSidedLighting, bool);

  vtkGetMacro(BackLights, bool);
  vtkSetMacro(BackLights, bool);
  vtkBooleanMacro(BackLights, bool);

  vtkGetMacro(DesiredUpdateRate, double);
  vtkSetMacro(DesiredUpdateRate, double);

  vtkGetMacro(Magnification, double);
  vtkSetMacro(Magnification, double);

  vtkGetMacro(MotionSensitivity, double);
  vtkSetMacro(MotionSensitivity, double);

  bool HasError();
  void ClearError();
  void SetError(const std::string& errorText);
  std::string GetError() const;

  vtkGetVector3Macro(LeftEyeBackgroundColor, double);
  vtkSetVector3Macro(LeftEyeBackgroundColor, double);

  vtkGetVector3Macro(RightEyeBackgroundColor, double);
  vtkSetVector3Macro(RightEyeBackgroundColor, double);

  vtkGetMacro(LeftEyeFullScreen, bool);
  vtkSetMacro(LeftEyeFullScreen, bool);
  vtkBooleanMacro(LeftEyeFullScreen, bool);

  vtkGetMacro(RightEyeFullScreen, bool);
  vtkSetMacro(RightEyeFullScreen, bool);
  vtkBooleanMacro(RightEyeFullScreen, bool);

  vtkGetMacro(LeftEyeMonitorIndex, int);
  vtkSetMacro(LeftEyeMonitorIndex, int);

  vtkGetMacro(RightEyeMonitorIndex, int);
  vtkSetMacro(RightEyeMonitorIndex, int);

protected:
  bool TwoSidedLighting;
  bool BackLights;
  double DesiredUpdateRate;
  double Magnification;
  double MotionSensitivity;

  double InteraxialDistance;
  double ZeroParallaxDistance;
  double FocalDistance;
  bool StereoEnabled;

  bool LeftEyeWindowVisible;
  bool RightEyeWindowVisible;

  double LeftEyeBackgroundColor[3];
  double RightEyeBackgroundColor[3];

  bool LeftEyeFullScreen;
  bool RightEyeFullScreen;

  int LeftEyeMonitorIndex;
  int RightEyeMonitorIndex;

  std::string LastErrorMessage;

  vtkMRMLVirtualRealityViewNode();
  ~vtkMRMLVirtualRealityViewNode() override;
  vtkMRMLVirtualRealityViewNode(const vtkMRMLVirtualRealityViewNode&);
  void operator=(const vtkMRMLVirtualRealityViewNode&);

  static const char* ReferenceViewNodeReferenceRole;
};

#endif
