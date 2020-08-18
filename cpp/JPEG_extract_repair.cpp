/*
Author: Charles Kinzel
Date:	06/19/2020
Title:	Challenge3.cpp
Desc.:	A program that extracts images in jpeg format from a binary file. Extracted jpegs 
		can have some non standard start byte, only jpegs with that start byte will be 
		extracted. Once extracted start bytes are repaired to FF D8 FF and are saved to a 
		directory dependent on the input file.
Input:	A file with start bytes of the jpegs to be extracted, a file with the jpegs
Output: Repaired images, standard output of extracted jpeg info
*/
#include <iostream>
#include <fstream>
#include <string>
#include <experimental/filesystem>
#include <bitset>
#include <openssl/md5.h>

namespace fs = std::experimental::filesystem;
using namespace std;

unsigned char *Crypt(unsigned char *data, int dataLength, unsigned int initialValue) {

	unsigned char* ret = new unsigned char[dataLength];
	unsigned int feed_back = 0x87654321;
	int steps = 8;
	for (int j = 0; j < dataLength; j++) {
		for (int i = 0; i < steps; i++) {
			// LSB is 0
			if (initialValue % 2 == 0) {
				initialValue = initialValue >> 1;
			}
			// LSB is 1
			else {
				initialValue = (initialValue >> 1) ^ feed_back;
			}
		}
		// 1 byte key
		int key = initialValue & 0xff;
		// obfuscation
		ret[j] = (key ^ *(data + j));
	}	
	return ret;
}

int kdbExtract(string file_path) {
	// Input file
	ifstream kdb_in(file_path, ios::in | ios::binary);
	// Output
	ofstream std_output;
	std_output.open("magicByte.txt", ios::trunc);
	// File open error	
	if (!kdb_in.is_open()) {
		cout << endl << "Error: File not found" << endl;
		return -2;
	}
	// Postion pointer after magic bytes
	kdb_in.seekg(6, ios::beg);
	// Position pointer at entry list
	uint32_t entry_list_ptr = 0;
	kdb_in.read(reinterpret_cast<char *>(&entry_list_ptr), sizeof(entry_list_ptr));
	kdb_in.seekg(entry_list_ptr, ios::beg);
	// Check for empty entry list	
	uint32_t end_of_list = 0;
	kdb_in.read(reinterpret_cast<char *>(&end_of_list), sizeof(end_of_list));
	if (end_of_list == 0xffffffff) {
		cout << "Entry list is empty." << endl;
		return -3;
	}
	// Extraction process
	while (end_of_list != 0xffffffff) {
		kdb_in.seekg(entry_list_ptr, ios::beg);
		// Get entry name
		char* entry_name = new char[16];
		kdb_in.read(entry_name, 16);
		// Write to file		
		std_output << entry_name << " - ";
		// Position pointer at block list and set entry list pointer
		uint32_t block_list_ptr = 0;
		kdb_in.read(reinterpret_cast<char *>(&block_list_ptr), sizeof(block_list_ptr));
		entry_list_ptr = kdb_in.tellg();
		kdb_in.seekg(block_list_ptr, ios::beg);
		// Check for empty block list	
		uint32_t end_of_block_list = 0;
		kdb_in.read(reinterpret_cast<char *>(&end_of_block_list), sizeof(end_of_block_list));
		if (end_of_block_list == 0xffffffff) {
			cout << "Block list is empty." << endl;
			continue;
		}
		// Extracting data from individual blocks
		while (end_of_block_list != 0xffffffff) {
			kdb_in.seekg(block_list_ptr, ios::beg);
			// Get block size and data ptr
			uint16_t block_size = 0;
			kdb_in.read(reinterpret_cast<char *>(&block_size), sizeof(block_size));
			uint32_t data_ptr = 0;
			kdb_in.read(reinterpret_cast<char *>(&data_ptr), sizeof(data_ptr));
			// Set block list pointer and position pointer at data
			block_list_ptr = kdb_in.tellg();
			kdb_in.seekg(data_ptr, ios::beg);
			// Get data
			unsigned char* data = new unsigned char[block_size];
			kdb_in.read((char*)data, block_size);
			// Decrypt data and write to file
			unsigned char* decode = new unsigned char[block_size];
			decode = Crypt(data, block_size, 0x4f574154);
			for (int i = 0; i < block_size; i++) {
				std_output << decode[i];
			}
			std_output << endl;
			// Set file pointer to the block list pointer
			// Check block_list_ptr for end of list 0xffffffff
			kdb_in.seekg(block_list_ptr, ios::beg);
			kdb_in.read(reinterpret_cast<char *>(&end_of_block_list), sizeof(end_of_block_list));
			// Garbage management
			delete[] decode;
			delete[] data;
		}
		// set file pointer to entry_list_pointer
		// check entry_list_ptr for end of list 0xfffffffff
		kdb_in.seekg(entry_list_ptr, ios::beg);
		kdb_in.read(reinterpret_cast<char *>(&end_of_list), sizeof(end_of_list));
		// Garbage management
		delete[] entry_name;
	}
	// File cleanup
	std_output.close();
	kdb_in.close();
	return 0;
}

