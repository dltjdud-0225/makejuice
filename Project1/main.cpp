#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <sstream>

using namespace std;

#define ENTER 13             // Enter Ű�� ASCII ��
#define SPACE 32             // Space Ű�� ASCII ��
#define INITIAL_SUM 1000     // ���� ���� �� �ʱ� �ݾ�
#define WIN_SUM 10000        // �¸� ���� �ݾ�
#define LOSS_SUM 0           // �й� ���� �ݾ�
#define MAGIC_KEY 224        // ����Ű�� ������ �� ��Ÿ���� Ư�� Ű ��

// �޴� �׸��� �����ϴ� enum
enum MENU
{
    GAMESTART = 0,  // ���� ����
    INFO,           // ���� ����
    QUIT            // ���� ����
};

// ���ӿ� ���� ������ �������� ������ ���ӽ����̽�
namespace GameConstants {
    const vector<pair<char, string>> fruitTypes = {
        {'Q', "����"}, {'W', "��"}, {'E', "�ٳ���"}, {'R', "����"}  // ���ϰ� �׿� �����Ǵ� Ű ��
    };
}

using GameConstants::fruitTypes;  // fruitTypes ���

// �ܼ� Ŀ���� (x, y) ��ǥ�� �̵���Ű�� �Լ�
void gotoxy(int x, int y)
{
    COORD Pos;
    Pos.X = x;  // X ��ǥ ����
    Pos.Y = y;  // Y ��ǥ ����
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);  // Ŀ�� ��ġ ����
}

// �ܼ� ȭ�� ���� �Լ� (ũ��, ���� ����)
void SetConsoleView()
{
    system("mode con:cols=70 lines=30");  // �ܼ� ȭ�� ũ�� ���� (���� 70, ���� 30)
    system("title Fruit Juice");          // �ܼ� â ���� ����
}

// ���� ���� ȭ���� �׸��� �Լ�
void DrawReadyGame()
{
    system("cls");  // ȭ���� Ŭ����
    gotoxy(20, 2);  // ȭ�� ��ǥ�� Ŀ�� �̵�
    cout << "******************************";
    gotoxy(20, 3);
    cout << "*       Make Juice           *";
    gotoxy(20, 4);
    cout << "******************************";
    gotoxy(25, 8);  // �޴� �׸� Ŀ�� ǥ��
    cout << "Game Start";
    gotoxy(25, 9);
    cout << "Game Info";
    gotoxy(25, 10);
    cout << "Quit" << endl;
}

// ���� ���� ȭ���� �׸��� �Լ�
void DrawInfoGame()
{
    system("cls");
    gotoxy(10, 3);
    cout << "*******************************************";
    gotoxy(10, 4);
    cout << "| Ű ����: Q:���� W:�� E:�ٳ��� R:����     |";
    gotoxy(10, 5);
    cout << "| ����: +500 ����Ʈ  ����: -500 ����Ʈ     |";
    gotoxy(10, 6);
    cout << "*******************************************";
    gotoxy(10, 7);
    cout << "| ���� ����:                              |";
    gotoxy(10, 8);
    cout << "| 1) �հ� = 0: \"BAD\"                     |";
    gotoxy(10, 9);
    cout << "| 2) �հ� = 10,000: \"GOOD\"               |";
    gotoxy(10, 10);
    cout << "*******************************************";
    gotoxy(10, 11);
    cout << "��� �Ϸ��� �ƹ� Ű�� �����ʽÿ� . . .";
    system("pause>null");  // �ƹ� Ű�� ������ ��� ����
}

// ���� ���� �Լ� (���� ���� ���� ����)
void GenerateQuestion(vector<pair<char, int>>& questionVec)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> fruitDist(0, fruitTypes.size() - 1);  // ������ �������� �̱� ���� ����
    uniform_int_distribution<> countDist(1, 3);  // ���� ������ �������� ����
    uniform_int_distribution<> totalCountDist(3, 10);  // �� ���� ���� ����

    questionVec.clear();  // ���� ���� �ʱ�ȭ
    int totalFruits = totalCountDist(gen);  // �� ���� ���� ���� ����

    // ���� ���� ����
    while (totalFruits > 0)
    {
        int fruitIndex = fruitDist(gen);  // �������� ���� ����
        char fruit = fruitTypes[fruitIndex].first;  // ���� Ű ����
        int count = min(countDist(gen), totalFruits);  // ���� ���� ���� ���� (���� �������� ���� ����)

        // �̹� �ش� ������ ������ ������ ������ ����
        auto it = find_if(questionVec.begin(), questionVec.end(),
            [fruit](const pair<char, int>& p) { return p.first == fruit; });

        if (it != questionVec.end())
        {
            it->second += count;
        }
        else
        {
            questionVec.emplace_back(fruit, count);  // ���ϰ� ������ ���� ���Ϳ� �߰�
        }
        totalFruits -= count;  // ���� ���� ���� ����
    }
}

