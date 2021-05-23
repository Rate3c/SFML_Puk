/*Работу выполнил студент гр.0301 Нестеров Евгений
* ЗАДАНИЕ:
Написать программу – графический редактор, позволяющую автоматизировать процесс создания вышивки сначала на экране компьютера,
а затем переноса данных в виде последовательности координат стежков в автоматическую швейную машину в качестве программы управления.
Пользователь с помощью курсора выбирает нужную точку экрана и нажимает «Enter» или левую кнопку мыши, 
фиксируя координату (0,0) начала рисунка в декартовой системе координат. Для задания очередного стежка курсор перемещается в другую точку, но не дальше,
чем на максимальную длину стежка, и т. д., образуя узор вышивки. Окончание ввода рисунка производится по нажатию клавиши «Esc».
*/

#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace sf;
using namespace std;

vector<sf::Shape*> lines;
vector<sf::Shape*> stezhok;
vector<double> Xcords; //вектор х координат
vector<double> Ycords; //вектор у координат
vector<double> Lenghts = { 0 }; //длины стежков

bool Is_empty(std::ifstream& pFile) {
    return pFile.peek() == std::ifstream::traits_type::eof();
}

void Exit(vector <double> Xcords, vector <double> Ycords) {
    cout << endl;
    for (int i = 0; i < Xcords.size(); i++)
    {
        cout << Xcords[i] << " " << Ycords[i] << " ; ";
        cout << "Lenght of stitch " << i << " is " << (int)Lenghts[i] << " mm" << endl;
    }

    cout << endl << "Do you want to save coordinates in config? Press \"1\" for YES or \"2\" for NO. ";
    int action;
    cin >> action;
    if (action == 1) {
        string cfg = "config.txt";
        fstream outconfig;
        outconfig.open(cfg, ios::out);
        for (int i = 0; i < Xcords.size(); i++)
        {
            outconfig << Xcords[i] << " " << Ycords[i] << endl; // вывод координат в файл
        }
        outconfig.close();
    }

    cout << endl << "Program complete." << endl;
}

//окружность для нагдлялной длины стежка
void StezhokLenght(int x, int y, double lengh) {
    CircleShape* stezh = new CircleShape(lengh); 
    stezh->setFillColor(Color(0, 0, 0));
    stezh->setOutlineThickness(1);
    stezh->setOutlineColor(sf::Color(255, 255, 255, 20));
    stezh->move(x - lengh, y - lengh);
    stezhok.push_back(stezh);
}

// нарисовать линию(стежок)
void DrawLine(int x, int y, double lengh, double ugol) {
    RectangleShape* myline = new RectangleShape(Vector2f(lengh, 1.f));
    myline->rotate(ugol);
    myline->setFillColor(Color(15, 180, 140));
    myline->move(x, y);
    lines.push_back(myline);
}

void DrawText(unsigned int shirina/*, int offset*/) {
    sf::Text dxdy;
    sf::Font font;
    font.loadFromFile("arial.ttf");
    dxdy.setFont(font);
    dxdy.setString("Offset by dx = 123");
    dxdy.setCharacterSize(20);
    dxdy.setFillColor(sf::Color::White);
    dxdy.setStyle(sf::Text::Bold);
    dxdy.move(shirina - 250, 0);
}

