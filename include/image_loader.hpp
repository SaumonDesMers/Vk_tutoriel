#ifndef IMAGE_LOADER_HPP
#define IMAGE_LOADER_HPP

#include <string>
#include <fstream>
#include <iostream>

enum class ImageFormat {
	PPM_P6,
	Unsupported
};

class ImageLoader {

public:

	/*
	 * The image is loaded into a buffer of size width * height * 4 (RGBA).
	 */
	uint8_t *loadImage(const std::string& path, int *width, int *height) {
		this->openFile(path);
		this->getImageFormat(path);
		uint8_t *buffer = this->readImage(width, height);
		this->closeFile();
		return buffer;
	}

	void freeImage(uint8_t *buffer) {
		delete[] buffer;
	}

private:

	std::ifstream file;
	ImageFormat imageFormat;

	void openFile(const std::string& path) {
		this->file.open(path);
		if (!this->file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
	}

	void closeFile() {
		this->file.close();
	}

	void getImageFormat(const std::string& path) {

		size_t dotPos = path.find_last_of(".");
		if (dotPos == std::string::npos) {
			this->imageFormat = ImageFormat::Unsupported;
			return;
		}

		std::string extension = path.substr(path.find_last_of(".") + 1);

		if (extension == "ppm") {
			std::string line;
			std::getline(file, line);
			if (line == "P6") {
				this->imageFormat = ImageFormat::PPM_P6;
				return;
			}
		}

		this->imageFormat = ImageFormat::Unsupported;
		return;
	}

	uint8_t *readImage(int *width, int *height) {

		switch (this->imageFormat) {
			case ImageFormat::PPM_P6:
				return this->readPPM_P6(width, height);
			default:
				throw std::runtime_error("unsupported image format!");
		}
		return nullptr;
	}

	uint8_t *readPPM_P6(int *width, int *height) {

		std::string line;

		this->file.seekg(0);
		std::getline(this->file, line); /* Skip the first line */

		/* Read the width and height */
		std::getline(this->file, line);
		size_t spacePos = line.find(" ");
		int w = std::stoi(line.substr(0, spacePos));
		int h = std::stoi(line.substr(spacePos + 1));
		*width = w;
		*height = h;

		/* Read the maximum value */
		std::getline(this->file, line);
		size_t maxValue = std::stoi(line);
		size_t bytesPerPixel = maxValue < 256 ? 1 : 2;

		/* TODO: Support 16-bit PPM images */
		if (bytesPerPixel == 2) {
			throw std::runtime_error("16-bit PPM images are not supported!");
		}

		size_t bufferSize = w * h * 4;
		uint8_t *buffer = new uint8_t[bufferSize];

		for (size_t i = 0; i < bufferSize; i += 4) {
			buffer[i] = this->file.get();
			buffer[i + 1] = this->file.get();
			buffer[i + 2] = this->file.get();
			buffer[i + 3] = 255;
		}

		return buffer;
	}

};

#endif // IMAGE_LOADER_HPP