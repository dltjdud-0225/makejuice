#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <sstream>

using namespace std;

#define ENTER 13             // Enter 키의 ASCII 값
#define SPACE 32             // Space 키의 ASCII 값
#define INITIAL_SUM 1000     // 게임 시작 시 초기 금액
#define WIN_SUM 10000        // 승리 조건 금액
#define LOSS_SUM 0           // 패배 조건 금액
#define MAGIC_KEY 224        // 방향키가 눌렸을 때 나타나는 특수 키 값

// 메뉴 항목을 나열하는 enum
enum MENU
{
    GAMESTART = 0,  // 게임 시작
    INFO,           // 게임 정보
    QUIT            // 게임 종료
};

// 게임에 사용될 과일의 정보들을 저장한 네임스페이스
namespace GameConstants {
    const vector<pair<char, string>> fruitTypes = {
        {'Q', "딸기"}, {'W', "귤"}, {'E', "바나나"}, {'R', "라임"}  // 과일과 그에 대응되는 키 값
    };
}

using GameConstants::fruitTypes;  // fruitTypes 사용

// 콘솔 커서를 (x, y) 좌표로 이동시키는 함수
void gotoxy(int x, int y)
{
    COORD Pos;
    Pos.X = x;  // X 좌표 설정
    Pos.Y = y;  // Y 좌표 설정
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);  // 커서 위치 변경
}

// 콘솔 화면 설정 함수 (크기, 제목 설정)
void SetConsoleView()
{
    system("mode con:cols=70 lines=30");  // 콘솔 화면 크기 설정 (가로 70, 세로 30)
    system("title Fruit Juice");          // 콘솔 창 제목 설정
}

// 게임 시작 화면을 그리는 함수
void DrawReadyGame()
{
    system("cls");  // 화면을 클리어
    gotoxy(20, 2);  // 화면 좌표로 커서 이동
    cout << "******************************";
    gotoxy(20, 3);
    cout << "*       Make Juice           *";
    gotoxy(20, 4);
    cout << "******************************";
    gotoxy(25, 8);  // 메뉴 항목에 커서 표시
    cout << "Game Start";
    gotoxy(25, 9);
    cout << "Game Info";
    gotoxy(25, 10);
    cout << "Quit" << endl;
}

// 게임 정보 화면을 그리는 함수
void DrawInfoGame()
{
    system("cls");
    gotoxy(10, 3);
    cout << "*******************************************";
    gotoxy(10, 4);
    cout << "| 키 설명: Q:딸기 W:귤 E:바나나 R:라임     |";
    gotoxy(10, 5);
    cout << "| 성공: +500 포인트  실패: -500 포인트     |";
    gotoxy(10, 6);
    cout << "*******************************************";
    gotoxy(10, 7);
    cout << "| 종료 조건:                              |";
    gotoxy(10, 8);
    cout << "| 1) 합계 = 0: \"BAD\"                     |";
    gotoxy(10, 9);
    cout << "| 2) 합계 = 10,000: \"GOOD\"               |";
    gotoxy(10, 10);
    cout << "*******************************************";
    gotoxy(10, 11);
    cout << "계속 하려면 아무 키나 누르십시오 . . .";
    system("pause>null");  // 아무 키나 눌러서 계속 진행
}

// 문제 생성 함수 (랜덤 과일 문제 생성)
void GenerateQuestion(vector<pair<char, int>>& questionVec)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> fruitDist(0, fruitTypes.size() - 1);  // 과일을 랜덤으로 뽑기 위한 분포
    uniform_int_distribution<> countDist(1, 3);  // 과일 개수를 랜덤으로 결정
    uniform_int_distribution<> totalCountDist(3, 10);  // 총 과일 개수 설정

    questionVec.clear();  // 문제 벡터 초기화
    int totalFruits = totalCountDist(gen);  // 총 과일 개수 랜덤 결정

    // 과일 문제 생성
    while (totalFruits > 0)
    {
        int fruitIndex = fruitDist(gen);  // 랜덤으로 과일 선택
        char fruit = fruitTypes[fruitIndex].first;  // 과일 키 선택
        int count = min(countDist(gen), totalFruits);  // 과일 개수 랜덤 선택 (남은 개수보다 적게 설정)

        // 이미 해당 과일이 문제에 있으면 개수만 증가
        auto it = find_if(questionVec.begin(), questionVec.end(),
            [fruit](const pair<char, int>& p) { return p.first == fruit; });

        if (it != questionVec.end())
        {
            it->second += count;
        }
        else
        {
            questionVec.emplace_back(fruit, count);  // 과일과 개수를 문제 벡터에 추가
        }
        totalFruits -= count;  // 남은 과일 개수 차감
    }
}

