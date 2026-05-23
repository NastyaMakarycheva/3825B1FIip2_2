#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <limits> 

class NumberGenerator {
private:
    int length;

    void swap(int& a, int& b) {
        int temp = a;
        a = b;
        b = temp;
    }

    void shuffle(std::vector<int>& digits) {
        for (int i = digits.size() - 1; i > 0; --i) { 
            int j = rand() % (i + 1); 
            swap(digits[i], digits[j]); 
        }
    }

public:
    NumberGenerator(int n) : length(n) {
    }

    std::string generate() {
        std::vector<int> digits = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }; 
         
        shuffle(digits); 

        if (length > 1 && digits[0] == 0) {
            for (int i = 1; i < 10; ++i) {
                if (digits[i] != 0) { 
                    swap(digits[0], digits[i]); 
                    break;
                }
            }
        }

        std::string result; 
        for (int i = 0; i < length; ++i) {
            result += static_cast<char>(digits[i] + '0');
        }
        return result;
    }
};

class GameLogic {
public:
    struct BullsAndCows {
        int bulls;
        int cows;
    };

    static bool isValidInput(const std::string& input, int length) {
        if (input.length() != length) return false;

        for (char c : input) { 
            if (!std::isdigit(c)) return false; 
        }

        if (length > 1 && input[0] == '0') { 
            return false; 
        }

        std::vector<bool> seen(10, false);
        for (char c : input) {
            int digit = c - '0'; 
            if (seen[digit]) return false; 
            seen[digit] = true; 
        }
        return true; 
    }

    static BullsAndCows calculateBullsAndCows(const std::string& guess, const std::string& secret) {
        BullsAndCows result{ 0, 0 };
        int length = secret.length();

        std::vector<bool> secretUsed(length, false); 
        std::vector<bool> guessUsed(length, false); 

        for (int i = 0; i < length; ++i) { 
            if (guess[i] == secret[i]) { 
                result.bulls++;
                secretUsed[i] = true; 
                guessUsed[i] = true; 
            }
        }

        for (int i = 0; i < length; ++i) {
            if (guessUsed[i]) continue; 
            for (int j = 0; j < length; ++j) {
                if (!secretUsed[j] && guess[i] == secret[j]) { 
                    result.cows++;
                    secretUsed[j] = true; 
                    break;
                }
            }
        }

        return result;
    }
};

class UserInterface {
private: 
    static void clearInputStream() { 
        std::cin.clear(); 
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }

public:
    static int getLength() {
        int n;
        do { 
            std::cout << "Enter the length of the desired number (1-10): ";
            if (!(std::cin >> n)) { 
                std::cout << "Invalid input! Please enter a number.\n"; 
                clearInputStream(); 
                continue; 
            }
            if (n < 1 || n > 10) {
                std::cout << "The length should be between 1 and 10!\n";
            }
        } while (n < 1 || n > 10);
        clearInputStream(); 
        return n;
    }

    static std::string getGuess(int length) {
        std::string guess;
        do {
            std::cout << "Enter " << length << "-digit number with non-repeating digits: ";
            std::cin >> guess;
            if (!GameLogic::isValidInput(guess, length)) {
                std::cout << "Incorrect input! Try again.\n";
                clearInputStream(); 
            }
        } while (!GameLogic::isValidInput(guess, length));
        return guess;
    }

    static void showResult(const GameLogic::BullsAndCows& result) {
        std::cout << "Result: " << result.bulls << " bulls, "
            << result.cows << " cows\n";
    }

    static void showWinMessage(int attempts) {
        std::cout << "Congratulations! You guessed the number for " << attempts << " attempts!\n";
    }
};

class BullsAndCowsGame {
private:
    int length;
    std::string secretNumber;
    int attempts;

public:
    BullsAndCowsGame(int n) : length(n), attempts(0) {
        NumberGenerator generator(n);
        secretNumber = generator.generate();
    }

    void play() {
        std::cout << "The game 'Bulls and Cows' has begun!\n";
        std::cout << "The computer made a wish " << length << "-digit number.\n";

        while (true) {
            attempts++;
            std::string guess = UserInterface::getGuess(length);
            GameLogic::BullsAndCows result = GameLogic::calculateBullsAndCows(guess, secretNumber);

            UserInterface::showResult(result); 

            if (result.bulls == length) { 
                UserInterface::showWinMessage(attempts); 
                break; 
            }
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    int n = UserInterface::getLength();
    BullsAndCowsGame game(n);
    game.play();
    return 0;
}
