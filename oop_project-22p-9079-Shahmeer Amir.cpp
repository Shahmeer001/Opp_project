#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>
using namespace std;

const int SCREEN_WIDTH = 120;
const int SCREEN_HEIGHT = 30;

class Player {
private:
    string name;
    int score;

public:
    Player(const string& playerName) : name(playerName), score(0) {}

    string getName() const {
        return name;
    }

    int getScore() const {
        return score;
    }

    void increaseScore(int points) {
        score += points;
    }

    // Friend function declaration
    friend void displayPlayerScore(const Player& player);
};

// Friend function definition
void displayPlayerScore(const Player& player) {
    cout << right << setw(80) << "Player: " << player.getName() << endl;
    cout << right << setw(80) << "Score: " << player.getScore() << endl;
    cout << endl;
}

void clearScreen() {
#ifdef __WIN32
    system("cls");
#elif __linux__
    system("clear");
#endif
}

void displayBanner() {
    cout << right << setw(80) << "********************************************" << endl;
    cout << right << setw(80) << "*              WELCOME TO THE              *" << endl;
    cout << right << setw(80) << "*            QUIZ MASTER PROGRAM           *" << endl;
    cout << right << setw(80) << "********************************************" << endl;
    cout << endl;
}

void printColoredStars(int numStars) {
    string colors[] = { "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m" }; //ascii space sequence
    string resetColor = "\033[0m";

    srand(time(0));

    for (int i = 0; i < numStars; i++) {
        int colorIndex = rand() % (sizeof(colors) / sizeof(colors[0])); // random color selector
        int xPos = rand() % SCREEN_WIDTH;
        int yPos = rand() % SCREEN_HEIGHT; //random position selector

        cout << "\033[" << yPos << ";" << xPos << "H";
        cout << colors[colorIndex] << "*" << resetColor;
    }
    //
    cout << "\033[0;0H"; //white on black
}

class Question {
protected:
    string question;

public:
    Question(const string& questionText) : question(questionText) {}

    virtual void displayQuestion() const = 0;
    virtual bool checkAnswer(int userChoice) const = 0;
};

class MultipleChoiceQuestion : public Question {
private:
    string* options;
    int optionCount;
    int correctOption;

public:
    MultipleChoiceQuestion(const string& questionText, string* options, int optionCount, int correctOption)
        : Question(questionText), options(options), optionCount(optionCount), correctOption(correctOption) {}

    ~MultipleChoiceQuestion() {
        delete[] options;
    }

    void displayQuestion() const override {
        cout << right << setw(63) << question << endl;
        for (int i = 0; i < optionCount; i++) {
            cout << setw(35) << i + 1 << ". " << options[i] << endl;
        }
    }

    bool checkAnswer(int userChoice) const override {
        return (userChoice == correctOption);
    }
};

class TrueFalseQuestion : public Question {
private:
    bool correctAnswer;

public:
    TrueFalseQuestion(const string& questionText, bool correctAnswer)
        : Question(questionText), correctAnswer(correctAnswer) {}

    void displayQuestion() const override {
        cout << right << setw(50) << question << " (1 for True / 0 for False)" << endl;
    }

    bool checkAnswer(int userChoice) const override {
        bool userAnswer = (userChoice == 1);
        return (userAnswer == correctAnswer);
    }
};

class Quiz {
private:
    Question** questions;
    int questionCount;

public:
    Quiz() : questions(nullptr), questionCount(0) {}

    ~Quiz() {
        for (int i = 0; i < questionCount; i++) {
            delete questions[i];
        }
        delete[] questions;
    }

    void addQuestion(Question* question) {
        Question** temp = new Question*[questionCount + 1];
        for (int i = 0; i < questionCount; i++) {
            temp[i] = questions[i];
        }
        temp[questionCount] = question;
        delete[] questions;
        questions = temp;
        questionCount++;
    }

    void startQuiz(Player& player) {
        for (int i = 0; i < questionCount; i++) {
            if (i != 0)
                printColoredStars(100);
            displayPlayerScore(player); // Display player's score and name before each question
            questions[i]->displayQuestion();

            int userChoice;
            cout << right << setw(52) << "Enter your answer: ";
            cin >> userChoice;

            if (questions[i]->checkAnswer(userChoice)) {
                cout << right << setw(45) << "Correct!" << endl;
                player.increaseScore(1);
            }
            else {
                cout << right << setw(45) << "Incorrect!" << endl;
            }
            _sleep(1000);
            clearScreen();
        }

        cout << "Quiz completed. Your score: " << player.getScore() << "/" << questionCount << endl;
    }
};

void saveQuizResults(const Player& player, const string& filename) {
    try {
        ofstream file(filename, ios::app);

        if (file.is_open()) {
            file << "Player: " << player.getName() << endl;
            file << "Score: " << player.getScore() << endl;
            file.close();
            cout << "Quiz results saved to " << filename << endl;
        }
        else {
            throw ("Failed to open the file: " + filename);
        }
    }
    catch (const exception& e) {
        cout << "Exception caught: " << e.what() << endl;
    }
}

void playQuiz() {
    clearScreen();
    Quiz quiz;

    // Add more questions
    MultipleChoiceQuestion q1("What is the capital of Pakistan?", new string[4]{"Berlin", "Lahore", "Jhelum", "Islamabad" }, 4, 4);
    TrueFalseQuestion q2("Is the sun a star?", true);
    MultipleChoiceQuestion q3("Which is the largest ocean in the world?", new string[4]{ "Pacific Ocean", "Indian Ocean", "Atlantic Ocean", "Arctic Ocean" }, 4, 1);
    TrueFalseQuestion q4("Is C++ a programming language?", true);
    MultipleChoiceQuestion q5("What is the color of the sky?", new string[3]{ "Red", "Blue", "Green" }, 3, 2); // New easy question
    TrueFalseQuestion q6("Is water wet?", true); // New easy question

    quiz.addQuestion(&q1);
    quiz.addQuestion(&q2);
    quiz.addQuestion(&q3);
    quiz.addQuestion(&q4);
    quiz.addQuestion(&q5); // Add the new easy question
    quiz.addQuestion(&q6); // Add the new easy question

    do {
        printColoredStars(100);
        displayBanner();
        string playerName;
        cout << right << setw(60) << "Enter your name: ";
        cin >> playerName;
        clearScreen();
        printColoredStars(100);

        Player player(playerName);
        cout << right << setw(60) << "Let's start the quiz!" << endl;
        quiz.startQuiz(player);
        printColoredStars(100);
        cout << "Player name: " << player.getName() << endl;
        cout << "Final Score: " << player.getScore() << endl;
        saveQuizResults(player, "quiz_results.txt");

        char playAgain;
        do {
            cout << "Do you want to play again? (Y/N): ";
            cin >> playAgain;
            clearScreen();
            printColoredStars(100);
            if (!(toupper(playAgain) == 'Y' || toupper(playAgain) == 'N')) {
                cout << "Incorrect Option! Try again" << endl;
            }
        } while (!(toupper(playAgain) == 'Y' || toupper(playAgain) == 'N'));

        if (toupper(playAgain) == 'N') {
            cout << "Good Bye!" << endl;
            return;
        }

    } while (true);
}

int main() {
    playQuiz();
   
}

