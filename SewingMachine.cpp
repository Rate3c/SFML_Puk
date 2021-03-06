/*?????? ???????? ??????? ??.0301 ???????? ???????
* ???????:
???????? ????????? ? ??????????? ????????, ??????????? ???????????????? ??????? ???????? ??????? ??????? ?? ?????? ??????????,
? ????? ???????? ?????? ? ???? ?????????????????? ????????? ??????? ? ?????????????? ??????? ?????? ? ???????? ????????? ??????????.
???????????? ? ??????? ??????? ???????? ?????? ????? ?????? ? ???????? ????? ?????? ????,
???????? ?????????? (0,0) ?????? ??????? ? ?????????? ??????? ?????????. ??? ??????? ?????????? ?????? ?????? ???????????? ? ?????? ?????, ?? ?? ??????,
??? ?? ???????????? ????? ??????, ? ?. ?., ??????? ???? ???????. ????????? ????? ??????? ???????????? ?? ??????? ??????? ?Esc?.
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

vector<double> Xcords; //vector x coordinates
vector<double> Ycords; //vector y coordinates
vector<double> Lenghts = { 0 }; //lenght of the stitches



//fot grid drawing
void Grid(RenderWindow& window1) {

	sf::Vector2u size = window1.getSize(); //?????? ????
	//?????
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
int Exit(vector <double>& Xcords, vector <double>& Ycords, RenderWindow& window1) {
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
			outconfig << Xcords[i] << " " << Ycords[i] << endl; // ????? ????????? ? ????
		}
		outconfig.close();
	}

	cout << endl << "Program complete." << endl; return 0;
}

//circle for per stitch length
CircleShape StezhokLenght(int x, int y, RenderWindow& window1, int stezhok) {
	CircleShape stezh(stezhok);
	stezh.setOutlineThickness(1);
	stezh.setOutlineColor(sf::Color(0, 0, 0, 45));
	stezh.move(x - stezhok, y - stezhok);
	window1.draw(stezh);
	return stezh;
}

// draw line(stitch)
void DrawNewLine(Color color1, int i, RenderWindow& window1) {
	double alpha;
	double lenght = sqrt(pow(Xcords[i] - Xcords[i - 1], 2) + pow(Ycords[i] - Ycords[i - 1], 2)); // ????? ?????
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

//display of information on offset and length of the last stitch on the screen
void DrawText(RenderWindow& window1) {
	sf::Vector2u size = window1.getSize(); //?????? ????

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
	dxdy.move(size.x - 300, 0);
	window1.draw(dxdy);
}

//delete previous line
void DeletePrevious(RenderWindow& window1, vector<double>& Xcords1, vector<double>& Ycords1, vector<double>& Lenghts1) {
	Xcords1.erase(Xcords1.end() - 1);
	Ycords1.erase(Ycords1.end() - 1);
	if (Lenghts1.size() > 1)
		Lenghts1.erase(Lenghts1.end() - 1);
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
	default: color1 = Color::Black; break;
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
void AutoCentering(vector <double> &cords1, vector <double>& cords2, unsigned int side1, unsigned int side2) {

	double shiftY = side1 / 2 - cords1[cords1.size() - 1];
	double shiftX = side2 / 2 - cords2[cords2.size() - 1];
		for (int i = 0; i < cords1.size(); i++)
		{
			cords1[i] = cords1[i] + shiftY;
			cords2[i] = cords2[i] + shiftX;
		}
}

//scaling
void Scaling(vector <double> &x, vector <double> &y, double scalee) {
	for (int  i = 0; i < x.size(); i++)
	{
		x[i] = (int)(x[i] * scalee);
		y[i] = (int)(y[i] * scalee);
	}
}

int main()
{
	setlocale(LC_ALL, "");
	Color linescolor; //color of the lines
	bool assistFlag = false; //flag
	double alpha;
	int stezhokL;
	int firsttap = 0; //counter of taps
	double scale = 1; // current scale
	RenderWindow window(VideoMode(1600, 900), "SFML2.0");
	window.setFramerateLimit(10);

	sf::Vector2u windowsize = window.getSize(); //size of rendered window

	//enter the lenght of the stitch
	cout << "   Enter lenght of the stitch: ";
	cin >> stezhokL; cout << endl;
	linescolor = LineColor(linescolor);

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

		if (Xcords.size() > 0) {

			if ((abs(Xcords[Xcords.size() - 1] - windowsize.x / 2) >= windowsize.x / 2 - 90) || (abs(Ycords[Ycords.size() - 1] - windowsize.y / 2) >= windowsize.y / 2) - 90) {
				AutoCentering(Ycords, Xcords, windowsize.y, windowsize.x);
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
				Exit(Xcords, Ycords, window);
				return 0;
			}
			//click registration
			if (event.type == sf::Event::MouseButtonPressed) {
				// reading the first point without drawing 
				if (firsttap == 0) {
					Vector2i position = Mouse::getPosition(window);
					Xcords.push_back(position.x);
					Ycords.push_back(position.y);
					firsttap++;
				}
				// lines drawing
				else {
					Xcords.push_back(event.mouseButton.x);
					Ycords.push_back(event.mouseButton.y);
					firsttap++;
					double lenght = sqrt(pow(Xcords[firsttap - 1] - Xcords[firsttap - 2], 2) + pow(Ycords[firsttap - 1] - Ycords[firsttap - 2], 2)); // lenght of the line
					// calculating the angle of rotation of the line 
					if (lenght <= stezhokL) {

						Lenghts.push_back(lenght);

						if (Xcords[firsttap - 1] - Xcords[firsttap - 2] < 0) {
							alpha = atan((Ycords[firsttap - 1] - Ycords[firsttap - 2]) / (Xcords[firsttap - 1] - Xcords[firsttap - 2])) * 180.0 / 3.141592;
						}
						else {
							alpha = 180 + atan((Ycords[firsttap - 1] - Ycords[firsttap - 2]) / (Xcords[firsttap - 1] - Xcords[firsttap - 2])) * 180.0 / 3.141592;
						}
					}
					else {
						cout << "   Wrong stitch lenght!" << endl;
						Xcords.erase(Xcords.end() - 1);
						Ycords.erase(Ycords.end() - 1);
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
					stezhokL = stezhokL + stezhokL * 0.1;
					scale = 1 + scale * 0.1;
					Scaling(Xcords, Ycords, scale);
					AutoCentering(Ycords, Xcords, windowsize.y, windowsize.x);
				}

				//scaling if "-" has been pressed
				if (event.key.code == sf::Keyboard::Subtract)
				{
					stezhokL = stezhokL - stezhokL * 0.1;
					scale = 1 - scale * 0.1;
					Scaling(Xcords, Ycords, scale);
					AutoCentering(Ycords, Xcords, windowsize.y, windowsize.x);
				}

				//delete previous stitch
				if (event.key.code == sf::Keyboard::BackSpace)
				{
					if (Xcords.size() > 0) {
						DeletePrevious(window, Xcords, Ycords, Lenghts);
						firsttap--;
					}
				}

				//exit
				if (event.key.code == sf::Keyboard::Escape)
				{
					Exit(Xcords, Ycords, window);
				}

				//toggle the flag on and off for the assistance circle
				if (event.key.code == sf::Keyboard::LAlt)
				{
					assistFlag = Assistance(assistFlag);
				}
			}

			window.clear(Color(240, 240, 240, 255));

			//the construction circle turns on when the flag is raised (the LAlt key is pressed)
			if ((firsttap > 0) && (assistFlag == true)) {
				window.draw(StezhokLenght(Xcords[firsttap - 1], Ycords[firsttap - 1], window, stezhokL));
			}

			//grid drawing
			Grid(window);

			for (int i = 1; i < Xcords.size(); i++) {
				DrawNewLine(linescolor, i, window);
			}

			//display text
			if (Xcords.size() > 0) {
				DrawText(window);
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
		for (int i = 0; i <= Xcords.size() - 1; i++)
		{
			Xcords[i] -= 30;
		}
		break;
	case sf::Keyboard::Right: //if the right key is pressed
		for (int i = 0; i <= Xcords.size() - 1; i++)
		{
			Xcords[i] += 30;
		}
		break;
	case sf::Keyboard::Up:  //if the up key is pressed
		for (int i = 0; i <= Xcords.size() - 1; i++)
		{
			Ycords[i] -= 30;
		}
		break;
	case sf::Keyboard::Down: //if the down key is pressed
		for (int i = 0; i <= Xcords.size() - 1; i++)
		{
			Ycords[i] += 30;
		}
	default:
		break;
	}
}

//exit function
void Exit(const sf::Event& event, RenderWindow& window1) {
	switch (event.key.code) {
	case sf::Keyboard::Escape: {
		Exit(Xcords, Ycords, window1);
	}
	default: break;
	}
}