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
- [ ] Add K-B model.
- [ ] Add FOV model.
- [ ] Add longitude-latitude expansion.(Cylindrical Equidistant Projection or Equirectangular Projection)
- [ ] Add centroid circle estimation.
- [ ] Add Ortho-rectified implementation.
- [ ] Add panoramic stitching algorithms.
- [ ] Add omnidirectional model.
- [ ] Add blender code for virtual data.
- [ ] Add blender patch for K-B model.
- [ ] Add 23-parameter estimation method.

2. Code Polishing
- [X] Add xml configuration in data preparing.
- [ ] Pre-requisition installation.
- [ ] Install and uninstall in cmake.
- [ ] Parallel.
- [ ] Using third-Party xml class

3. Documentary
- [ ] Camera Lens.
- [ ] How to chose proper model for modeling the lens.

4. Machine
- [ ] Insta360
- [ ] Stereo Fisheye

## Third_Party
- [OpenCV](https://github.com/opencv/opencv)
- [Ceres-Solver](https://github.com/ceres-solver/ceres-solver)
- [PoseLib](https://github.com/vlarsson/PoseLib)

## Reference
1. [Paper](None)