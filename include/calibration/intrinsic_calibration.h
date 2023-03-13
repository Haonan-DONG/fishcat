#ifndef INTRINSIC_CALIBRATION_H_
#define INTRINSIC_CALIBRATION_H_

#include "calibration/calibration_base.h"

namespace fishcat
{
    void IntrinsicCalibrationHelp();

    void SaveIntrinsicCameraParams(CalibrationSettings &s, cv::Size &image_size, cv::Mat &camera_matrix, cv::Mat &dist_coeffs,
                                   const std::vector<cv::Mat> &rvecs, const std::vector<cv::Mat> &tvecs,
                                   const std::vector<float> &reproj_errs, const std::vector<std::vector<cv::Point2f>> &image_points,
                                   double total_avg_err);
    bool RunCalibrationAndSave(CalibrationSettings &s, cv::Size image_size, cv::Mat &camera_matrix, cv::Mat &dist_coeffs, std::vector<std::vector<cv::Point2f>> image_points, std::vector<std::vector<cv::Point3f>> object_points);
    bool RunCalibration(CalibrationSettings &s, cv::Size &image_size, cv::Mat &camera_matrix, cv::Mat &dist_coeffs,
                        std::vector<std::vector<cv::Point2f>> image_points,
                        std::vector<std::vector<cv::Point3f>> object_points,
                        std::vector<cv::Mat> &rvecs, std::vector<cv::Mat> &tvecs,
                        std::vector<float> &reproj_errs, double &avg_err);
    double ComputeReprojectionErrors(const std::vector<std::vector<cv::Point3f>> &object_points,
                                     const std::vector<std::vector<cv::Point2f>> &image_points,
                                     const std::vector<cv::Mat> &rvecs, const std::vector<cv::Mat> &tvecs,
                                     const cv::Mat &camera_matrix, const cv::Mat &dist_coeffs,
                                     std::vector<float> &perViewErrors,
                                     std::vector<std::vector<double>> &reproj_error_single,
                                     bool use_fisheye);
}

#endif