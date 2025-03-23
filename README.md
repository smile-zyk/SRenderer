# SRenderer
SRenderer is a software rasterization renderer

![img](https://img2022.cnblogs.com/blog/1754745/202205/1754745-20220509041219464-648911963.png)
![img](https://img2022.cnblogs.com/blog/1754745/202205/1754745-20220509020500453-1435694532.png)
![img](https://img2022.cnblogs.com/blog/1754745/202205/1754745-20220509021507682-604754385.png)
![img](https://img2022.cnblogs.com/blog/1754745/202205/1754745-20220509021408282-1804216936.png)
![img](https://img2022.cnblogs.com/blog/1754745/202205/1754745-20220509021556503-1822989123.png)![img](https://img2022.cnblogs.com/blog/1754745/202205/1754745-20220509021612489-947227062.png)

# Implementation
- Bresenham's line algorithm
- Half-Space triangle rasterization algorithm
- Cohen-Sutherland & Sutherland-Hodgman algorithm(Homogeneous space clipping)
- Back-face culling
- Perspective correct interpolation
- Blinn-Phong shader model
- Texture Sample
- Orbital camera
- User-Friendly graphical interface
  ...
> More features will be added in the future(maybe) 

# Display
- control the model
![img](https://img2022.cnblogs.com/blog/1754745/202205/1754745-20220509041513760-51626822.gif)
- switch theme
![img](https://img2022.cnblogs.com/blog/1754745/202205/1754745-20220509033444899-807773672.gif)
...
>[More information in Zhihu](https://www.zhihu.com/question/24786878/answer/2476658460)
  
# Third Party
- [QT 6.2.4](https://www.qt.io/blog/qt-6.2.4-released)(GUI)
- [QDarkStyleSheet](https://github.com/ColinDuquesnoy/QDarkStyleSheet)(Style sheet to Beautify QT interface)
- [Assimp](https://github.com/assimp/assimp)(Load model)
> Only *.obj files can be loaded for now
- [TBB](https://github.com/oneapi-src/oneTBB)(Multithread acceleration)
- [GLM](https://github.com/Groovounet/glm)(Linear algebra and SIMD support)

# Download
Executable file package for Windows10/64 bit(Qt6 stops supporting Win7 and Win8, and only supports 64-bit systems) can download from the [Releases page](https://github.com/smile-zyk/SRenderer/releases/tag/v1.0.0).

# Build
you can clone this repository and build with QT6.2.4, windows third party packages is already in this repository.

## Build on Linux
### Dependencies
```
# archlinux:
sudo pacman -S onetbb assimp qt6

# ubuntu 24.04 LTS:
sudo apt install qt6-base-dev libtbb-dev libassimp-dev
```

### Build
```
mkdir build
cmake -B build -S .
cmake --build ./build --parallel
```
