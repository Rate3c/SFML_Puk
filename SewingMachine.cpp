/*Работу выполнил студент гр.0301 Нестеров Евгений
* ЗАДАНИЕ:
Написать программу – графический редактор, позволяющую автоматизировать процесс создания вышивки сначала на экране компьютера,
а затем переноса данных в виде последовательности координат стежков в автоматическую швейную машину в качестве программы управления.
Пользователь с помощью курсора выбирает нужную точку экрана и нажимает левую кнопку мыши, 
фиксируя координату (0,0) начала рисунка в декартовой системе координат. Для задания очередного стежка курсор перемещается в другую точку, но не дальше,
чем на максимальную длину стежка, и т. д., образуя узор вышивки. Окончание ввода рисунка производится по нажатию клавиши «Esc».
_________________________________________________________

BackSpace - delete previous stitch
Esc - exit
LeftMouseClick - new stitch
LAlt - drawwing assistant
Left Arrow - move to left
Right Arrow - move to right
Up Arrow - move to up
Down Arrow - move to down
_________________________________________________________
cords.txt - Файл с полученными коорлинатами 
config.txt - Файл-конфиг
*/

#pragma execution_character_set("utf-8")

#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>

using namespace sf;
using namespace std;

vector<double> Xcords; //вектор х координат
vector<double> Ycords; //вектор у координат
vector<double> Lenghts = { 0 }; //длины стежков

vector <Shape*> marks2;
vector <Shape*> marks1;

void Markings(unsigned int x, unsigned int y) {
    for (int i = 40; i < x;)
    {
        RectangleShape myline1(Vector2f(x, 1.f));
        myline1.rotate(180.f);
        myline1.setFillColor(Color(125, 125, 125, 125));
        myline1.move(i, 0);
    }
    for (int j = 40; j < y;)
    {
        RectangleShape myline2(Vector2f(x, 1.f));
        myline2.rotate(-90.f);
        myline2.setFillColor(Color(125, 125, 125, 125));
        myline2.move(0, j);
    }
}

//проверка, не пустой ли файл(конфиг)
bool Is_empty(std::ifstream& pFile) {
    return pFile.peek() == std::ifstream::traits_type::eof();
}

