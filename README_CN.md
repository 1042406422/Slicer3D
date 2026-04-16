# Slicer3D 双机偏振投影3D显示模块

## 简介

这是一个将原SlicerVirtualReality插件改造为双机偏振投影3D显示系统的版本。

## 功能特点

- 双视图渲染系统（左右眼分离渲染）
- 立体相机系统（支持瞳距、视差调整）
- 兼容偏振投影系统

## 安装方法

### 方法一：GitHub Actions自动构建（推荐）

1. 将此项目上传到GitHub仓库
2. 进入Actions页面，点击"Build Slicer3D Extension"工作流
3. 点击"Run workflow"，选择Slicer版本
4. 等待构建完成后，下载Artifacts中的zip文件
5. 在3D Slicer中：View → Extension Manager → Install from file

### 方法二：本地构建

#### 前提条件

1. **安装3D Slicer**（版本5.6或更高）
   - 从 https://download.slicer.org 下载

2. **安装开发工具**
   ```powershell
   # 使用winget安装
   winget install Kitware.CMake
   winget install Git.Git
   winget install Microsoft.VisualStudio.2022.BuildTools --override "--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
   ```

3. **下载Slicer开发版本**
   - 需要从源码编译Slicer，或使用包含开发文件的版本

#### 构建步骤

```powershell
# 1. 打开PowerShell，进入项目目录
cd "e:\diy rj\SlicerVirtualReality-master"

# 2. 创建构建目录
mkdir build
cd build

# 3. 配置项目（需要指定Slicer开发目录）
cmake -G "Visual Studio 17 2022" -A x64 `
  -DSlicer_DIR:PATH="C:/path/to/Slicer/lib/Slicer-5.10" `
  -DCMAKE_BUILD_TYPE:STRING=Release `
  -DSlicer3D_SUPERBUILD:BOOL=OFF `
  ..

# 4. 编译
cmake --build . --config Release

# 5. 打包
cpack -C Release
```

## 使用方法

1. 打开 3D Slicer
2. 在模块列表中找到 **3D** 模块
3. 点击 **"Enable stereo view"** 启用立体显示
4. 点击 **"Enable rendering"** 开始渲染
5. 将左右眼渲染窗口分别输出到两台投影机

## 参数说明

| 参数 | 说明 | 默认值 |
|------|------|--------|
| Interaxial Distance | 瞳距（毫米） | 65.0 |
| Zero Parallax Distance | 零视差距离 | 2000.0 |
| Magnification | 放大倍数 | 10.0 |
| Motion Sensitivity | 运动灵敏度 | 50% |

## 双机偏振投影设置

1. 准备两台投影机，分别安装偏振片（正交方向）
2. 将左右眼渲染窗口拖动到对应显示器
3. 观众佩戴对应的偏振眼镜观看立体效果

## 技术原理

- **立体相机系统**：根据瞳距自动计算左右眼相机位置
- **视差控制**：通过零视差距离控制物体的立体深度感
- **分离渲染**：两个独立的渲染窗口，分别输出到两台投影机

## 许可证

Apache License 2.0
