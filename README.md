# Fish-CAT : Fisheye Calibration and Application Toolbox.

## Purpose
Aim to generate a generic toolbox for fisheye lens, including fisheye lens intrinsic calibration, multi-fisheye lens extrinsic calibration, panoramic image stitching, Fisheye lens 3D reconstruction and Deep learning.

## Pre-requisition lib (Tested Version)
- OpenCV (3.4.6)
- Ceres-Solver (2.0.0)
- GLOG
- Eigen (3.3.9)

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
- [X] Add fisheye calibration pipeline for single board and sample data.
- [X] Add K-B model.
- [X] Add longitude-latitude expansion.(Cylindrical Equidistant Projection or Equirectangular Projection).
- [ ] Add panoramic stitching algorithms.
- [ ] Add FOV model.
- [ ] Add centroid circle estimation.
- [ ] Add Ortho-rectified implementation.
- [ ] Add omnidirectional model.
- [ ] Add blender code for virtual data.
- [ ] Add blender patch for K-B model.
- [ ] Add 23-parameter estimation method.
- [ ] Add pixel interpolation.

2. Code Polishing
- [X] Add xml configuration in data preparing.
- [ ] Initialization function.
- [ ] Add setting_base, separated from calibration_base class.
- [X] Pre-requisition installation.
- [ ] Install and uninstall in cmake.
- [ ] Parallel for fisheye to equirectangular projection.
- [ ] Parallel for image undistortion.
- [ ] Using third-Party xml class
- [ ] s.file_type = unknown?
- [ ] Single image failure bug.

3. Documentary
- [ ] Camera Lens.
- [ ] How to chose proper model for modeling the lens.

4. Machine
- [X] GoPro
- [ ] Insta360
- [ ] Stereo Fisheye

5. Demo Data
- GoPro Hero 4 with chessboard (14\*9) in [Baidu Disk](https://pan.baidu.com/s/1pjY5FuheeUftFYjDW7jffg)(*pwd: z7jz*).

## Third_Party
- [OpenCV](https://github.com/opencv/opencv)
- [Ceres-Solver](https://github.com/ceres-solver/ceres-solver)
- [PoseLib](https://github.com/vlarsson/PoseLib)

## Reference
1. [Paper](None)