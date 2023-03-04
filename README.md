# Fish-CAT : Fisheye Calibration and Application Toolbox.

## Purpose
Aim to generate a generic toolbox for fisheye lens, including fisheye lens intrinsic calibration, multi-fisheye lens extrinsic calibration, panoramic image stitching, Fisheye lens 3D reconstruction and Deep learning.

## Pre-requisition lib
- OpenCV
- Ceres-Solver
- GLOG

## How to use.
```shell
# clone the code.
git clone https://github.com/Haonan-DONG/fisheye-toolbox.git

# compile the code.
mkdir build && cd build
cmake ..
make -j
```

## Todo List
1. Module and Functional
- [ ] Add fisheye calibration pipeline for single board and sample data.
- [ ] Add KB model, FOV model.
- [ ] Add panoramic stitching algorithms.
- [ ] Add cylinder expansion.
- [ ] Add blender code for virtual data.

2. Code Polishing
- [ ] Add xml configuration in data preparing.
- [ ] Pre-requisition installation.
- [ ] Install and uninstall in cmake.
- [ ] Parallel.

3. Documentary
- [ ] Camera Lens.

## Third_Party
- [OpenCV](https://github.com/opencv/opencv)
- [Ceres-Solver](https://github.com/ceres-solver/ceres-solver)
- [PoseLib](https://github.com/vlarsson/PoseLib)

## Reference
1. [Paper](None)