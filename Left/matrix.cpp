#include <matrix.h>
#include <iostream>

#include <cstdlib> // для функций rand() и srand()
#include <ctime> // для функции time()

using namespace std;

// Определения конструкторов
Matrix::Matrix(){}

Matrix::Matrix(int rows, int col)
{
    this->_rows = rows;
    this->_columns = col;
}
Matrix::Matrix(const Matrix& c1)
{
    this->_rows = c1._rows;
    this->_columns = c1._columns;
}

// Деструктор
Matrix::~Matrix()
{
    for (int i = 0; i < _rows; i++)
        delete[] matrix[i];

    delete[] matrix;
}

// Определение методов
void Matrix::setSize()
{
    cout<<"Введите количество строк и столбцов:";
    cin>>_rows>>_columns;
}

void Matrix::manualInput()
{
    matrix = new int*[_rows]; // строки
    for (int i = 0; i < _rows; i++)
        matrix[i] = new int [_columns]; // столбцы
    
    cout<<"Введите матрицу:"<<endl;
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0;j < _columnes;j++)
        {
            cin>>matrix[i][j];
        }
        cout<<endl;
    }
}

void Matrix::randomGen()
{
    srand( time(0) );

    matrix = new int*[_rows]; // строки
    for (int i = 0; i < _rows; i++)
        matrix[i] = new int [_columns]; // столбцы    
    
    for (int i = 0; i < _rows; i++)
        for(int j = 0; j < _columnes; j++)
            matrix[i][j] = 1 + rand() % 10;
}

void Matrix::transposition()
{
    //Не уверен что так можно делать
    Matrix **tmp;
    tmp = this;
    for(int i = 0; i < _rows; i++)
    {
        for(int j = 0; j < _columns; j++)
            matrix[i][j] = tmp[j][i];
    }
    
}

void Matrix::showMatrix()const
{
    cout<<"Введённая матрица:";
    for(int i = 0; i < _rows; i++)
    {
        for(int j = 0; j < _columns; j++)
            cout<<matrix[i][j]<<" ";
        cout<<endl;
    }
}

// Перегрузка операций
Matrix& Matrix::operator *(Complex& t)
{
    Matrix tmp;

    return tmp;
}

Matrix& Matrix::operator +(Complex& t)
{
    Matrix tmp;

    return tmp;
}