//exit
void Exit(vector <double>& Xcords, vector <double>& Ycords, RenderWindow &window1) {
    cout << endl;
    for (int i = 0; i < Xcords.size(); i++)
    {
        cout << Xcords[i] << " " << Ycords[i] << " ; ";
        cout << "Lenght of stitch " << i << " is " << (int)Lenghts[i] << " mm" << endl;
    }

    window1.close();

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
CircleShape StezhokLenght(int x, int y, double lengh, RenderWindow &window1) {
    CircleShape stezh(lengh); 
    //stezh.setFillColor(Color::White);
    stezh.setOutlineThickness(1);
    stezh.setOutlineColor(sf::Color(0, 0, 0, 45));
    stezh.move(x - lengh, y - lengh);
    window1.draw(stezh);
    return stezh;
}

// draw line(stitch)
void DrawLine(int x, int y, double lengh, double ugol, Color color1) {
    RectangleShape* myline = new RectangleShape(Vector2f(lengh, 2.f));
    myline->rotate(ugol);
    myline->setFillColor(color1);
    myline->move(x, y);
}

void DrawNewLine(Color color1, int i, RenderWindow &window1) {
    double alpha;
    double lenght = sqrt(pow(Xcords[i] - Xcords[i - 1], 2) + pow(Ycords[i] - Ycords[i - 1], 2)); // длина линии
    if (Xcords[i] - Xcords[i - 1] < 0) {
        alpha = atan((Ycords[i] - Ycords[i - 1]) / (Xcords[i] - Xcords[i - 1])) * 180.0 / 3.141592;
    }
    else {
        alpha = 180 + atan((Ycords[i] - Ycords[i - 1]) / (Xcords[i] - Xcords[i - 1])) * 180.0 / 3.141592;
    }
    RectangleShape myline(Vector2f(lenght, 2.f));
    myline.rotate(alpha);
    myline.setFillColor(color1);
    myline.move(Xcords[i], Ycords[i]);
    window1.draw(myline);
}

//отображение информации по смещению и длине последнего стежка на экране
void DrawText(unsigned int shirina, RenderWindow &window1) {
    Vector2i position = Mouse::getPosition(window1);

    ostringstream offsetDX; //offset by dx
    ostringstream offsetDY; //offset by dy
    ostringstream laststitch; // lenght of the last stitch

    offsetDX << abs(position.x - Xcords[Xcords.size() - 1]);
    offsetDY << abs(position.y - Ycords[Ycords.size() - 1]);
    laststitch << (int)Lenghts[Lenghts.size() - 1];

    sf::Text dxdy;
    sf::Font font;
    font.loadFromFile("ebrima.ttf");
    dxdy.setFont(font);
    dxdy.setString("OFFSET BY dx = " + offsetDX.str() + "\n" + "OFFSET by dy = " + offsetDY.str() + "\n" + "Last stitch's lenght = " + laststitch.str());
    dxdy.setCharacterSize(20);
    dxdy.setFillColor(sf::Color::Black);
    dxdy.setStyle(sf::Text::Bold);
    dxdy.move(shirina - 300, 0);
    window1.draw(dxdy);
}

//delete previous line
void DeletePrevious(RenderWindow &window1, vector<double>& Xcords1, vector<double>& Ycords1, vector<double>& Lenghts1) {
    Xcords1.erase(Xcords1.end() - 1);
    Ycords1.erase(Ycords1.end() - 1);
    if(Lenghts1.size()>1)
    Lenghts1.erase(Lenghts1.end() - 1);
}

//line's color
Color LineColor(Color color1) {
    cout << "   What color would you like to see your lines?" << endl
        << "\t" << "1.Gray" << endl
        << "\t" << "2.Black" << endl
        << "\t" << "3.Green" << endl
        << "\t" << "4.Blue" << endl
        << "\t" << "5.Red" << endl
        << "\t" << "6.Yellow" << endl
        << "\t" << "7.Cyan" << endl
        << "   Your choice: ";
    int x;
    cin >> x;
    cout << endl;
    switch (x)
    {
    case 1: color1 = Color(128, 128, 128); break;
    case 2: color1 = Color::Black; break;
    case 3: color1 = Color::Green; break;
    case 4: color1 = Color::Blue; break;
    case 5: color1 = Color::Red; break;
    case 6: color1 = Color::Yellow; break;
    case 7: color1 = Color::Cyan; break;
    default: color1 = Color::Black; break;
    }
    return color1;
}

void keyPressed(const sf::Event& event);

int main()
{
    setlocale(LC_ALL, "");
    Color linescolor;
    bool assistFlag = false;
    double alpha;
    int firsttap = 0; //counter of taps
    RenderWindow window(VideoMode(1600, 900), "SFML2.0");
    window.setFramerateLimit(10);
    sf::Vector2u size = window.getSize();
    unsigned int width = size.x;
    unsigned int height = size.y;

    linescolor = LineColor(linescolor);
    int stezhokL = 200; //максимальная длина стежка
    int choice;
    cout << "   Would you like to use your config(1) or draw new(2)? ";
    cin >> choice;
    if (choice == 1) {
        string cfg = "config.txt";
        ifstream incfg;
        incfg.open(cfg);
        if (!incfg.is_open()) {
            cout << "   Openning file error." << endl;
        }
        else {
            cout << "   Config opened." << endl;
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

                    if (firsttap != 0) {
                        double lenght = sqrt(pow(Xcords[firsttap] - Xcords[firsttap - 1], 2) + pow(Ycords[firsttap] - Ycords[firsttap - 1], 2));

                        if (Xcords[firsttap] - Xcords[firsttap - 1] < 0) {
                            alpha = atan((Ycords[firsttap] - Ycords[firsttap - 1]) / (Xcords[firsttap] - Xcords[firsttap - 1])) * 180.0 / 3.141592;
                        }
                        else {
                            alpha = 180 + atan((Ycords[firsttap] - Ycords[firsttap - 1]) / (Xcords[firsttap] - Xcords[firsttap - 1])) * 180.0 / 3.141592;
                        }

                        Lenghts.push_back(lenght);
                    }
                }
            }
            else cout << "   Config is empty." << endl;
        }
        incfg.close();
        choice = 0;
    }

    //клеточки
    for (int i = 20; i < width;)
    {
        RectangleShape* myline1 = new RectangleShape(Vector2f(height, 1.f));
        myline1->rotate(90.f);
        myline1->setFillColor(Color(125, 125, 125, 150));
        myline1->move(i, 0);
        marks1.push_back(myline1);
        i = i + 20;
    }
    for (int j = 20; j < height;)
    {
        RectangleShape* myline2 = new RectangleShape(Vector2f(width, 1.f));
        myline2->setFillColor(Color(125, 125, 125, 150));
        myline2->move(0, j);
        marks2.push_back(myline2);
        j = j + 20;
    }

    // main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed: {
                Exit(Xcords, Ycords, window);
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

                    StezhokLenght(event.mouseButton.x, event.mouseButton.y, stezhokL, window);

                }
                // отрисовка линий 
                else {
                    Xcords.push_back(event.mouseButton.x);
                    Ycords.push_back(event.mouseButton.y);
                    firsttap++;
                    double lenght = sqrt(pow(Xcords[firsttap - 1] - Xcords[firsttap - 2], 2) + pow(Ycords[firsttap - 1] - Ycords[firsttap - 2], 2)); // длина линии
                    // вычисление угла поворота линии 
                    if (lenght <= stezhokL) {

                        Lenghts.push_back(lenght);

                        if (Xcords[firsttap - 1] - Xcords[firsttap - 2] < 0) {
                            alpha = atan((Ycords[firsttap - 1] - Ycords[firsttap - 2]) / (Xcords[firsttap - 1] - Xcords[firsttap - 2])) * 180.0 / 3.141592;
                        }
                        else {
                            alpha = 180 + atan((Ycords[firsttap - 1] - Ycords[firsttap - 2]) / (Xcords[firsttap - 1] - Xcords[firsttap - 2])) * 180.0 / 3.141592;
                        }

                        //DrawLine(event.mouseButton.x, event.mouseButton.y, lenght, alpha, linescolor);
                    }
                    else {
                        cout << "   Wrong stitch lenght!" << endl;
                        Xcords.erase(Xcords.end() - 1);
                        Ycords.erase(Ycords.end() - 1);
                        firsttap--;
                    }
                }

            }
            case sf::Event::KeyPressed: //нажата клавиша клавиатуры
                keyPressed(event);
                break;
            }
            //чтобы при резайзе окна не сжимался рисунок
            if (event.type == Event::Resized)
            {
                Vector2f windowSize = Vector2f(event.size.width, event.size.height);
                window.setView(View(Vector2f(windowSize.x / 2.f, windowSize.y / 2.f), Vector2f(windowSize)));
            }
        }

        //backspace чтобы удалить линию
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) && (Xcords.size() > 0)) {
            DeletePrevious(window, Xcords, Ycords, Lenghts);
            firsttap--;
        }

        //включение и выключение флага для вспомогательной окружности
        if ((assistFlag == false) && Keyboard::isKeyPressed(Keyboard::LAlt)) {
            assistFlag = true;
        }
        else if ((assistFlag == true) && Keyboard::isKeyPressed(Keyboard::LAlt)) {
            assistFlag = false;
        }

        window.clear(Color(240, 240, 240, 255));

        //вспомогательная окружность включается, когда поднят флаг(нажата клавиша LAlt)
        if ((firsttap > 0) && (assistFlag == true)) {
            window.draw(StezhokLenght(Xcords[firsttap - 1], Ycords[firsttap - 1], stezhokL, window));
        }

        //два цикла для отрисовки сетки
        for (auto it3 = marks1.begin(); it3 != marks1.end(); it3++)
        {
            window.draw(**it3);
        }
        for (auto it4 = marks2.begin(); it4 != marks2.end(); it4++)
        {
            window.draw(**it4);
        }

        for (int i = 1; i < Xcords.size(); i++)
        {
            DrawNewLine(linescolor, i, window);
        }

        //отображение текста
        if (Xcords.size() > 0) {
            DrawText(width, window);
        }

        window.display();

        // вывод координат в файл
        fstream cords;
        cords.open("cords.txt", ios::out);
        cords << "Координаты узлов: " << endl;
        for (int  i = 0; i < Xcords.size();  i++)
        {
            cords << i+1 << " - (" << Xcords[i] << "; " << Ycords[i] << ") ";
            cords << "Длина " << i << " стежка = " << (int)Lenghts[i] <<"mm" << endl;
        }
        cords.close(); 

        //при нажатии Escape выход из программы
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            Exit(Xcords, Ycords, window); return 0;
        }
    }
    return 0;
}

//обработка смещения рисунка
void keyPressed(const sf::Event& event)
//обработка нажатия клавиш
{
    switch (event.key.code)  //анализ нажатой клавиши
    {
    case sf::Keyboard::Left:  //если нажата клавиша влево
        for (int i = 0; i <= Xcords.size()-1; i++)
        {
            Xcords[i] -= 30;
        }
        break;
    case sf::Keyboard::Right: //если нажата клавиша вправо
        for (int i = 0; i <= Xcords.size() - 1; i++)
        {
            Xcords[i] += 30;
        }
            break;
    case sf::Keyboard::Up:  //если нажата клавиша вверх
        for (int i = 0; i <= Xcords.size() - 1; i++)
        {
            Ycords[i] -= 30;
        }
        break;
    case sf::Keyboard::Down: //если нажата клавиша вниз
        for (int i = 0; i <= Xcords.size() - 1; i++)
        {
            Ycords[i] += 30;
        }
        break;
    }
}