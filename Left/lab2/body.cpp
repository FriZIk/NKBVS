/*
Создать класс Triangle(треугольник), задав в нем длиныдвух сторони угол между ними. 
Реализовать методы вычисления площади и периметра. На его основе создать классы, 
описывающие  равносторонний,  равнобедренный  и  прямоугольный треугольники с 
присущими им методами вычисления площади и периметра.  
*/

#include <iostream>
#include <math.h>
#include <vld.h>

using std::cin;
using std::cout;

// Базовый абстрактный класс
class Triangle
{
    /*Две стороны и угол между ними*/
    protected:
        int sideA; 
        int sideB; 
        int angle;
    public:
        Triangle() = default;
        Triangle(int aSideA, int aSideB, int aAngle)
        {
            int sideA = aSideA;
            int sideB = aSideB;
            int angle = aAngle;
        }

    // Виртуальные функции для площади и периметра
    virtual void getArea() = 0; // Было бы default, класс был бы не абстрактным
    virtual void getPerimeter() = 0; 

    // Вирутальный деструктор
    virtual ~Triangle() = default;
};

// Производный класс(равносторонний)
class Equilateral : public Triangle
{
    public:
        // Вот тут не понял
        Equilateral(int aSideA, int aSideB, int aAngle)
        :Triangle(aSideA,aSideB,aAngle)
        {}
    void getArea() override
    {
        cout<<"Площадь треугольника:"<<(sqtr(3)/4)*(a*a);
    }
    void getPerimeter() override
    {
        cout<<"Периметр треугольника:"<<a*3;
    }
};

// Производный класс(равнобедренный)
class Isosceles : public Triangle
{
    public:
        // Вот тут не понял
        Isosceles(int aSideA, int bSideB, int aAngle)
        :Triangle(aSideA,aSideB,aAngle){}

    void getArea() override
    {
        cout<<"Площадь треугольника:"<<(b*sqrt((a*a) - ((b*b)/4)))/2;
    }
    void getPerimeter() override
    {
        // P = a*2 + b
        cout<<"Периметр треугольника:"<<a*2 + b;
    }
};

// Производный класс(прямоугольный)
class Isosceles : public Triangle
{
    public:
        // Вот тут не понял
        Isosceles(int aSideA, int bSideB, int aAngle)
        :Triangle(aSideA,aSideB,aAngle){}

    void getArea() override
    {
        cout<<"Площадь треугольника:"<<(a*b)/2;
    }
    void getPerimeter() override
    {
        cout<<"Периметр треугольника:"<<a*2 + b;
    }
};

// Добавить описание других ситуаций для каждой из функций

int main()
{
    setlocale(LC_ALL,"Russian");
    Triangle* v = new 
}