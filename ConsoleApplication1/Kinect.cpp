#include <opencv2/opencv.hpp>

#include <iostream>
#include <viewer/k4adepthpixelcolorizer.h>
#include <viewer/k4apixel.h>
#include <viewer/k4astaticimageproperties.h>
#include <viewer/texture.h>
#include <viewer/viewerwindow.h>
#include <k4abt.h>
#include <k4abt.hpp>

#include "Kinect.h"
#include "Client.h"
#include "Data.h"

#pragma region Kinect_imu

uint32_t device_count = 0;
k4a_device_t device = NULL;
size_t serial_size = 0;
char* serial = 0;

k4a_device_configuration_t config;
k4a_calibration_t calibration;
k4a_transformation_t transformation;
k4a_capture_t capture = NULL;
k4a_wait_result_t get_capture_result;

#pragma endregion

#pragma region Values
#pragma endregion

#pragma region sending_data_saving_data

char sending_data[20];
char csv_kinect_data[1024];

#pragma endregion
//넣는 순서 중요

void connet()
{
    if (device_count != 0)
    {
        printf("device already conneted");
        return;
    };

    device_count = k4a_device_get_installed_count();
    printf("Found %d connected devices:\n", device_count);
   

    for (uint8_t deviceIndex = 0; deviceIndex < device_count; deviceIndex++)
    {
        if (K4A_RESULT_SUCCEEDED != k4a_device_open(deviceIndex, &device))
        {
            printf("%d: Failed to open device\n", deviceIndex);
            return;
        }
    }
}

int color_viewer()
{
    // Allocate memory for the serial, then acquire it
    // Get the size of the serial number    
    k4a_device_get_serialnum(device, NULL, &serial_size);
    serial = (char*)(malloc(serial_size));
    k4a_device_get_serialnum(device, serial, &serial_size);
    printf("Opened device: %s\n", serial);
    free(serial);
    
    // Configure a stream of 4096x3072 BRGA color data at 15 frames per second
    config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.camera_fps = K4A_FRAMES_PER_SECOND_15;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32; //컬러 이미지를 위해서는 BGRA32를 사용해야 한다.
    config.color_resolution = K4A_COLOR_RESOLUTION_720P;
    config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
    config.synchronized_images_only = true;
    
    
    if (K4A_RESULT_SUCCEEDED != k4a_device_get_calibration(device, config.depth_mode, config.color_resolution, &calibration))
    {
        printf("Failed to get calibration\n");
        return 1;
    }

    transformation = k4a_transformation_create(&calibration);

    // Start the camera with the given configuration
    if (K4A_FAILED(k4a_device_start_cameras(device, &config)))
    {
        printf("Failed to start cameras!\n");
        k4a_device_close(device);
        return 1;
    }    

    while (device != NULL)
    {        
        get_capture_result = k4a_device_get_capture(device, &capture, K4A_WAIT_INFINITE);

        // Capture a depth frame
        switch (k4a_device_get_capture(device, &capture, K4A_WAIT_INFINITE))
        {
        case K4A_WAIT_RESULT_SUCCEEDED:
            break;
        case K4A_WAIT_RESULT_TIMEOUT:
            printf("Timed out waiting for a capture\n");
            break;
        case K4A_WAIT_RESULT_FAILED:
            printf("Failed to read a capture\n");
            break;
        }

        // Camera capture and application specific code would go here
        // Access the depth16 image
        k4a_image_t image = k4a_capture_get_color_image(capture);

        uint8_t* color_buffer = k4a_image_get_buffer(image);

        /*printf(" | Depth16 res:%4dx%4d stride:%5d\n",
            k4a_image_get_height_pixels(image),
            k4a_image_get_width_pixels(image),
            k4a_image_get_stride_bytes(image));*/

        int rows = k4a_image_get_height_pixels(image);
        int cols = k4a_image_get_width_pixels(image);

        cv::Mat color = cv::Mat(rows, cols, CV_8UC4, color_buffer);

        cv::imshow("color_view", color);
        
        int key = cv::waitKey(1);
        if (key == 27) 
        {
            printf("key pressed\n");
            cv::destroyWindow("color_view");
            break;
        }

        // Release the capture
        k4a_capture_release(capture);
        k4a_image_release(image);
    }

    printf("viewer closed\n");

    return 0;
}


