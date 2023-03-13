#include <base/log.h>
#include <iostream>

#include "calibration/intrinsic_calibration.h"

namespace fishcat
{

    void IntrinsicCalibrationHelp()
    {
        LOG(INFO) << "Official Camera Intrinsic Calibration." << std::endl;
        LOG(INFO) << "Near the sample file you'll find the configuration file, which has detailed help of "
                     "how to edit it.  It may be any OpenCV supported file format XML/YAML."
                  << std::endl;
    }

    double ComputeReprojectionErrors(const std::vector<std::vector<cv::Point3f>> &object_points,
                                     const std::vector<std::vector<cv::Point2f>> &image_points,
                                     const std::vector<cv::Mat> &rvecs, const std::vector<cv::Mat> &tvecs,
                                     const cv::Mat &camera_matrix, const cv::Mat &dist_coeffs,
                                     std::vector<float> &perViewErrors,
                                     std::vector<std::vector<double>> &reproj_error_single,
                                     bool use_fisheye = false)
    {
        std::vector<cv::Point2f> imagePoints2;
        int i, totalPoints = 0;
        double totalErr = 0, err;
        perViewErrors.resize(object_points.size());
        std::vector<double> reproj_error_single_vector;

        for (i = 0; i < (int)object_points.size(); ++i)
        {

            cv::projectPoints(cv::Mat(object_points[i]), rvecs[i], tvecs[i], camera_matrix,
                              dist_coeffs, imagePoints2);

            for (int j = 0; j < image_points[i].size(); j++)
            {
                reproj_error_single_vector.push_back(std::sqrt((image_points[i][j].x - imagePoints2[j].x) *
                                                                   (image_points[i][j].x - imagePoints2[j].x) +
                                                               (image_points[i][j].y - imagePoints2[j].y) *
                                                                   (image_points[i][j].y - imagePoints2[j].y)));
            }

            reproj_error_single.push_back(reproj_error_single_vector);
            reproj_error_single_vector.clear();

            err = norm(cv::Mat(image_points[i]), cv::Mat(imagePoints2), cv::NORM_L2);

            int n = (int)object_points[i].size();
            perViewErrors[i] = (float)std::sqrt(err * err / n);
            totalErr += err * err;
            totalPoints += n;
        }

        return std::sqrt(totalErr / totalPoints);
    }

    bool RunCalibration(CalibrationSettings &s, cv::Size &image_size, cv::Mat &camera_matrix, cv::Mat &dist_coeffs,
                        std::vector<std::vector<cv::Point2f>> image_points,
                        std::vector<std::vector<cv::Point3f>> object_points,
                        std::vector<cv::Mat> &rvecs, std::vector<cv::Mat> &tvecs,
                        std::vector<float> &reproj_errs, double &avg_err)
    {
        std::vector<std::vector<double>> reproj_err_single;
        int total_number = 0;
        for (int index = 0; index < object_points.size(); index++)
        {
            total_number += object_points[index].size();
        }

        camera_matrix = cv::Mat::eye(3, 3, CV_64F);
        if (s.flag_)
            camera_matrix.at<double>(0, 0) = 1.0;

        dist_coeffs = cv::Mat::zeros(4, 1, CV_64F);

        // Find intrinsic and extrinsic camera parameters
        double rms;
        if (s.use_fisheye_model_)
        {
            rms = cv::fisheye::calibrate(object_points, image_points, image_size, camera_matrix, dist_coeffs, rvecs, tvecs, s.flag_);
            avg_err = ComputeReprojectionErrors(object_points, image_points,
                                                rvecs, tvecs, camera_matrix, dist_coeffs,
                                                reproj_errs, reproj_err_single);
        }
        else
        {
            rms = cv::calibrateCamera(object_points, image_points, image_size, camera_matrix,
                                      dist_coeffs, rvecs, tvecs, s.flag_ | cv::CALIB_FIX_K4 | cv::CALIB_FIX_K5);
            avg_err = ComputeReprojectionErrors(object_points, image_points,
                                                rvecs, tvecs, camera_matrix, dist_coeffs,
                                                reproj_errs, reproj_err_single);
        }

        // ---- log out the re-projection error ----
        LOG(INFO) << "Re-projection error reported by CalibrateCamera: " << rms
                  << " and average re-projection error is : " << avg_err
                  << std::endl;
        bool ok = cv::checkRange(camera_matrix) && checkRange(dist_coeffs);

        return ok;
    }

