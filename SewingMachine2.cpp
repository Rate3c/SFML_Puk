/*Работу выполнил студент гр.0301 Нестеров Евгений
* ЗАДАНИЕ:
Написать программу – графический редактор, позволяющую автоматизировать процесс создания вышивки сначала на экране компьютера,
а затем переноса данных в виде последовательности координат стежков в автоматическую швейную машину в качестве программы управления.
Пользователь с помощью курсора выбирает нужную точку экрана и нажимает левую кнопку мыши,
фиксируя координату (0,0) начала рисунка в декартовой системе координат. Для задания очередного стежка курсор перемещается в другую точку, но не дальше,
чем на максимальную длину стежка, и т. д., образуя узор вышивки. Окончание ввода рисунка производится по нажатию клавиши «Esc».
_________________________________________________________
* The work was carried out by student group 0301 Nesterov Evgeny
* THE TASK:
Write a program - a graphic editor that allows you to automate the process of creating embroidery, first on the computer screen,
and then transferring the data as a sequence of stitch coordinates to the automatic sewing machine as a control program.
Using the cursor, the user selects the desired point on the screen and presses the left mouse button,
fixing the coordinate (0,0) of the beginning of the drawing in the Cartesian coordinate system. To specify the next stitch, the cursor moves to another point, but not further,
than the maximum stitch length, etc., to form an embroidery pattern. The end of entering a picture is made by pressing the "Esc" key.
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
config.txt - config file
*/

#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>

using namespace sf;
using namespace std;

vector<double> DX; //offsets by DX
vector<double> DY; //offsets by DY


//fot grid drawing
void Grid(RenderWindow& window1) {

	sf::Vector2u size = window1.getSize(); //размер окна
	//сетка
	for (int i = 25; i < size.x;)
	{
		RectangleShape myline1(Vector2f(size.y, 1.f));
		myline1.rotate(90.f);
		myline1.setFillColor(Color(100, 100, 100, 150));
		myline1.move(i, 0);
		window1.draw(myline1);
		i = i + 25;
	}
	for (int j = 25; j < size.y;)
	{
		RectangleShape myline2(Vector2f(size.x, 1.f));
		myline2.setFillColor(Color(100, 100, 100, 150));
		myline2.move(0, j);
		window1.draw(myline2);
		j = j + 25;
	}

}

//checking if the file is empty (config)
bool Is_empty(std::ifstream& pFile) {
	return pFile.peek() == std::ifstream::traits_type::eof();
}

//exit
int Exit(vector <double>& DX, vector <double>& DY, RenderWindow& window1, double currscal) {
	cout << endl << 0 << ";" << 0 << endl;
	for (int i = 1; i < DX.size(); i++)
	{
		int x = 0; int y = 0;
		for (int j = 1; j <= i; j++)
		{
			x += DX[j];
			y += DY[j];
		}
		cout << x << ";" << y << endl;
	}

	window1.close();

	cout << endl << "Do you want to save coordinates in config? Press \"1\" for YES or \"2\" for NO. ";
	int action;
	cin >> action;
	if (action == 1) {
		string cfg = "config.txt";
		fstream outconfig;
		outconfig.open(cfg, ios::out);
		outconfig << 0 << " " << 0 << endl;
		for (int i = 1; i < DX.size(); i++)
		{
			DX[i] = (int)(DX[i] / currscal); 
			DY[i] = (int)(DY[i] / currscal); 
			outconfig << DX[i] << " " << DY[i] << endl; // вывод координат в файл
		}
		outconfig.close();
	}

	cout << endl << "Program complete." << endl; return 0;
}

