
#ifndef VZMATRIX_H
#define VZMATRIX_H

#include <vector>


class VzMatrix
{
public:
    VzMatrix(short nrows, short ncols);
    VzMatrix(const CSize& sizee);
    ~VzMatrix();

    short& at(short row, short col);
    const short& at(short row, short col) const;
    short& at(const POINTS& poshort);
    const short& at(const POINTS& poshort) const;

    CSize size() const;
    short rows() const;
    short cols() const;

    bool valid(const POINTS& poshort) const;

private:
    short nrows;
    short ncols;
	std::vector<short> data;
};


#endif // VZMATRIX_H