int body_viewer()
{
    //Configure a stream of 4096x3072 BRGA color data at 15 frames per second
    config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.camera_fps = K4A_FRAMES_PER_SECOND_15;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32; //컬러 이미지를 위해서는 BGRA32를 사용해야 한다.
    config.color_resolution = K4A_COLOR_RESOLUTION_720P;
    config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
    config.synchronized_images_only = true;

    if (K4A_RESULT_SUCCEEDED != k4a_device_get_calibration(device, config.depth_mode, K4A_COLOR_RESOLUTION_OFF, &calibration))
    {
        printf("Get depth camera calibration failed!\n");
        return 0;
    }

    k4abt_tracker_configuration_t tracker_configuration = K4ABT_TRACKER_CONFIG_DEFAULT;

    k4abt::tracker tracker = k4abt::tracker::create(calibration);

    multi_csv_kinect();     //csv로 저장하기 위해 파일 생성

    while (device != NULL)
    {       
        try
        {
            get_capture_result = k4a_device_get_capture(device, &capture, K4A_WAIT_INFINITE);

            if (!tracker.enqueue_capture(capture))
            {
                // It should never hit timeout when K4A_WAIT_INFINITE is set.
                std::cout << "Error! Add capture to tracker process queue timeout!" << std::endl;
                return 0;
            }

            k4abt::frame body_frame = tracker.pop_result();
            if (body_frame != nullptr)
            {
                uint32_t num_bodies = body_frame.get_num_bodies();
                //printf("num_bodies : %d\n", num_bodies);

                /*double time_ = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(
                    body_frame.get_device_timestamp()).count());*/

                for (uint32_t i = 0; i < num_bodies; i++)
                {
                    k4abt_body_t body = body_frame.get_body(i);
                    /*kvalues.clear();
                    kvalues.push_back(LocalMilli());*/
#pragma region BODYS
                    // neck
                    k4a_float3_t neck_pos = body.skeleton.joints[NECK].position;
                    k4a_quaternion_t neck_qua = body.skeleton.joints[NECK].orientation;
                    k4abt_joint_confidence_level_t neck_conf = body.skeleton.joints[NECK].confidence_level;

                    // PELVIS
                    k4a_float3_t pelvis_pos = body.skeleton.joints[PELVIS].position;
                    k4a_quaternion_t pelvis_qua = body.skeleton.joints[PELVIS].orientation;
                    k4abt_joint_confidence_level_t pelvis_conf = body.skeleton.joints[PELVIS].confidence_level;

                    //////////////////////////////////////////////////// LEFT ////////////////////////////////////////////////////

                    // left shoulder
                    k4a_float3_t left_sholder_pos = body.skeleton.joints[SHOULDER_LEFT].position;
                    k4a_quaternion_t left_sholder_qua = body.skeleton.joints[SHOULDER_LEFT].orientation;
                    k4abt_joint_confidence_level_t left_sholder_conf = body.skeleton.joints[SHOULDER_LEFT].confidence_level;

                    // left elbow
                    k4a_float3_t elbow_left_pos = body.skeleton.joints[ELBOW_LEFT].position;
                    k4a_quaternion_t elbow_left_qua = body.skeleton.joints[ELBOW_LEFT].orientation;
                    k4abt_joint_confidence_level_t elbow_left_conf = body.skeleton.joints[ELBOW_LEFT].confidence_level;

                    // left wrist
                    k4a_float3_t left_wrist_pos = body.skeleton.joints[WRIST_LEFT].position;
                    k4a_quaternion_t left_wrist_qua = body.skeleton.joints[WRIST_LEFT].orientation;
                    k4abt_joint_confidence_level_t left_wrist_conf = body.skeleton.joints[WRIST_LEFT].confidence_level;

                    //left hand (PELVIS 기준)
                    //x : 카메라를 마주보고 왼쪽 +, 0, 오른쪽 +
                    //y : 카메라를 마주보고 위쪽 -, 0, 아래쪽 + << ???
                    //z : 카메라를 마주보고 앞쪽으로 갈수록 작아지고, 멀어질 수록 수가 커진다
                    k4a_float3_t left_hand_pos = body.skeleton.joints[HAND_LEFT].position;
                    k4a_quaternion_t left_hand_qua = body.skeleton.joints[HAND_LEFT].orientation;
                    k4abt_joint_confidence_level_t left_hand_conf = body.skeleton.joints[HAND_LEFT].confidence_level;

                    k4a_float3_t left_ankle_pos = body.skeleton.joints[ANKLE_LEFT].position;
                    k4a_quaternion_t left_ankle_qua = body.skeleton.joints[ANKLE_LEFT].orientation;
                    k4abt_joint_confidence_level_t left_ankle_conf = body.skeleton.joints[ANKLE_LEFT].confidence_level;

                    //////////////////////////////////////////////////// RIGHT ////////////////////////////////////////////////////
                    k4a_float3_t right_sholder_pos = body.skeleton.joints[SHOULDER_RIGHT].position;
                    k4a_quaternion_t right_sholder_qua = body.skeleton.joints[SHOULDER_RIGHT].orientation;
                    k4abt_joint_confidence_level_t right_sholder_conf = body.skeleton.joints[SHOULDER_RIGHT].confidence_level;

                    k4a_float3_t elbow_right_pos = body.skeleton.joints[ELBOW_RIGHT].position;
                    k4a_quaternion_t elbow_right_qua = body.skeleton.joints[ELBOW_RIGHT].orientation;
                    k4abt_joint_confidence_level_t elbow_right_conf = body.skeleton.joints[ELBOW_RIGHT].confidence_level;

                    k4a_float3_t right_wrist_pos = body.skeleton.joints[WRIST_RIGHT].position;
                    k4a_quaternion_t right_wrist_qua = body.skeleton.joints[WRIST_RIGHT].orientation;
                    k4abt_joint_confidence_level_t right_wrist_conf = body.skeleton.joints[WRIST_RIGHT].confidence_level;
                    
                    k4a_float3_t right_hand_pos = body.skeleton.joints[HAND_RIGHT].position;
                    k4a_quaternion_t right_hand_qua = body.skeleton.joints[HAND_RIGHT].orientation;
                    k4abt_joint_confidence_level_t right_hand_conf = body.skeleton.joints[HAND_RIGHT].confidence_level;

                    k4a_float3_t right_ankle_pos = body.skeleton.joints[ANKLE_RIGHT].position;
                    k4a_quaternion_t right_ankle_qua = body.skeleton.joints[ANKLE_RIGHT].orientation;
                    k4abt_joint_confidence_level_t right_ankle_conf = body.skeleton.joints[ANKLE_RIGHT].confidence_level;
#pragma endregion
                    //printf("hand : %f %f %f\n", left_hand_pos.xyz.x, left_hand_pos.xyz.y, left_hand_pos.xyz.z);  
                    string captured_time = stamp_time(2);
                    char temp_time[20];
                    strcpy(temp_time, captured_time.c_str());

                    //csv로 저장하기 위함
                    sprintf(csv_kinect_data, "%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                        temp_time,
                        neck_pos.xyz.x, neck_pos.xyz.y, neck_pos.xyz.z,
                        left_sholder_pos.xyz.x, left_sholder_pos.xyz.y, left_sholder_pos.xyz.z,
                        elbow_left_pos.xyz.x, elbow_left_pos.xyz.y, elbow_left_pos.xyz.z,
                        left_wrist_pos.xyz.x, left_wrist_pos.xyz.y, left_wrist_pos.xyz.z,
                        left_hand_pos.xyz.x, left_hand_pos.xyz.y, left_hand_pos.xyz.z,

                        right_sholder_pos.xyz.x, right_sholder_pos.xyz.y, right_sholder_pos.xyz.z,
                        elbow_right_pos.xyz.x, elbow_right_pos.xyz.y, elbow_right_pos.xyz.z,
                        right_wrist_pos.xyz.x, right_wrist_pos.xyz.y, right_wrist_pos.xyz.z,
                        right_hand_pos.xyz.x, right_hand_pos.xyz.y, right_hand_pos.xyz.z,
                        
                        pelvis_pos.xyz.x, pelvis_pos.xyz.y, pelvis_pos.xyz.z,
                        left_ankle_pos.xyz.x, left_ankle_pos.xyz.y, left_ankle_pos.xyz.z,
                        right_ankle_pos.xyz.x, right_ankle_pos.xyz.y, right_ankle_pos.xyz.z);
                    
                    //cout << csv_kinect_data;

                    write_file(csv_kinect_data);

                    //데이터 전송을 위해서 형 변환을 시켜줌 k4a_float3_t 에서 char로 변환
                    sprintf(sending_data, "kinect,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", 
                        neck_pos.xyz.x, neck_pos.xyz.y, neck_pos.xyz.z,
                        left_sholder_pos.xyz.x, left_sholder_pos.xyz.y, left_sholder_pos.xyz.z,
                        elbow_left_pos.xyz.x, elbow_left_pos.xyz.y, elbow_left_pos.xyz.z,
                        left_wrist_pos.xyz.x, left_wrist_pos.xyz.y, left_wrist_pos.xyz.z,
                        left_hand_pos.xyz.x, left_hand_pos.xyz.y, left_hand_pos.xyz.z,

                        right_sholder_pos.xyz.x, right_sholder_pos.xyz.y, right_sholder_pos.xyz.z,
                        elbow_right_pos.xyz.x, elbow_right_pos.xyz.y, elbow_right_pos.xyz.z,
                        right_wrist_pos.xyz.x, right_wrist_pos.xyz.y, right_wrist_pos.xyz.z,
                        right_hand_pos.xyz.x, right_hand_pos.xyz.y, right_hand_pos.xyz.z,
                        
                        pelvis_pos.xyz.x, pelvis_pos.xyz.y, pelvis_pos.xyz.z,
                        left_ankle_pos.xyz.x, left_ankle_pos.xyz.y, left_ankle_pos.xyz.z,
                        right_ankle_pos.xyz.x, right_ankle_pos.xyz.y, right_ankle_pos.xyz.z);

                    receive_k_data(sending_data);   //데이터 전송
                    
                }

                k4a::image body_index_map = body_frame.get_body_index_map(); //body index 저장, skeleton 아님

                uint8_t* body_buffer = body_index_map.get_buffer();

                int rows = body_index_map.get_height_pixels();
                int cols = body_index_map.get_width_pixels();

                cv::Mat body = cv::Mat(rows, cols, CV_8UC1, body_buffer);   //Mat 형식으로 이미지 변환

                cv::imshow("body", body);

                int key = cv::waitKey(1);
                if (key == 27)
                {
                    printf("key pressed\n");
                    cv::destroyWindow("body");
                    close_file();
                    break;
                }
                //printf("successfully captured\n");
                //printf("%d %d", rows, cols);

            }
            else
            {
                //  It should never hit timeout when K4A_WAIT_INFINITE is set.
                std::cout << "Error! Pop body frame result time out!" << std::endl;
                return 0;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Failed with exception:" << std::endl
                << "    " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
};

void thread_viewer()
{
    thread t3(start_client);
    thread t1(color_viewer);
    thread t2(body_viewer);
    
    t3.join();
    t1.join();
    t2.join();
};

void connect_cut() 
{
    if (device_count != 0)
    {
        //k4abt_tracker_shutdown(tracker);
        //k4abt_tracker_destroy(tracker);
        // Shut down the camera when finished with application logic
        k4a_device_stop_cameras(device);
        k4a_device_close(device);

        device_count = 0;
        device = NULL;
        printf("device released\n");
        return;
    }
    else 
    {
        printf("device is not connected yet!\n");
    }

}