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

## Module
### Fisheye Intrinsic Calibration.
```shell
fishcat intrinsic_calibration path_to_settings_intrinsic.xml
```

1. Demo Data
- GoPro Hero 4 with chessboard (14\*9) in [Baidu Disk](https://pan.baidu.com/s/1pjY5FuheeUftFYjDW7jffg)(*pwd: z7jz*). Note that the abs_path_to_xml and abs_path_to_img should be specified.
2. Distortion model
Kannala-Brandt Model.

### Single-Fisheye Cylindrical Expansion.
```shell
fishcat fisheye_expansion path_to_settings.xml
```

1. Demo Data

Convert calibrated fisheye image into cylindrical projection. The original fisheye image is calibrated with known intrinsics and distortion. Data will be released.
 <div align=center> <img src="doc/imgs/fisheye_expansion.png"
  width = 100%/> </div>

2. Distortion model.
Kannala-Brandt Model (Instead of FOV expansion.)

## Todo List
1. Calibration Module
- [X] Add fisheye calibration pipeline for single board and sample data.
- [X] Add K-B model.
- [ ] Add PNP on extrinsic calibration.
- [ ] Try to test different PnP module.
- [ ] Add FOV model.
- [ ] Add centroid circle estimation.
- [ ] Add omnidirectional model.
- [ ] Add 23-parameter estimation method.
- [ ] Add pixel interpolation.

2. Panoramic Stitching Module
- [X] Add longitude-latitude expansion.(Cylindrical Equidistant Projection or Equirectangular Projection).
- [ ] Add panoramic stitching algorithms.

3. Fisheye Projection Module
- [ ] Add Ortho-rectified implementation.
- [ ] Add perspective projection and cube-map expansion.

4. Data Generator Module
- [ ] Add blender code for virtual data.
- [ ] Add blender patch for K-B model.

5. Code Polishing
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

6. Documentary
- [ ] Camera Lens.
- [ ] How to chose proper model for modeling the lens.

7. Machine
- [X] GoPro
- [X] Insta360
- [ ] Stereo Fisheye

## Third_Party
- [OpenCV](https://github.com/opencv/opencv)
- [Ceres-Solver](https://github.com/ceres-solver/ceres-solver)
- [PoseLib](https://github.com/vlarsson/PoseLib)

## Reference
1. [Paper](None)

## Citation

If you use this library or find the documentation useful for your research, please consider citing:

```bibtex
@misc{Dong2023fishcat,
    author    = {Haonan Dong},
    title     = {FishCat:Fisheye Calibration and Application Toolbox.},
    year      = {2023},
    url       = {https://github.com/Haonan-DONG/fisheye-toolbox},
}
```