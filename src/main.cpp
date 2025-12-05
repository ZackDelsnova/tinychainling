#include <iostream>
#include <string>
#include <ctime>

#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>

const int difficulty = 4;

// small sha-256
class SimpleSHA256 {
public:
	static std::string hash(const std::string& input) {
		// readable and simplified SHA256 implementation
		// not 100% optimal

		// constants (first 32 bits of fractional part of cube root of primes)
		static const uint32_t k[64] = {
			0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
			0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
			0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
			0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
			0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
			0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
			0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
			0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
		};

		// inmitialize hash values (first 32 bit of fractional part of primes)
		uint32_t h[8] = {
			0x6a09e667,
			0xbb67ae85,
			0x3c6ef372,
			0xa54ff53a,
			0x510e527f,
			0x9b05688c,
			0x1f83d9ab,
			0x5be0cd19
		};
		
		// padding
		std::vector<uint8_t> msg(input.begin(), input.end());
		size_t originalLength = msg.size();

		// append '1' bit
		msg.push_back(0x80);

		// pad with 0s until length = 56 (mod 64)
		while ((msg.size() % 64) != 56) {
			msg.push_back(0x00);
		}

		// append original length in bits
		uint64_t bitLen = originalLength * 8;
		for (int i = 7; i >= 0; i--) {
			msg.push_back((bitLen >> (i * 8)) & 0xff);
		}

		// process 512 bit chunks
		for (size_t chunk = 0; chunk < msg.size(); chunk += 64) {
			uint32_t w[64];

			// break chunk 32 bit words
			for (int i = 0; i < 16; i++) {
				w[i] = (msg[chunk + i * 4] << 24) |
					(msg[chunk + i * 4 + 1] << 16) |
					(msg[chunk + i * 4 + 2] << 8) |
					(msg[chunk + i * 4 + 3]);
			}

			// extend first 16 words into 64
			for (int i = 16; i < 64; i++) {
				uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
				uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
				w[i] = w[i - 16] + s0 + w[i - 7] + s1;
			}

			// working variables
			uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];

			// main loop
			for (int i = 0; i < 64; i++) {
				uint32_t s1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
				uint32_t ch = (e & f) ^ ((~e) & g);
				uint32_t temp1 = hh + s1 + ch + k[i] + w[i];
				uint32_t s0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
				uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
				uint32_t temp2 = s0 + maj;

				hh = g;
				g = f;
				f = e;
				e = d + temp1;
				d = c;
				c = b;
				b = a;
				a = temp1 + temp2;
			}

			// add chunk result to hash values
			h[0] += a; h[1] += b; h[2] += c; h[3] += d;
			h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
		}

		std::stringstream ss;
		for (int i = 0; i < 8; i++) {
			ss << std::hex << std::setw(8) << std::setfill('0') << h[i];
		}

		return ss.str();
	}

private:
	static uint32_t rotr(uint32_t x, uint32_t n) {
		return (x >> n) | (x << (32 - n));
	}
};

struct Block {
	int index; // index of the block as its name suggests
	std::time_t timestamp; // time of creation
	std::string data; // data
	std::string prevHash; // hash of previous block
	std::string hash; // hash of this block
	int nonce = 0; // number used for mining

	// block initialization
	Block(int idx, const std::string& d, const std::string& prev) 
		: index(idx), timestamp(std::time(nullptr)), data(d), prevHash(prev), nonce(0) {
		hash = calculateHash();
		mineBlock();
	}

	// calculate hash
	std::string calculateHash() const {
		std::stringstream ss;
		ss << index << timestamp << data << prevHash << nonce;
		return SimpleSHA256::hash(ss.str());
	}

	// print block info
	void print() const {
		std::cout << "block " << index << "\n";
		std::cout << "timestamp: " << timestamp << "\n";
		std::cout << "data: " << data << "\n";
		std::cout << "prev hash: " << prevHash << "\n";
		std::cout << "hash: " << hash << "\n";
		std::cout << "-----------------------------\n\n";
	}

	void mineBlock() {
		while (hash.substr(0, difficulty) != std::string(difficulty, '0')) {
			nonce++;
			hash = calculateHash();
		}
	}
};

bool isHashValid(const Block& block) {
	return block.calculateHash() == block.hash;
}

class Blockchain {
public:
	Blockchain() {
		chain.push_back(createGenesisBlock());
	}

	// genesis / first block creation
	Block createGenesisBlock() {
		return Block(0, "GENESIS BLOCK", "0");
	}

	// get last block
	const Block& getLastBlock() const {
		return chain.back();
	}

	// add block
	void addBlock(const std::string& data) {
		Block last = getLastBlock();
		Block newBlock(
			last.index + 1,
			data,
			last.hash
		);
		chain.push_back(newBlock);
	}

	bool isChainValid() const {
		for (size_t i = 1; i < chain.size(); ++i) {
			const Block& current = chain[i];
			const Block& previous = chain[i - 1];

			if (!isHashValid(current))
				return false;

			if (current.prevHash != previous.hash)
				return false;
		}

		return true;
	}

	void printChain() const {
		for (const auto& block : chain) {
			block.print();
		}
	}

	void tamperBlock(int index, const std::string& newData) {
		if (index <= 0 || index >= (int)chain.size()) {
			std::cout << "invalid index, cant tamper with genesis block :(\n";
			return;
		}

		Block& blk = chain[index];
		blk.data = newData;

		blk.hash = blk.calculateHash();

		std::cout << "block " << index << " has been tampered >:)\n";
		std::cout << "new hash (unmined): " << blk.hash << "\n";
	}

	void repairChain() {
		std::cout << "\nstarting chain repair.....\n";

		for (size_t i = 1; i < chain.size(); i++) {
			Block& current = chain[i];
			Block& previous = chain[i - 1];

			// if link broken or hash doesnt satisfy mining rule
			if (current.prevHash != previous.hash ||
				current.hash.substr(0, difficulty) != std::string(difficulty, '0')) {
				
				std::cout << "\nrepairing block " << i << "......\n";

				//update previous hash
				current.prevHash = previous.hash;
				
				// remine block
				current.nonce = 0;
				current.hash = current.calculateHash();

				std::cout << "mining block " << i << ".......\n";
				current.mineBlock();

				std::cout << "block " << i << " fixed\n";
			}
		}

		std::cout << "\n chain fully repaired :) \n";
	}

private:
	std::vector<Block> chain;
};

int main() {
	
	std::cout << "tinychainling started....\n";

	Blockchain chain;
	int choice = 0;

	while (true) {
		std::cout << "1. add block\n";
		std::cout << "2. print blockchain\n";
		std::cout << "3. validate chain\n";
		std::cout << "4. exit\n";
		std::cout << "5. tamper with block\n";
		std::cout << "6. repair chain\n";
		std::cout << "choice: ";
		std::cin >> choice;

		if (choice == 1) {
			std::string data;
			std::cout << "enter data: ";
			std::cin.ignore();
			std::getline(std::cin, data);
			chain.addBlock(data);
		}
		else if (choice == 2) {
			chain.printChain();
		}
		else if (choice == 3) {
			if (chain.isChainValid()) std::cout << "chain VALID :)\n";
			else std::cout << "chain BROKEN :(\n";
		}
		else if (choice == 4) break;
		else if (choice == 5) {
			int index;
			std::string newData;

			std::cout << "enter block index to tamper with: ";
			std::cin >> index;
			std::cin.ignore();

			std::cout << "enter new FAKE data: ";
			std::getline(std::cin, newData);

			chain.tamperBlock(index, newData);
		}
		else if (choice == 6) {
			chain.repairChain();
		}
	}

	return 0;
}

