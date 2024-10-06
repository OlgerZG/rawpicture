#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

// Define the resolution and RAW format (RAW8 or RAW10)
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define RAW_FORMAT 8  // Change to 8 for RAW8 format

void captureImage(cv::Mat& image) {
    cv::VideoCapture cap(0);  // Open default camera
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video capture." << std::endl;
        return;
    }

    cap >> image;  // Capture a frame
    cap.release(); // Release the camera
}

void saveRawImage(const cv::Mat& image, const std::string& filename) {
    // Create a RAW buffer
    std::vector<uint16_t> raw_buffer(IMAGE_WIDTH * IMAGE_HEIGHT * 3);  // 3 channels for RGB

    // Convert each pixel to RAW format
    for (int i = 0; i < IMAGE_HEIGHT; ++i) {
        for (int j = 0; j < IMAGE_WIDTH; ++j) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
            for (int channel = 0; channel < 3; ++channel) {
                if (RAW_FORMAT == 10) {
                    // Store each channel in the RAW10 buffer
                    uint16_t value = static_cast<uint16_t>((pixel[channel] * (1023.0 / 255.0)));  // Scale to 10 bits
                    raw_buffer[(i * IMAGE_WIDTH + j) * 3 + channel] = value & 0x3FF;  // Keep only lower 10 bits
                } else if (RAW_FORMAT == 8) {
                    // Store each channel in the RAW8 buffer
                    uint8_t value = static_cast<uint8_t>(pixel[channel]);  // Keep 8 bits
                    raw_buffer[(i * IMAGE_WIDTH + j) * 3 + channel] = value;  // Store directly
                }
            }
        }
    }

    // Save the raw image to a binary file
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(reinterpret_cast<char*>(raw_buffer.data()), raw_buffer.size() * sizeof(uint16_t));
    ofs.close();

    std::cout << "RAW" << RAW_FORMAT << " image saved as " << filename << std::endl;
}

cv::Mat loadRawImage(const std::string& filename, const cv::Size& size) {
    // Read the raw image data
    std::ifstream ifs(filename, std::ios::binary);
    std::vector<uint16_t> raw_data(size.width * size.height * 3);  // 3 channels for RGB
    ifs.read(reinterpret_cast<char*>(raw_data.data()), raw_data.size() * sizeof(uint16_t));
    ifs.close();

    // Create an RGB image
    cv::Mat rgb_image(size, CV_8UC3);
    for (int i = 0; i < size.height; ++i) {
        for (int j = 0; j < size.width; ++j) {
            for (int channel = 0; channel < 3; ++channel) {
                if (RAW_FORMAT == 10) {
                    uint16_t value = raw_data[(i * size.width + j) * 3 + channel] & 0x3FF;  // Keep only lower 10 bits
                    rgb_image.at<cv::Vec3b>(i, j)[channel] = static_cast<uint8_t>((value * 255) / 1023);  // Scale back to [0, 255]
                } else if (RAW_FORMAT == 8) {
                    uint8_t value = static_cast<uint8_t>(raw_data[(i * size.width + j) * 3 + channel]);  // Directly read 8 bits
                    rgb_image.at<cv::Vec3b>(i, j)[channel] = value;  // Set the pixel value for the channel
                }
            }
        }
    }

    return rgb_image;
}

int main() {
    cv::Mat image(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);  // Create a blank image with defined size
    captureImage(image);  // Capture an image from the camera

    if (!image.empty()) {
        // Save the image as RAW format
        saveRawImage(image, "image.raw" + std::to_string(RAW_FORMAT));

        // Load the RAW image back and convert to RGB
        cv::Mat rgb_image = loadRawImage("image.raw" + std::to_string(RAW_FORMAT), image.size());

        // Save the RGB image
        cv::imwrite("image_rgb.png", rgb_image);
        std::cout << "RGB image saved as image_rgb.png" << std::endl;
    }

    return 0;
}