int main(int argc, char* argv[]) {
	
	// Input file
	if (argc < 3) {
		cout << endl << "Usage: " << argv[0] << " <magic byte file>" << " <file to be carved>" << endl;
		return -1;
	}
	string magic_file_path = argv[1];
	string input_file_path = argv[2];

	// Extract magic bytes
	kdbExtract(argv[1]);
	ifstream magicByteFile("magicByte.txt", ios::in | ios::binary);
	if (!magicByteFile.is_open()) {
		cout << endl << "Error: File not found" << endl;
		return -2;
	}
	magicByteFile.seekg(8, ios::beg);
	string magicBytes;
	magicByteFile >> magicBytes;
	// Input file
	ifstream inputFile(argv[2], ios::in | ios::binary);
	if (!inputFile.is_open()) {
		cout << endl << "Error: File not found" << endl;
		return -2;
	}
	/*** Extract jpegs ***/
	// Create Directory
	fs::create_directory(input_file_path + "_Repaired/");
	string data_string((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
	// Declare File Pointers
	size_t jpeg_pos = 0, end_bytes = 0, file_size = 0;
	// Boolean for abscence of jpegs with magic byte header
	bool empty = true;
	int img_count = 1;
	// File for output of image info
	ofstream file_info;
	file_info.open("challenge_III_output.txt", ios::trunc);
	while (jpeg_pos != string::npos) {
		/** Search file for jpegs **/
		// if an image has not been found yet
		if(empty==true)
			jpeg_pos = data_string.find(magicBytes, jpeg_pos);
		// If at least one image has been found
		else
			jpeg_pos = data_string.find(magicBytes, jpeg_pos + 1);
		// Break out of the while loop at the end of the file
		if (jpeg_pos == string::npos) {
			continue;
		}
		// An image has been found
		empty = false;
		end_bytes = data_string.find("ÿÙ", jpeg_pos); // Find the end byte
		file_size = (end_bytes - jpeg_pos) + 2; // Calculate the file size
		// Extract individual image and repair magic bytes
		unsigned char* img = new unsigned char[file_size];
		inputFile.seekg(jpeg_pos, ios::beg);
		inputFile.read((char*)img, file_size);
		img[0] = 0xff;
		img[1] = 0xd8;
		img[2] = 0xff;
		// Save image to file
		ofstream saved_jpeg;
		saved_jpeg.open(input_file_path + "_Repaired/" + to_string(jpeg_pos) + ".jpeg", ios::binary);
		saved_jpeg.write((char*)img, file_size);
		// Compute MD5 hash
		int md5_hash_length = 16;
		unsigned char* md5_hash = new unsigned char[md5_hash_length];
		MD5(img, file_size, md5_hash);
		// Write to file image properties
		file_info << "Image " << img_count << ": " << endl;
		file_info << "Offset: " << dec << jpeg_pos << endl;
		file_info << "Size: " << file_size << " bytes" << endl;
		file_info << "MD5 Hash: ";
		for (int i = 0; i < md5_hash_length; i++) {
			bitset<8> hex_md5_hash(md5_hash[i]);
			file_info << hex << hex_md5_hash.to_ulong();
		}
		file_info << endl << "Path: " << input_file_path + "_Repaired/" + to_string(jpeg_pos) + ".jpeg" << endl;
		file_info << endl;
		// Cleanup
		delete[] md5_hash;
		delete[] img;
		saved_jpeg.close();
		img_count++;		
	}
	// Empty file check
	if (empty == true) {
		cout << "No " << magicBytes << " jpegs found." << endl;
		return -4;
	}
	// Cleanup
	file_info.close();
	inputFile.close();
	magicByteFile.close();

	return 0;
}
