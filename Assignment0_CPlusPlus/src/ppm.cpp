#include <iostream>
#include <fstream>
#include "PPM.h"

// Constructor loads a filename with the .ppm extension
PPM::PPM(std::string fileName){
	// Set m_PixelData to nullptr so destructor does not contain undefined behavior if constructor throws exception
	m_PixelData = nullptr;
	
	// Check that we have been given a .ppm file
	if (!isPPMFile(fileName)) {
		std::cerr << "Expected a .ppm file for constructing a PPM object" << std::endl;
		throw std::invalid_argument("Invalid argument: expected .ppm file");
		return;
	}
	
	// Open the file
	std::ifstream infile(fileName);
	
	// Read the pixel data
	if (infile.is_open()) {
		// String to read file data into
		std::string line;
		
		// Skip header line
		std::getline(infile, line);
		
		// Skip 0 or more comments
		do {
			std::getline(infile, line);
		} while(line[0] == '#');
		
		// Get width and height of image
		char* cstr = new char[line.length() + 1];
		std::strcpy(cstr, line.c_str());
		m_width = std::atoi(std::strtok(cstr, " \n"));
		m_height = std::atoi(std::strtok(nullptr, " \n"));
		delete[] cstr;
		
		// Initialize char array for pixel data
		m_PixelData = new unsigned char[m_width * m_height * 3];
		
		// Get value to scale colors by
		std::getline(infile, line);
		float colorScale = 255 / std::stof(line);
		
		int index = 0;
		// Process pixel data
		while(std::getline(infile, line)) {
			char* cstr = new char[line.length() + 1];
			std::strcpy(cstr, line.c_str());
			
			char* token = std::strtok(cstr, " \n");
			while(token != nullptr) {
				m_PixelData[index] = (unsigned char)(colorScale * std::atoi(token));
				token = std::strtok(nullptr, " \n");
				index++;
			}
			delete[] cstr;
		}
	}
	
	// Close the file
	infile.close();
}

// Destructor clears any memory that has been allocated
PPM::~PPM(){
	// Free char array
	delete[] m_PixelData;
}

// Saves a PPM Image to a new file.
void PPM::savePPM(std::string outputFileName){
	// Check that we have been given a .ppm filename
	if (!isPPMFile(outputFileName)) {
		std::cerr << "Expected a .ppm filename to save to" << std::endl;
		throw std::invalid_argument("Invalid argument: expected .ppm file");
		return;
	}
	
	// Open a file
	std::ofstream outfile(outputFileName.c_str());
	if(outfile.is_open()){
		std::FILE* fp = std::fopen(outputFileName.c_str(), "w+");
		
		// Write header, width/height, and color scale
		std::fputs("P3\n", fp);
		std::fprintf(fp, "%d %d\n", m_width, m_height);
		std::fputs("255\n",fp);
		
		for(int ii = 0; ii < m_width * m_height * 3; ++ii){
			std::fprintf(fp, "%d", m_PixelData[ii]);
			std::fputs(" ",fp);
			std::fputs("\n",fp);
		}
		std::fclose(fp);
		outfile.close();
	}
}

// Darken subtracts 50 from each of the red, green
// and blue color components of all of the pixels
// in the PPM. Note that no values may be less than
// 0 in a ppm.
void PPM::darken(){
	int values = m_width * m_height * 3;
	for (int ii = 0; ii < values; ++ii) {
		m_PixelData[ii] = (unsigned char)clampRGB((int)m_PixelData[ii] - 50);
	}
}

// Sets a pixel to a specific R,G,B value
void PPM::setPixel(int x, int y, int R, int G, int B){
	// Clamp RGB values to 0-255 range
	int vals[3] = { R, G, B };
	for (int& val : vals) {
		val = clampRGB(val);
	}
	
	// Convert coordinates to index in pixel data
	int pixelStart = (x + y * m_width) * 3;
	
	// Set values at index to clamped given RGB values
	for (int ii = 0; ii < 3; ++ii) {
		m_PixelData[pixelStart + ii] = (unsigned char)vals[ii];
	}
}

bool PPM::isPPMFile(std::string fileName) {
	// Find last occurence of a period in the string, so we can get the file extension
	std::size_t dot = fileName.rfind(".");
	
	// If filename has a dot, check if extension is ppm
	if (dot != std::string::npos) {
		return fileName.substr(dot + 1) == "ppm";
	}
	return false;
}

int PPM::clampRGB(int val) {
	return val < 0 ? 0 : (val > 255 ? 255 : val);
}
