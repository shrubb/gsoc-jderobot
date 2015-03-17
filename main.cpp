/* The problem seems not to have a polynomial solution :(  */
/* (of course, provided that there can be "wide" paths)    */
/* So this code (bruteforce) is extremely inefficient      */
/*                                                         */
/* The implementation can be made faster in pure C by      */
/* reducing memory allocations, pushbacks & popbacks, etc. */

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <fstream>

struct Coord
{
    size_t row, col;

    Coord() {}

    Coord(size_t _row, size_t _col): row(_row), col(_col) {}

    const Coord operator+(const std::pair<int, int> & other) const
    {
        return Coord(this->row + other.first, this->col + other.second);
    }
};

int numberOfHoles = 0;
std::vector< Coord > answer;
std::vector< std::string > table;
std::vector< Coord > currentPath; // actually a stack; made "vector" to enable "good" copying
size_t rows, cols;

inline const bool isValid(const Coord & x)
{
    return
        x.row >= 0 && x.row < rows &&
        x.col >= 0 && x.col < cols;
}

inline const char get(const Coord & x)
{
    return isValid(x) ? table[x.row][x.col] : 0;
}

std::pair<int, int> trans[4] = {
        std::make_pair(1, 0),
        std::make_pair(0, -1),
        std::make_pair(-1, 0),
        std::make_pair(0, 1) };

void DFS()
{
    table[currentPath.back().row][currentPath.back().col] = 'X';
    for (int i = 0; i < 4; ++i)
    {
        Coord temp = currentPath.back() + trans[i];
        if (!isValid(temp) || get(temp) != '.')
            continue;

        // remember current path in a stack
        currentPath.push_back(temp);

        DFS();
        // the answer can't be greater than the number of holes
        if (answer.size() == numberOfHoles)
            return;

        currentPath.pop_back();
    }
    table[currentPath.back().row][currentPath.back().col] = '.';

    // update the answer
    if (currentPath.size() > answer.size())
    {
        answer.resize(currentPath.size());
        std::copy(currentPath.begin(), currentPath.end(), answer.begin());
    }
}

inline char numberOfMove(int i)
{
    int k = i % (10 + 26 + 26);

    if (k < 10)
        return '0' + k;
    else if (k < 36)
        return 'a' + (k - 10);
    else
        return 'A' + (k - 10 - 26);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Argument #1 should be input file" << std::endl;
        return 0;
    }

    std::ifstream in(argv[1]);
    if (in.fail())
    {
        std::cout << "Error opening file" << std::endl;
        return 0;
    }

    std::string s;
    while (in >> s)
    {
        numberOfHoles += std::count(s.begin(), s.end(), '.');
        table.push_back(s);
    }
    in.close();

    rows = table.size();
    cols = table[0].size();
    Coord current;

    // DFS from all the holes near the walls
    for (current.row = 0; current.row < rows; ++current.row)
    {
        for (current.col = 0; current.col < cols; ++current.col)
        {
            // start only from holes, obviously
            if (get(current) != '.')
                continue;

            bool isSide = false;
            for (int i = 0; i < 4; ++i)
            {
                if (get(current + trans[i]) != '.')
                {
                    isSide = true;
                    break;
                }
            }

            // no need to check inner holes
            if (!isSide)
                continue;

            table[current.row][current.col] = 'X';
            currentPath.push_back(Coord(current.row, current.col));
            DFS();
            currentPath.pop_back();
            table[current.row][current.col] = '.';
        }
    }

    // prepare the answer for output
    for (int i = 0; i < answer.size(); ++i)
    {
        // will print 0 1 2 3 ... 9 a b c d ... y z A B C D ... Y Z 0 1 2 3 ...
        // instead of 0 1 2 3 ... 9 10 11 12 13 ...
        // (to maintain rows' widths)
        table[answer[i].row][answer[i].col] = numberOfMove(i);
    }

    std::ofstream out(argv[1]);
    out << answer.size() << std::endl;
    auto tableEnd = table.end();
    for (auto str = table.begin(); str != tableEnd; ++str)
    {
        out << *str << std::endl;
    }

    return 0;
}