//circle for per stitch length
void StezhokLength(vector <double>& DX1, vector <double>& DY1, RenderWindow& window1, int stezhok) {
	double x = DX1[0];
	double y = DY1[0];
	for (int i = 1; i < DX1.size(); i++)
	{
		x += DX1[i];
		y += DY1[i];
	}

	CircleShape stezh(stezhok);
	stezh.setOutlineThickness(1);
	stezh.setOutlineColor(sf::Color(0, 0, 0, 45));
	stezh.move(x - stezhok, y - stezhok);
	window1.draw(stezh);

	Vector2i position = Mouse::getPosition(window1);
	if (sqrt(pow(position.x - x, 2) + pow(position.y - y, 2)) <= stezhok) {
		sf::VertexArray assistanceline(sf::LineStrip, 2);
		assistanceline[0].position = sf::Vector2f(x, y);
		assistanceline[1].position = sf::Vector2f(position.x, position.y);
		assistanceline[0].color = Color(75, 0, 130);
		assistanceline[1].color = Color(75, 0, 130);
		window1.draw(assistanceline);
	}
}

// draw line(stitch)
void DrawNewLine(Color color1, int i, RenderWindow& window1) {
	double alpha;
	double length = sqrt(pow(DX[i], 2) + pow(DY[i], 2)); // длина линии
	if (DX[i] < 0) {
		alpha = 180 + atan((DY[i]) / (DX[i])) * 180.0 / 3.141592;
	}
	else {
		alpha = atan((DY[i]) / (DX[i])) * 180.0 / 3.141592;
	}
	RectangleShape myline(Vector2f(length, 2.f));
	myline.rotate(alpha);
	myline.setFillColor(color1);
	double x = DX[0];
	double y = DY[0];
	for (int j = 1; j < i; j++)
	{
		x += DX[j];
		y += DY[j];
	}
	myline.move(x, y);
	window1.draw(myline);
}

//display of information on offset and length of the last stitch on the screen
void DrawText(RenderWindow& window1, double curscale) {
	sf::Vector2u size = window1.getSize(); //размер окна

	Vector2i position = Mouse::getPosition(window1);

	ostringstream offsetDX; //offset by dx
	ostringstream offsetDY; //offset by dy
	ostringstream lengthfrom; // length of the last stitch
	ostringstream scale; //scale of window

	double x = DX[0];
	double y = DY[0];
	for (int i = 1; i < DX.size(); i++)
	{
		x += DX[i];
		y += DY[i];
	}
	double length = sqrt(pow(position.x - x, 2) + pow(position.y - y, 2));
	offsetDX << abs(position.x - x);
	offsetDY << abs(position.y - y);
	lengthfrom << (int)length;
	scale << curscale;

	sf::Text dxdy;
	sf::Font font;
	font.loadFromFile("ebrima.ttf");
	dxdy.setFont(font);
	dxdy.setString("OFFSET BY dx = " + offsetDX.str() + "\n" + "OFFSET by dy = " + offsetDY.str() + "\n" + "Length from last point = " + lengthfrom.str() + "\nCurrent scale: " + scale.str() + ": 1");
	dxdy.setCharacterSize(20);
	dxdy.setFillColor(sf::Color::Black);
	dxdy.setStyle(sf::Text::Bold);
	dxdy.move(size.x - 300, 0);
	window1.draw(dxdy);
}

//delete previous line
void DeletePrevious(RenderWindow& window1, vector<double>& DX1, vector<double>& DY1) {
	DX1.erase(DX1.end() - 1);
	DY1.erase(DY1.end() - 1);
}

//line's color
Color LineColor(Color color1) {
	cout << "   What color would you like to see your lines?" << endl
		<< "\t" << "1.Orange" << endl
		<< "\t" << "2.Black" << endl
		<< "\t" << "3.Green" << endl
		<< "\t" << "4.Blue" << endl
		<< "\t" << "5.Red" << endl
		<< "\t" << "6.Yellow" << endl
		<< "\t" << "7.Cyan" << endl
		<< "\t" << "8.Purple" << endl
		<< "\t" << "9.White" << endl
		<< "   Your choice: ";
	int x;
	cin >> x;
	cout << endl;
	switch (x)
	{
	case 1: color1 = Color(255, 125, 0); break;
	case 2: color1 = Color::Black; break;
	case 3: color1 = Color::Green; break;
	case 4: color1 = Color::Blue; break;
	case 5: color1 = Color::Red; break;
	case 6: color1 = Color::Yellow; break;
	case 7: color1 = Color::Cyan; break;
	case 8: color1 = Color(255, 0, 255); break;
	case 9: color1 = Color::White; break;
	default: color1 = Color::Black; break;
	}
	return color1;
}

