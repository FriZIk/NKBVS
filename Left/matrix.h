// matrix.h

/*
Определить класс вещественных матриц (Matrix) с методами, реализующими сложение и
умножение матриц, транспонирование.Размер матрицы задается при ее ксоздании
*/
#pragma once

class Matrix
{

private:
    int **matrix;
    int _rows,_columns;

public:
    // Конструкторы
    Matrix();
    Matrix(int, int);
    Matrix(const Matrix&);

    // Методы
    void setSize(); // Задать количество столбцов и строк
    void manualInput(); // Ввод с клавиатуры
    void randomGen(); // Заполнение матрицы случайными числами
    void transposition(); // Транспонирование матрицы
    void showMatrix(); // Вывод матрицы на экран

    // Перегрузка операций
    Matrix operator = (Matrix&);
    Matrix operator + (Matrix&);
    Matrix operator * (Matrix&);

    // Деструктор
    ~Matrix();
}
