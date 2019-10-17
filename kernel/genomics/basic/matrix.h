#include <vector>

template <typename T>
class Matrix
{
public:
    Matrix(){};
    Matrix(int rows, int cols)
    {
        for(int i=0; i<rows; ++i){
            data_.push_back(std::vector<T>(cols));
        }
    }
    // other ctors ....
    std::vector<T> & operator[](int i)
    {
        return data_[i];
    }
    const std::vector<T> & operator[] (int i) const
    {
        return data_[i];
    }
    // other accessors, like at() ...

	// other member functions, like reserve()....

private:
    std::vector<std::vector<T> > data_;
    
};

/*
int main()
{
    dynamic_array<int> a(3, 3);
    a[1][1] = 2;
    int x = a[1][1];
    return 0;
}*/ 