Color WindowColor(Color color1) {
	cout << "   What color would you like to see your window?" << endl
		<< "\t" << "1.White" << endl
		<< "\t" << "2.Black" << endl
		<< "\t" << "3.Green" << endl
		<< "\t" << "4.Blue" << endl
		<< "\t" << "5.Red" << endl
		<< "\t" << "6.Yellow" << endl
		<< "\t" << "7.Orange" << endl
		<< "   Your choice: ";
	int x;
	cin >> x;
	cout << endl;
	switch (x)
	{
	case 1: color1 = Color(240, 240, 240, 255); break;
	case 2: color1 = Color(0, 0, 0); break;
	case 3: color1 = Color(0, 125, 0); break;
	case 4: color1 = Color(0, 0, 255); break;
	case 5: color1 = Color(255, 0, 0); break;
	case 6: color1 = Color(255, 255, 0); break;
	case 7: color1 = Color(255, 125, 0); break;
	default: color1 = Color(240, 240, 240, 255); break;
	}
	return color1;
}

void Shift(const sf::Event& event, RenderWindow& window1);

//raising and lowering the flag when pressing LAlt
bool Assistance(bool assistFlag1) {

	if (assistFlag1 == false) {
		assistFlag1 = true;
	}
	else if (assistFlag1 == true) {
		assistFlag1 = false;
	}
	return assistFlag1;
}

//auto centering if picture is out of window
void AutoCentering(vector <double>& DY1, vector <double>& DX1, unsigned int side1, unsigned int side2) {
	double x = DX1[0];
	double y = DY1[0];
	for (int i = 1; i < DX1.size(); i++)
	{
		x += DX1[i];
		y += DY1[i];
	}
	double shiftY = side1 / 2 - y;
	double shiftX = side2 / 2 - x;
	DY1[0] = DY1[0] + shiftY;
	DX1[0] = DX1[0] + shiftX;
}

//scaling
void Scaling(vector <double>& x, vector <double>& y, double scalee, int j, int& stezh) {
	if (j == 1) {
		stezh *= 1.2;
		for (int i = 1; i < x.size(); i++)
		{
			x[i] = (int)(x[i] * 1.1);
			y[i] = (int)(y[i] * 1.1);
		}
	}
	else if (j == 2){
		stezh /= 1.2;
		for (int i = 1; i < x.size(); i++)
		{
			x[i] = (int)(x[i] / 1.1);
			y[i] = (int)(y[i] / 1.1);
		}
	}

	
}