// ���� ���͸� ���ڿ��� ��ȯ�ϴ� �Լ�
void VectorToString(const vector<pair<char, int>>& questionVec, string& questionStr)
{
    questionStr.clear();  // ���� ���ڿ� ����
    // �� ���Ͽ� ���� ���ϸ�� ������ ���ڿ��� ����
    for (const auto& fruit : questionVec)
    {
        auto it = find_if(fruitTypes.begin(), fruitTypes.end(),
            [&fruit](const pair<char, string>& type) { return type.first == fruit.first; });

        if (it != fruitTypes.end())
        {
            ostringstream oss;
            oss << it->second << fruit.second << " ";  // ���� �̸��� ���� ��ġ��
            questionStr += oss.str();  // ��� ���ڿ��� �߰�
        }
    }
}

// ����ڰ� �Է��� ���� �´��� Ȯ���ϴ� �Լ�
bool CheckAnswer(const vector<pair<char, int>>& questionVec, const string& userInput)
{
    string correctAnswer;
    // �ùٸ� ���� ���ڿ��� ����
    for (const auto& fruit : questionVec)
    {
        correctAnswer += string(fruit.second, fruit.first);  // ���� Ű�� ������ ���� ����
    }
    return correctAnswer == userInput;  // ����� �Է��� ����� ��ġ�ϸ� true ��ȯ
}

// ���� ���� �� ȭ���� �׸��� �Լ�
void DrawStartGame(int sum, const string& questionStr, const string& answerStr)
{
    system("cls");
    gotoxy(5, 1);
    cout << "*******************************************";
    gotoxy(8, 3);
    cout << "sum: " << sum;

    gotoxy(8, 5);
    cout << "Q: ���� W: �� E: �ٳ��� R: ����";

    gotoxy(8, 8);
    cout << "�մ� : " << questionStr;

    gotoxy(8, 10);
    cout << "A: " << answerStr;

    gotoxy(8, 12);
    cout << "Press SPACE to submit your answer.";  // ����ڰ� ���� �����ϵ��� �ȳ�
    gotoxy(5, 18);
    cout << "*******************************************" << endl;
}

// ���� ���� ȭ���� �׸��� �Լ�
void DrawGameOver(const string& message)
{
    system("cls");
    gotoxy(20, 8);
    cout << "-------------------";
    gotoxy(20, 9);
    cout << "| " << message << " |";
    gotoxy(20, 10);
    cout << "-------------------";
    system("pause>null");  // ���� �� �ƹ� Ű�� �������� �ȳ�
}

// ���� ���� �Լ�
void StartGame()
{
    int sum = INITIAL_SUM;  // �ʱ� �ݾ� ����
    while (true)
    {
        vector<pair<char, int>> questionVec;  // ������ ������ ����
        string questionStr, answerStr;  // ���� ���ڿ��� ����ڰ� �Է��� ��

        GenerateQuestion(questionVec);  // ���� ����
        VectorToString(questionVec, questionStr);  // ������ ���ڿ��� ��ȯ

        while (true)
        {
            DrawStartGame(sum, questionStr, answerStr);  // ���� ȭ�� ���

            if (sum <= LOSS_SUM)  // �ݾ��� 0 ������ ��� ���� ����
            {
                DrawGameOver("BAD");
                return;
            }
            else if (sum >= WIN_SUM)  // �ݾ��� 10,000 �̻��� ��� �¸�
            {
                DrawGameOver("GOOD");
                return;
            }

            int input = _getch();
            input = toupper(input);  // ��ҹ��� ��� ���

            if (input == 'Q' || input == 'W' || input == 'E' || input == 'R') // ��ȿ�� ���� �Է�
            {
                answerStr += static_cast<char>(input);
            }
            else if (input == SPACE)  // �� ����
            {
                if (CheckAnswer(questionVec, answerStr))  // ������ ���
                {
                    sum += 500;  // ���� ����
                }
                else  // ������ ���
                {
                    sum -= 500;  // ���� ����
                }
                break;  // ���� ������ �̵�
            }
            else if (input == '\b' && !answerStr.empty())  // BACKSPACE �Է� ó��
            {
                answerStr.pop_back();  // �Է� ���ڿ����� ������ ���� ����
            }
        }
    }
}
int main()
{
    SetConsoleView();  // �ܼ� ����
    int menuSelection = 0;  // �޴� ���� �ʱ�ȭ

    while (true)
    {
        DrawReadyGame();  // ���� ���� ȭ�� ���
        int input = _getch();

        if (input == MAGIC_KEY)  // ����Ű �Է� ó��
        {
            input = _getch();  // ����Ű �� �б�
            if (input == 72)  // ���� ����Ű
                menuSelection = (menuSelection + 2) % 3;
            else if (input == 80)  // �Ʒ��� ����Ű
                menuSelection = (menuSelection + 1) % 3;
        }
        else if (input == ENTER)  // Enter Ű �Է� ó��
        {
            switch (menuSelection)
            {
            case GAMESTART:
                StartGame();
                break;
            case INFO:
                DrawInfoGame();
                break;
            case QUIT:
                return 0;
            }
        }
    }
}