int main()
{
    double alpha;
    int stezhokL = 200;
    int firsttap = 0;
    double degree = 180.0 / 3.141592;
    RenderWindow window(VideoMode(1600, 900), "SFML2.0");
    window.setFramerateLimit(30);
    sf::Vector2u size = window.getSize();
    unsigned int width = size.x;
    unsigned int height = size.y;

    int choice;
    cout << "Would you like to use your config(1) or draw new(2)? ";
    cin >> choice;
    if (choice == 1) {
        string cfg = "config.txt";
        ifstream incfg;
        incfg.open(cfg);
        if (!incfg.is_open()) {
            cout << "Openning file error." << endl;
        }
        else {
            cout << "Config opened." << endl;
            int I;

            if (!Is_empty(incfg)) {

                for (int i = 0; !incfg.eof(); ++i)
                {
                    if (i % 2 != 0) {
                        incfg >> I;
                        Ycords.push_back(I);
                    }
                    else if (i % 2 == 0) {
                        incfg >> I;
                        Xcords.push_back(I);
                    }
                }
                Xcords.erase(Xcords.end() - 1);

                for (firsttap; firsttap < Xcords.size(); firsttap++) {

                    StezhokLenght(Xcords[firsttap], Ycords[firsttap], stezhokL);
                    if (firsttap != 0) {
                        double lenght = sqrt(pow(Xcords[firsttap] - Xcords[firsttap - 1], 2) + pow(Ycords[firsttap] - Ycords[firsttap - 1], 2));

                        if (Xcords[firsttap] - Xcords[firsttap - 1] < 0) {
                            alpha = atan((Ycords[firsttap] - Ycords[firsttap - 1]) / (Xcords[firsttap] - Xcords[firsttap - 1])) * degree;
                        }
                        else {
                            alpha = 180 + atan((Ycords[firsttap] - Ycords[firsttap - 1]) / (Xcords[firsttap] - Xcords[firsttap - 1])) * degree;
                        }

                        Lenghts.push_back(lenght);

                        DrawLine(Xcords[firsttap], Ycords[firsttap], lenght, alpha);
                    }
                }
            }
            else cout << "Config is empty." << endl;
        }
        incfg.close();
    }

    // главный цикл
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
            {
                window.close();
                return 0;
            }
            //регистрация нажатия
            case sf::Event::MouseButtonPressed: {
                // считывание первой точки без рисования 
                if (firsttap == 0) {
                    Vector2i position = Mouse::getPosition(window);
                    Xcords.push_back(position.x);
                    Ycords.push_back(position.y);
                    firsttap++;

                    StezhokLenght(event.mouseButton.x, event.mouseButton.y, stezhokL);
                }
                // отрисовка линий 
                else {

                    Xcords.push_back(event.mouseButton.x);
                    Ycords.push_back(event.mouseButton.y);
                    firsttap++;
                    double lenght = sqrt(pow(Xcords[firsttap - 1] - Xcords[firsttap - 2], 2) + pow(Ycords[firsttap - 1] - Ycords[firsttap - 2], 2)); // длина линии
                    // вычисление угла поворота линии 
                    if (lenght <= stezhokL) {
                        StezhokLenght(event.mouseButton.x, event.mouseButton.y, stezhokL); // окружность для наглядности
                        Lenghts.push_back(lenght);

                        if (Xcords[firsttap - 1] - Xcords[firsttap - 2] < 0) {
                            alpha = atan((Ycords[firsttap - 1] - Ycords[firsttap - 2]) / (Xcords[firsttap - 1] - Xcords[firsttap - 2])) * degree;
                        }
                        else {
                            alpha = 180 + atan((Ycords[firsttap - 1] - Ycords[firsttap - 2]) / (Xcords[firsttap - 1] - Xcords[firsttap - 2])) * degree;
                        }

                        DrawLine(event.mouseButton.x, event.mouseButton.y, lenght, alpha);
                    }
                    else {
                        cout << "Wrong stitch lenght!" << endl;
                        Xcords.erase(Xcords.end() - 1);
                        Ycords.erase(Ycords.end() - 1);
                        firsttap--;
                    }
                }
            }
            }
        }

        //backspace чтобы удалить линию
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))&&(lines.size()>0))
        {
            stezhok.erase(stezhok.end() - 1);
            lines.erase(lines.end()-1);
            Xcords.erase(Xcords.end()-1);
            Ycords.erase(Ycords.end()-1);
            Lenghts.erase(Lenghts.end()-1);
            window.display();
            firsttap--;
        }

        window.clear();

        if (Xcords.size() > 0) {
            Vector2i position = Mouse::getPosition(window);

            ostringstream offsetDX; //offset by dx
            ostringstream offsetDY; //offset by dy
            ostringstream laststitch; // lenght of the last stitch

            offsetDX << abs(position.x - Xcords[Xcords.size() - 1]);
            offsetDY << abs(position.y - Ycords[Ycords.size() - 1]);
            laststitch << (int)Lenghts[Lenghts.size() - 1];

            sf::Text dxdy;
            sf::Font font;
            font.loadFromFile("arial.ttf");
            dxdy.setFont(font);
            dxdy.setString("OFFSET BY dx = " + offsetDX.str() + "\n" + "OFFSET by dy = " + offsetDY.str() + "\n" + "Lenght of the last stitch = " + laststitch.str());
            dxdy.setCharacterSize(17);
            dxdy.setFillColor(sf::Color::White);
            dxdy.setStyle(sf::Text::Bold);
            dxdy.move(width - 300, 0);
            window.draw(dxdy);
        }


        for (auto it1 = stezhok.begin(); it1 != stezhok.end(); it1++)
        {
            window.draw(**it1);
        }

        for (auto it = lines.begin(); it != lines.end(); it++)
        {
            window.draw(**it);
        }

        window.display();

        fstream cords;
        cords.open("cords.txt", ios::out);
        cords << "Координаты узлов: " << endl;
        for (int  i = 0; i < Xcords.size();  i++)
        {
            cords << i+1 << " - (" << Xcords[i] << "; " << Ycords[i] << ") "; // вывод координат в файл
            cords << "Длина " << i << " стежка = " << (int)Lenghts[i] <<"mm" << endl;
        }
        cords.close(); 

        //при нажатии Escape выход из программы
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            Exit(Xcords, Ycords); return 0;
        }
    }
    return 0;
}