int main()
{
	Color windowcolor; //color of the window
	Color linescolor; //color of the lines
	bool assistFlag = false; //flag
	int stezhokL;
	int firsttap = 0; //counter of taps
	double scale = 1; // current scale
	double currscale = scale;
	RenderWindow window(VideoMode(1600, 900), "SFML2.0");
	window.setFramerateLimit(10);

	sf::Vector2u windowsize = window.getSize(); //size of rendered window

	//enter the length of the stitch
	cout << "   Enter length of the stitch: ";
	cin >> stezhokL; cout << endl;
	linescolor = LineColor(linescolor); //lines' color
	windowcolor = WindowColor(windowcolor); // window's color

	//reading config
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
						DY.push_back(I);
					}
					else if (i % 2 == 0) {
						incfg >> I;
						DX.push_back(I);
					}
					firsttap++;
				}
				DX.erase(DX.end() - 1);
				firsttap /= 2;
			}
			else cout << "   Config is empty." << endl;
		}
		incfg.close();
		choice = 0;

		if (DX.size() > 0) {

			if ((abs(DX[DX.size() - 1] - windowsize.x / 2) >= windowsize.x / 2 - 100) || (abs(DY[DY.size() - 1] - windowsize.y / 2) >= windowsize.y / 2) - 100) {
				AutoCentering(DY, DX, windowsize.y, windowsize.x);
			}
		}
	}

	// main loop
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case (sf::Event::KeyPressed): { //keyboard key pressed
				//shifting
				Shift(event, window);
				break;
			}
			default:
				break;
			}
			//if you pressed the cross, the program will end
			if (event.type == sf::Event::Closed) {
				Exit(DX, DY, window, currscale);
				return 0;
			}
			//click registration
			if (event.type == sf::Event::MouseButtonPressed) {
				// reading the first point without drawing 
				if (firsttap == 0) {
					DX.push_back(event.mouseButton.x);
					DY.push_back(event.mouseButton.y);
					firsttap++;
				}
				// lines drawing
				else {
					double x = DX[0];
					double y = DY[0];
					for (int i = 1; i < DX.size(); i++)
					{
						x += DX[i];
						y += DY[i];
					}
					DX.push_back(event.mouseButton.x - x);
					DY.push_back(event.mouseButton.y - y);
					firsttap++;
					double length = sqrt(pow(DX[firsttap - 1], 2) + pow(DY[firsttap - 1], 2)); // length of the line
					if (abs(length) > stezhokL) {
						cout << "   Wrong stitch length!" << endl;
						DX.erase(DX.end() - 1);
						DY.erase(DY.end() - 1);
						firsttap--;
					}
				}

			}

			//so that when the window is resized, the picture does not shrink
			if (event.type == Event::Resized)
			{
				Vector2f windowSize = Vector2f(event.size.width, event.size.height);
				window.setView(View(Vector2f(windowSize.x / 2.f, windowSize.y / 2.f), Vector2f(windowSize)));
			}

			if (event.type == sf::Event::KeyReleased)
			{
				//scaling if "+" has been pressed
				if (event.key.code == sf::Keyboard::Add)
				{
					if (currscale < 2.4) {
						int j = 1;
						currscale += 0.1;
						Scaling(DX, DY, currscale, j, stezhokL);
						AutoCentering(DY, DX, windowsize.y, windowsize.x);
					}
				}

				//scaling if "-" has been pressed
				if (event.key.code == sf::Keyboard::Subtract)
				{
					if (currscale > 0.5) {
						int j = 2;
						currscale -= 0.1;
						Scaling(DX, DY, scale, j, stezhokL);
						AutoCentering(DY, DX, windowsize.y, windowsize.x);
					}
				}

				//delete previous stitch
				if (event.key.code == sf::Keyboard::BackSpace)
				{
					if (DX.size() > 0) {
						DeletePrevious(window, DX, DY);
						firsttap--;
					}
				}

				//exit
				if (event.key.code == sf::Keyboard::Escape)
				{
					Exit(DX, DY, window, currscale);
				}

				//toggle the flag on and off for the assistance circle
				if (event.key.code == sf::Keyboard::LAlt)
				{
					assistFlag = Assistance(assistFlag);
				}
			}

			window.clear(Color(windowcolor));

			//the construction circle turns on when the flag is raised (the LAlt key is pressed)
			if ((firsttap > 0) && (assistFlag == true)) {
				StezhokLength(DX, DY, window, stezhokL);
			}

			//grid drawing
			Grid(window);

			for (int i = 1; i < DX.size(); i++) {
				DrawNewLine(linescolor, i, window);
			}

			//display text
			if (DX.size() > 0) {
				DrawText(window, currscale);
			}
			window.display();
		}
	}
	return 0;
}

//key handling
void Shift(const sf::Event& event, RenderWindow& window1) {
	switch (event.key.code)  //analysis of the pressed key
	{
	case sf::Keyboard::Left:  //if the left key is pressed
		DX[0] -= 30;
		break;
	case sf::Keyboard::Right: //if the right key is pressed
		DX[0] += 30;
		break;
	case sf::Keyboard::Up:  //if the up key is pressed
		DY[0] -= 30;
		break;
	case sf::Keyboard::Down: //if the down key is pressed
		DY[0] += 30;
		break;
	default:
		break;
	}
}