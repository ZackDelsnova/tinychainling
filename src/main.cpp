#include <iostream>
#include <string>
#include <ctime>

struct Block {
	int index; // index of the block as its name suggests
	std::time_t timestamp; // time of creation
	std::string data; // data
	std::string prevHash; // hash of previous block
	std::string hash; // hash of this block
	int nonce; // number used for mining

	// block initialization
	Block(int idx, const std::string& d, const std::string& prev) 
		: index(idx), timestamp(std::time(nullptr)), data(d), prevHash(prev), hash(""), nonce(0) {}

	// print block info
	void print() const {
		std::cout << "Block " << index << " [\n";
		std::cout << "  Timestamp: " << timestamp << "\n";
		std::cout << "  Data: " << data << "\n";
		std::cout << "  PrevHash: " << prevHash << "\n";
		std::cout << "  Hash: " << hash << "\n";
		std::cout << "  Nonce: " << nonce << "\n]\n\n";
	}
};

int main() {
	
	std::cout << "tinychainling started....\n";

	Block genesis(0, "Hello Tinychainling!", "0"); // first block has prevHash = 0
	genesis.print();


	return 0;
}

