#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

namespace crk {
	const int COL = 5;
	const int ROW = 5;
}

vector<int> rec(const char puzzle[crk::ROW][crk::COL], const string word, int row, int col, int index, int lookrow, int lookcol) {
	vector<int> v;

	if (index == word.size()) {
			
		v.push_back(row);
		v.push_back(col);

		return v;
	}
	else if (word[index] == puzzle[row + lookrow][col + lookcol]) {
		v = rec(puzzle, word, row + lookrow, col + lookcol, index + 1, lookrow, lookcol);
	}
	else {
		v.push_back(row);
		v.push_back(col);
		return v;
	}

	return v;

}

vector<int> search(const char puzzle[crk::ROW][crk::COL], const string word, int row, int col, int index) {
	char look1 = puzzle[row - 1][col - 1];
	char look2 = puzzle[row - 1][col];
	char look3 = puzzle[row - 1][col + 1];
	char look4 = puzzle[row][col - 1];
	char look5 = puzzle[row][col + 1];
	char look6 = puzzle[row + 1][col - 1];
	char look7 = puzzle[row + 1][col];
	char look8 = puzzle[row + 1][col + 1];
	vector<int> c;

	if (word[index] == look1)
		c = rec(puzzle, word, row - 1, col - 1, index + 1, -1, -1);
	else if (word[index] == look2)
		c = rec(puzzle, word, row - 1, col, index + 1, -1, 0);
	else if (word[index] == look3)
		c = rec(puzzle, word, row - 1, col + 1, index + 1, -1, 1);
	else if (word[index] == look4)
		c = rec(puzzle, word, row, col - 1, index + 1, 0, -1);
	else if (word[index] == look5)
		c = rec(puzzle, word, row, col + 1, index + 1, 0, 1);
	else if (word[index] == look6)
		c = rec(puzzle, word, row + 1, col - 1, index + 1, 1, -1);
	else if (word[index] == look7)
		c = rec(puzzle, word, row + 1, col, index + 1, 1, 0);
	else if (word[index] == look8)
		c = rec(puzzle, word, row + 1, col + 1, index + 1, 1, 1);
	else {
		cout << "Nope" << endl;
		
		c.push_back(-5);
		c.push_back(-5);
		return c;
	}
	
	return c;

}



vector<int> solve(const char puzzle[crk::ROW][crk::COL], const string word) {

	int index = 0;
	vector<int> v;
	for (int i = 0; i < crk::ROW; i++) {
		for (int j = 0; j < crk::COL; j++) {
			if (puzzle[i][j] == word[0]) {
		
				v = search(puzzle, word, i, j, index + 1);
				v.push_back(i);
				v.push_back(j);

				return v;
			}
		}
	}
	
	return v;
}

int main() {

	const char page[crk::ROW][crk::COL] = { { 'Z', 'R', 'P', 'B', 'D' },{ 'N', 'S', 'J', 'L', 'A' },{ 'G', 'P', 'L', 'M', 'C' },
	{ 'O', 'F', 'I', 'N', 'D' },{ 'Q', 'W', 'Y', 'U', 'K' } };
	const string word = "FINK";

	vector<int> v = solve(page, word);

	if (v[1] < word.size())
		cout << "Not found!" << endl;
	else {
		cout << "Word found: [" << v[2] << "][" << v[3]
			<< "] to [" << v[0] << "][" << v[1] << "]" << endl;
	}

	
	if (getchar() == '\n')
		return 0;
}