    bool RunCalibrationAndSave(CalibrationSettings &s, cv::Size image_size, cv::Mat &camera_matrix, cv::Mat &dist_coeffs, std::vector<std::vector<cv::Point2f>> image_points, std::vector<std::vector<cv::Point3f>> object_points)
    {
        std::vector<cv::Mat> rvecs, tvecs;
        std::vector<float> reproj_errs;
        double total_avg_err = 0;

        bool ok = RunCalibration(s, image_size, camera_matrix, dist_coeffs,
                                 image_points, object_points,
                                 rvecs, tvecs,
                                 reproj_errs, total_avg_err);
        LOG(INFO) << (ok ? "Calibration succeeded" : "Calibration failed");

        if (ok)
            SaveIntrinsicCameraParams(s, image_size, camera_matrix, dist_coeffs, rvecs, tvecs, reproj_errs,
                                      image_points, total_avg_err);
        return ok;
    }

    // Print camera parameters to the output file
    void SaveIntrinsicCameraParams(CalibrationSettings &s, cv::Size &image_size, cv::Mat &camera_matrix, cv::Mat &dist_coeffs,
                                   const std::vector<cv::Mat> &rvecs, const std::vector<cv::Mat> &tvecs,
                                   const std::vector<float> &reproj_errs, const std::vector<std::vector<cv::Point2f>> &image_points,
                                   double total_avg_err)
    {
        cv::FileStorage fs(s.output_fileName_, cv::FileStorage::WRITE);

        time_t tm;
        time(&tm);
        struct tm *t2 = localtime(&tm);
        char buf[1024];
        strftime(buf, sizeof(buf) - 1, "%c", t2);

        fs << "calibration_Time" << buf;

        if (!rvecs.empty() || !reproj_errs.empty())
            fs << "nrOfFrames" << (int)std::max(rvecs.size(), reproj_errs.size());
        fs << "image_Width" << image_size.width;
        fs << "image_Height" << image_size.height;
        fs << "board_Width" << s.board_size_.width;
        fs << "board_Height" << s.board_size_.height;
        fs << "square_Size" << s.square_size_;

        if (s.flag_ & cv::CALIB_FIX_ASPECT_RATIO)
            fs << "FixAspectRatio" << s.aspect_ratio_;

        if (s.flag_)
        {
            sprintf(buf, "flags: %s%s%s%s",
                    s.flag_ & cv::CALIB_USE_INTRINSIC_GUESS ? " +use_intrinsic_guess" : "",
                    s.flag_ & cv::CALIB_FIX_ASPECT_RATIO ? " +fix_aspect_ratio_" : "",
                    s.flag_ & cv::CALIB_FIX_PRINCIPAL_POINT ? " +fix_principal_point" : "",
                    s.flag_ & cv::CALIB_ZERO_TANGENT_DIST ? " +zero_tangent_dist" : "");
            // cvWriteComment( *fs, buf, 0 );
        }

        fs << "flagValue" << s.flag_;

        fs << "Camera_Matrix" << camera_matrix;
        fs << "Distortion_Coefficients" << dist_coeffs;

        fs << "Avg_Reprojection_Error" << total_avg_err;
        if (!reproj_errs.empty())
            fs << "Per_View_Reprojection_Errors" << cv::Mat(reproj_errs);

        if (!rvecs.empty() && !tvecs.empty())
        {
            CV_Assert(rvecs[0].type() == tvecs[0].type());
            cv::Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
            for (int i = 0; i < (int)rvecs.size(); i++)
            {
                cv::Mat r = bigmat(cv::Range(i, i + 1), cv::Range(0, 3));
                cv::Mat t = bigmat(cv::Range(i, i + 1), cv::Range(3, 6));

                CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
                CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
                //*.t() is MatExpr (not cv::Mat) so we can use assignment operator
                r = rvecs[i].t();
                t = tvecs[i].t();
            }
            // cvWriteComment( *fs, "a set of 6-tuples (rotation std::vector + translation std::vector) for each view", 0 );
            fs << "Extrinsic_Parameters" << bigmat;
        }

        if (!image_points.empty())
        {
            cv::Mat imagePtMat((int)image_points.size(), (int)image_points[0].size(), CV_32FC2);
            for (int i = 0; i < (int)image_points.size(); i++)
            {
                cv::Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
                cv::Mat imgpti(image_points[i]);
                imgpti.copyTo(r);
            }
            fs << "Image_points" << imagePtMat;
        }
    }
} // namespace fishcat