// 문제 벡터를 문자열로 변환하는 함수
void VectorToString(const vector<pair<char, int>>& questionVec, string& questionStr)
{
    questionStr.clear();  // 기존 문자열 비우기
    // 각 과일에 대해 과일명과 개수를 문자열로 연결
    for (const auto& fruit : questionVec)
    {
        auto it = find_if(fruitTypes.begin(), fruitTypes.end(),
            [&fruit](const pair<char, string>& type) { return type.first == fruit.first; });

        if (it != fruitTypes.end())
        {
            ostringstream oss;
            oss << it->second << fruit.second << " ";  // 과일 이름과 개수 합치기
            questionStr += oss.str();  // 결과 문자열에 추가
        }
    }
}

// 사용자가 입력한 답이 맞는지 확인하는 함수
bool CheckAnswer(const vector<pair<char, int>>& questionVec, const string& userInput)
{
    string correctAnswer;
    // 올바른 정답 문자열을 생성
    for (const auto& fruit : questionVec)
    {
        correctAnswer += string(fruit.second, fruit.first);  // 과일 키와 개수로 정답 생성
    }
    return correctAnswer == userInput;  // 사용자 입력이 정답과 일치하면 true 반환
}

// 게임 진행 중 화면을 그리는 함수
void DrawStartGame(int sum, const string& questionStr, const string& answerStr)
{
    system("cls");
    gotoxy(5, 1);
    cout << "*******************************************";
    gotoxy(8, 3);
    cout << "sum: " << sum;

    gotoxy(8, 5);
    cout << "Q: 딸기 W: 귤 E: 바나나 R: 라임";

    gotoxy(8, 8);
    cout << "손님 : " << questionStr;

    gotoxy(8, 10);
    cout << "A: " << answerStr;

    gotoxy(8, 12);
    cout << "Press SPACE to submit your answer.";  // 사용자가 답을 제출하도록 안내
    gotoxy(5, 18);
    cout << "*******************************************" << endl;
}

// 게임 종료 화면을 그리는 함수
void DrawGameOver(const string& message)
{
    system("cls");
    gotoxy(20, 8);
    cout << "-------------------";
    gotoxy(20, 9);
    cout << "| " << message << " |";
    gotoxy(20, 10);
    cout << "-------------------";
    system("pause>null");  // 종료 후 아무 키나 누르도록 안내
}

// 게임 시작 함수
void StartGame()
{
    int sum = INITIAL_SUM;  // 초기 금액 설정
    while (true)
    {
        vector<pair<char, int>> questionVec;  // 문제를 저장할 벡터
        string questionStr, answerStr;  // 문제 문자열과 사용자가 입력한 답

        GenerateQuestion(questionVec);  // 문제 생성
        VectorToString(questionVec, questionStr);  // 문제를 문자열로 변환

        while (true)
        {
            DrawStartGame(sum, questionStr, answerStr);  // 게임 화면 출력

            if (sum <= LOSS_SUM)  // 금액이 0 이하일 경우 게임 오버
            {
                DrawGameOver("BAD");
                return;
            }
            else if (sum >= WIN_SUM)  // 금액이 10,000 이상일 경우 승리
            {
                DrawGameOver("GOOD");
                return;
            }

            int input = _getch();
            input = toupper(input);  // 대소문자 모두 허용

            if (input == 'Q' || input == 'W' || input == 'E' || input == 'R') // 유효한 과일 입력
            {
                answerStr += static_cast<char>(input);
            }
            else if (input == SPACE)  // 답 제출
            {
                if (CheckAnswer(questionVec, answerStr))  // 정답일 경우
                {
                    sum += 500;  // 점수 증가
                }
                else  // 오답일 경우
                {
                    sum -= 500;  // 점수 감소
                }
                break;  // 다음 문제로 이동
            }
            else if (input == '\b' && !answerStr.empty())  // BACKSPACE 입력 처리
            {
                answerStr.pop_back();  // 입력 문자열에서 마지막 문자 제거
            }
        }
    }
}
int main()
{
    SetConsoleView();  // 콘솔 설정
    int menuSelection = 0;  // 메뉴 선택 초기화

    while (true)
    {
        DrawReadyGame();  // 게임 시작 화면 출력
        int input = _getch();

        if (input == MAGIC_KEY)  // 방향키 입력 처리
        {
            input = _getch();  // 방향키 값 읽기
            if (input == 72)  // 위쪽 방향키
                menuSelection = (menuSelection + 2) % 3;
            else if (input == 80)  // 아래쪽 방향키
                menuSelection = (menuSelection + 1) % 3;
        }
        else if (input == ENTER)  // Enter 키 입력 처리
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