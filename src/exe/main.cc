
// Copyright (c) 23, Haonan
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of Haonan nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: Haonan Dong

#include <iostream>
#include <vector>
#include <functional>
#include <iomanip>

#include "base/string_format.h"
#include "base/log.h"
#include "calibration/calibration_base.h"
#include "calibration/intrinsic_calibration.h"

typedef std::function<int(int, char **)> command_func_t;

int ShowHelp(const std::vector<std::pair<std::string, command_func_t>> &commands)
{

    std::cout << "fishcat -- Fisheye Calibration and Application Toolbox"
              << std::endl
              << std::endl;

    std::cout << "Usage:" << std::endl;
    std::cout << "  fishcat [command] [options]" << std::endl
              << std::endl;

    std::cout << "Documentation:" << std::endl;
    std::cout << "https://haonan-dong.github.io/" << std::endl
              << std::endl;

    std::cout
        << "Example usage:" << std::endl;
    std::cout << "  fishcat help [ -h, --help ]" << std::endl;
    std::cout << "  fishcat intrinsic_calibration" << std::endl;

    std::cout << "Available commands:" << std::endl;
    std::cout << "  help" << std::endl;
    for (const auto &command : commands)
    {
        std::cout << "  " << command.first << std::endl;
    }
    std::cout << std::endl;

    return EXIT_SUCCESS;
}

int RunIntrinsicCalibration(int argc, char **argv)
{
    fishcat::IntrinsicCalibrationHelp();
    fishcat::CalibrationSettings s;
    const std::string input_settings_file = argc > 1 ? argv[1] : "test.xml";

    // reading the input file and checking.
    cv::FileStorage fs(input_settings_file, cv::FileStorage::READ); // Read the CalibrationSettings

    if (!fs.isOpened())
    {
        LOG(ERROR) << "Could not open the configuration file: \""
                   << input_settings_file
                   << "\""
                   << std::endl;
        return -1;
    }

    // from the setting in the xml file.
    fs["Settings"] >> s;
    fs.release(); // close CalibrationSettings file

    if (!s.good_input_)
    {
        LOG(ERROR) << "Invalid input detected. Application stopping. "
                   << std::endl;
        return -1;
    }

    // preparing the image for calibration.
    std::vector<std::vector<cv::Point2f>> image_points;
    std::vector<std::vector<cv::Point3f>> object_points;
    cv::Mat camera_matrix, dist_coeffs;
    cv::Size image_size;

    const cv::Scalar RED(0, 0, 255), GREEN(0, 255, 0);
    int current_image_index = 0;

    // Processing the images.
    for (int i = 0; i < s.image_list_.size(); i++)
    {
        int chessBoardFlags;
        cv::Mat view, view_gray;
        std::vector<cv::Point2f> point_buf;
        std::vector<cv::Point3f> object_point;

        switch (s.calibration_pattern_)
        {
        case fishcat::CalibrationSettings::CHESSBOARD:
            view = s.NextImage();
            image_size = view.size(); // Format input image.

            cv::cvtColor(view, view_gray, cv::COLOR_BGR2GRAY);

            chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH;
            cv::findChessboardCorners(view_gray, s.board_size_, point_buf, chessBoardFlags);
            cv::cornerSubPix(view_gray, point_buf, cv::Size(11, 11),
                             cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
            for (int i = 0; i < s.board_size_.height; ++i)
                for (int j = 0; j < s.board_size_.width; ++j)
                    object_point.push_back(cv::Point3f(j * s.square_size_, i * s.square_size_, 0));

            image_points.push_back(point_buf);
            object_points.push_back(object_point);

            break;
        default:
            LOG(WARNING) << "Not suitable board found."
                         << std::endl;
            break;
        }

        LOG(INFO) << "Processing image " << std::setw(4) << current_image_index
                  << " in " << s.image_list_.size()
                  << " images, named of : "
                  << s.image_list_[i]
                  << std::endl;
        current_image_index++;
    }

    // here saves the re-projection error.
    if (image_points.size() > 0)
    {
        LOG(INFO) << "Images are all detected for their corner points, and will be calibrated."
                  << std::endl;
        RunCalibrationAndSave(s, image_size, camera_matrix, dist_coeffs, image_points, object_points);
    }
    else
    {
        LOG(ERROR) << "There are not sufficient chessboard corner points for calibration."
                   << std::endl;
        return false;
    }

    // ------------------- Quantitative Evaluation -------------------------------

    // -----------------------Show the undistorted image for the image list ------------------------
    if (s.input_type_ == fishcat::CalibrationSettings::IMAGE_LIST && s.show_undistorsed_)
    {
        cv::Mat map1, map2;
        if (s.use_fisheye_model_)
        {
            cv::fisheye::initUndistortRectifyMap(camera_matrix, dist_coeffs, cv::Mat(),
                                                 cv::getOptimalNewCameraMatrix(camera_matrix, dist_coeffs, image_size, 1, image_size, 0), image_size, CV_16SC2, map1, map2);
        }
        else
        {
            initUndistortRectifyMap(camera_matrix, dist_coeffs, cv::Mat(),
                                    getOptimalNewCameraMatrix(camera_matrix, dist_coeffs, image_size, 1, image_size, 0),
                                    image_size, CV_16SC2, map1, map2);
        }

#pragma omp parallel for
        for (int i = 0; i < (int)s.image_list_.size(); i++)
        {
            cv::Mat view, rview;
            std::string image_path = s.image_path_ + s.image_list_[i];
            view = cv::imread(image_path, 1);
            if (view.empty())
            {
                LOG(WARNING) << "Image is missing, name of : "
                             << image_path
                             << std::endl;
                continue;
            }
            cv::remap(view, rview, map1, map2, cv::INTER_LINEAR);
            LOG(INFO) << "Saving the undistorted images named : " + s.image_list_[i]
                      << std::endl;
            std::string undistorted_image_path = s.image_path_ + stringformat::Format("undistorted_image_{0}.jpg", s.image_list_[i]);
            cv::imwrite(undistorted_image_path, rview);
        }
    }

    return EXIT_SUCCESS;
}

int RunPanoramicStitching(int argc, char **argv)
{
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    InitialGoogleLog(argv);

    std::vector<std::pair<std::string, command_func_t>> commands;

    commands.emplace_back("intrinsic_calibration", &RunIntrinsicCalibration);
    commands.emplace_back("panoramic_stitching", &RunPanoramicStitching);

    if (argc == 1)
    {
        return ShowHelp(commands);
    }

    const std::string command = argv[1];
    if (command == "help" || command == "-h" || command == "--help")
    {
        return ShowHelp(commands);
    }
    else
    {
        command_func_t matched_command_func = nullptr;
        for (const auto &command_func : commands)
        {
            if (command == command_func.first)
            {
                matched_command_func = command_func.second;
                break;
            }
        }
        if (matched_command_func == nullptr)
        {
            std::cerr << "ERROR: Command not recognized. To list the "
                         "available commands, run `spreads help`."
                      << std::endl;
            return EXIT_FAILURE;
        }
        else
        {
            int command_argc = argc - 1;
            char **command_argv = &argv[1];
            command_argv[0] = argv[0];
            return matched_command_func(command_argc, command_argv);
        }
    }

    return ShowHelp(commands);
}