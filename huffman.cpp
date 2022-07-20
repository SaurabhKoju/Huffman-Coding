#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>
#include <string>
#include <map>
#include "heap.h"

struct Node{
	unsigned char c;
	unsigned int frequency;
	Node* left;
	Node* right;
};

struct Tree {
	Node* head;
};

bool operator<(Tree a, Tree b){
	return a.head->frequency < b.head->frequency;
}

map<unsigned char, string> encode;

void dfs(Node* x, string encodingstring = "") {
	if (x->left == NULL) {
		encode[x->c] = encodingstring;
		return;
	}
	dfs(x->left, encodingstring + "0");
	dfs(x->right, encodingstring + "1");
}

Tree buildtree(unsigned int charfrequency[256]) {
	Heap<Tree> priorityqueue;
	for (int i = 0; i < 256; i++) {
		if (charfrequency[i] == 0)continue;
		Node* x = new Node{ unsigned char(i), charfrequency[i] };
		Tree y{ x };
		priorityqueue.insert(y);
	}
	Node* head;
	while (priorityqueue.size() > 1) {
		Tree l = priorityqueue.get_min();
		Tree r = priorityqueue.get_min();
		Node* head = new Node;
		head->left = l.head;
		head->right = r.head;
		head->frequency = l.head->frequency + r.head->frequency;
		Tree parent{ head };
		priorityqueue.insert(parent);
	}
	return priorityqueue.get_min();
}


void decompress(std::ifstream& input, std::ofstream& output) {
	unsigned int count;
	unsigned int charfrequency[256];

	input.read(reinterpret_cast<char*>(&count), 4);
	for (int i = 0; i < 256; i++) {
		input.read(reinterpret_cast<char*>(&charfrequency[i]), 4);
	}
	Node* decoder = buildtree(charfrequency).head;
	Node* current = decoder;
	char c;
	while (input.read(&c, 1) && count) {
		vector<bool> bits(8);
		int byte = unsigned char(c);
		
		for (int i = 0; i < 8; i++) {
			bits[7 - i] = byte % 2;
			byte /= 2;
		}
		for (bool bit : bits) {
			if (bit)current = current->right;
			else current = current->left;
			if (current->left == NULL) {
				char wr = current->c;
				output.write(&wr, 1);
				count--;
				if (!count)break;
				current = decoder;
			}
		}
	}
}


void compress(std::ifstream& input, std::ofstream& output, bool passtable = true) {
	unsigned int charfrequency[256] = { 0 };
	unsigned readsize = 0;
	char c;
	while (input.read(&c, 1)) {
		charfrequency[unsigned char(c)]++;
		readsize++;
	}

	dfs(buildtree(charfrequency).head, "");

	std::cout.precision(4);
	cout << setw(10) << "Character" << setw(32) << "Encoded string" << setw(20) << "Frequency" << std::endl << std::endl;

	for (auto p : encode) {
		std::cout << setw(10) << p.first << setw(32) << p.second << setw(20) << charfrequency[p.first] << std::endl;
	}

	input.clear();
	input.seekg(0, std::ios::beg);

	unsigned writesize = 0;
	if (passtable) {
		output.write(reinterpret_cast<char*>(&readsize), 4);
		writesize += 4;
		for (int i = 0; i < 256; i++) {
			output.write(reinterpret_cast<char*>(&charfrequency[i]), 4);
			writesize += 4;
		}
	}
	string bitstream = "";
	while (input.read(&c, 1)) {
		bitstream += encode[unsigned char(c)];
		if (bitstream.length() >= 8) {
			output.put(char(stoi(bitstream.substr(0, 8), nullptr, 2)));
			writesize++;
			bitstream = bitstream.substr(8, bitstream.length() - 8);
		}
	}
	if (bitstream.length() > 0) {
		bitstream += string(8 - bitstream.length(), '0');
		output.put(char(stoi(bitstream, nullptr, 2)));
		writesize++;
	}
	std::cout << "Input file size = " << readsize << " bytes" << std::endl;
	std::cout << "Output file size = " << writesize << " bytes" << std::endl;
	std::cout << "Compression ratio = " << float(writesize) / readsize << std::endl;
}


int main(int argc, char** argv){
	if (argc == 1) {
		std::ifstream input("novel.txt", std::ios::binary);
		std::ofstream output("output.txt", std::ios::binary);
		compress(input, output);
		input.close();
		output.close();
		std::ifstream cmpd("output.txt", std::ios::binary);
		std::ofstream original("original.txt", std::ios::binary);
		decompress(cmpd, original);
	}
	else {
		if (argc < 4) {
			std::cout << "Not enough arguments" << std::endl;
			return 1;
		}
		std::ifstream input(argv[2], std::ios::binary);
		if (!input) {
			std::cout << "Could not find the specified file" << std::endl;
			return 1;
		}
		std::ofstream output(argv[3], std::ios::binary);
		string command = argv[1];
		if (command == "compress") {
			if (argc == 5 && string(argv[4]) == "notable")compress(input, output, false);
			else compress(input, output);
		}
		else if (command == "decompress") decompress(input, output);
		else std::cout << "Could not recognize command" << std::endl;
	}
}
