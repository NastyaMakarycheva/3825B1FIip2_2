#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>

using namespace std;

struct Date {
    int day, month, year;
    Date() : day(0), month(0), year(0) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}
};

struct Time {
    int hour, minute;
    Time() : hour(0), minute(0) {}
    Time(int h, int m) : hour(h), minute(m) {}
};

struct Seat {
    int row;
    int number;
    bool isVip;
    bool isFree;
    Seat() : row(0), number(0), isVip(false), isFree(true) {}
};

struct Ticket {
    Date date;
    Time time;
    string filmName;
    int hallNumber;
    Seat seat;
    double price;
    Ticket() : hallNumber(0), price(0.0) {}
};

struct Session {
    Date date;
    Time time;
    string filmName;
    int hallNumber;
    double basePrice;
    vector<vector<Seat> > seats;
    Session() : hallNumber(0), basePrice(0.0) {}
};

Date getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);

    Date current;
    current.day = now_tm->tm_mday;
    current.month = now_tm->tm_mon + 1;
    current.year = now_tm->tm_year + 1900;
    return current;
}

Time getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);

    Time current;
    current.hour = now_tm->tm_hour;
    current.minute = now_tm->tm_min;
    return current;
}

bool isDateEqual(Date d1, Date d2) {
    return d1.day == d2.day && d1.month == d2.month && d1.year == d2.year;
}

class Cinema {
private:
    vector<Session> sessions;

    int daysDifference(Date d1, Date d2) {
        if (d1.year != d2.year) return 365;
        if (d1.month != d2.month) return 30;
        return d2.day - d1.day;
    }

    double getPriceByTime(double basePrice, Time time) {
        if (time.hour < 12) {
            return basePrice * 0.75;
        }
        else if (time.hour <= 18) {
            return basePrice;
        }
        else {
            return basePrice * 1.5;
        }
    }

    bool canSellForSession(Date sessionDate, Time sessionTime) {
        Date currentDate = getCurrentDate();
        Time currentTime = getCurrentTime();
        int daysDiff = daysDifference(currentDate, sessionDate);

        if (daysDiff < 0 || daysDiff > 3) {
            return false;
        }

        if (isDateEqual(sessionDate, currentDate)) {
            int currentMinutes = currentTime.hour * 60 + currentTime.minute;
            int sessionMinutes = sessionTime.hour * 60 + sessionTime.minute;

            if (currentMinutes > sessionMinutes + 10) {
                return false;
            }
        }

        return true;
    }

public:
    bool addSession(Date date, Time time, string filmName, int hallNumber,
        int rows, int seatsPerRow, double basePrice) {

        Date currentDate = getCurrentDate();
        int daysDiff = daysDifference(currentDate, date);

        if (daysDiff < 0 || daysDiff > 30) {
            cout << "Error: Session must be within 30 days from current date" << endl;
            return false;
        }

        Session s;
        s.date = date;
        s.time = time;
        s.filmName = filmName;
        s.hallNumber = hallNumber;
        s.basePrice = basePrice;

        s.seats.resize(rows);
        for (int i = 0; i < rows; i++) {
            s.seats[i].resize(seatsPerRow);
            for (int j = 0; j < seatsPerRow; j++) {
                s.seats[i][j].row = i + 1;
                s.seats[i][j].number = j + 1;
                s.seats[i][j].isFree = true;
                if (i < 2 || j >= seatsPerRow - 2) {
                    s.seats[i][j].isVip = true;
                }
                else {
                    s.seats[i][j].isVip = false;
                }
            }
        }

        sessions.push_back(s);
        return true;
    }

    int findSession(Date date, Time time, string filmName, int hallNumber) {
        for (int i = 0; i < sessions.size(); i++) {
            if (sessions[i].date.day == date.day &&
                sessions[i].date.month == date.month &&
                sessions[i].date.year == date.year &&
                sessions[i].time.hour == time.hour &&
                sessions[i].time.minute == time.minute &&
                sessions[i].filmName == filmName &&
                sessions[i].hallNumber == hallNumber) {
                return i;
            }
        }
        return -1;
    }

    void showAllSessions() {
        if (sessions.empty()) {
            cout << "No available sessions" << endl;
            return;
        }

        Date currentDate = getCurrentDate();
        Time currentTime = getCurrentTime();

        cout << "\nAvailable sessions (next 30 days)" << endl;
        for (int i = 0; i < sessions.size(); i++) {
            bool isAvailable = canSellForSession(sessions[i].date, sessions[i].time);

            cout << i + 1 << ". Film: " << sessions[i].filmName
                << ", Date: " << sessions[i].date.day << "." << sessions[i].date.month << "." << sessions[i].date.year
                << ", Time: " << sessions[i].time.hour << ":"
                << (sessions[i].time.minute < 10 ? "0" : "") << sessions[i].time.minute
                << ", Hall: " << sessions[i].hallNumber
                << ", Base price: " << sessions[i].basePrice << " rub."
                << (isAvailable ? " (available)" : " (not available for sale)") << endl;
        }
    }

    bool canSellTickets(int sessionIndex, bool isVip, int count) {
        if (sessionIndex == -1) return false;

        if (!canSellForSession(sessions[sessionIndex].date, sessions[sessionIndex].time)) {
            cout << "Error: Tickets can only be sold for sessions within 3 days and no later than 10 minutes after start" << endl;
            return false;
        }

        int freeCount = 0;
        for (int i = 0; i < sessions[sessionIndex].seats.size(); i++) {
            for (int j = 0; j < sessions[sessionIndex].seats[i].size(); j++) {
                if (sessions[sessionIndex].seats[i][j].isFree &&
                    sessions[sessionIndex].seats[i][j].isVip == isVip) {
                    freeCount++;
                }
            }
        }
        return freeCount >= count;
    }

    vector<Seat> reserveSeats(int sessionIndex, bool isVip, int count) {
        vector<Seat> reserved;

        for (int i = 0; i < sessions[sessionIndex].seats.size() && reserved.size() < count; i++) {
            for (int j = 0; j < sessions[sessionIndex].seats[i].size() && reserved.size() < count; j++) {
                if (sessions[sessionIndex].seats[i][j].isFree &&
                    sessions[sessionIndex].seats[i][j].isVip == isVip) {
                    sessions[sessionIndex].seats[i][j].isFree = false;
                    reserved.push_back(sessions[sessionIndex].seats[i][j]);
                }
            }
        }
        return reserved;
    }

    void freeSeats(int sessionIndex, vector<Seat> seatsToFree) {
        for (int i = 0; i < seatsToFree.size(); i++) {
            for (int r = 0; r < sessions[sessionIndex].seats.size(); r++) {
                for (int c = 0; c < sessions[sessionIndex].seats[r].size(); c++) {
                    if (sessions[sessionIndex].seats[r][c].row == seatsToFree[i].row &&
                        sessions[sessionIndex].seats[r][c].number == seatsToFree[i].number) {
                        sessions[sessionIndex].seats[r][c].isFree = true;
                    }
                }
            }
        }
    }

    double getTicketPrice(int sessionIndex, bool isVip) {
        if (sessionIndex == -1) return 0;
        double price = getPriceByTime(sessions[sessionIndex].basePrice, sessions[sessionIndex].time);
        if (isVip) {
            price = price * 2;
        }
        return price;
    }

    Session getSession(int sessionIndex) {
        return sessions[sessionIndex];
    }

    int getSessionsCount() {
        return sessions.size();
    }
};

class TicketOffice {
private:
    Cinema* cinema;

    struct Order {
        int sessionIndex;
        vector<Seat> seats;
        double totalPrice;
        string filmName;
        Date date;
        Time time;
        int hallNumber;
        Order() : sessionIndex(-1), totalPrice(0.0), hallNumber(0) {}
    };

    Order currentOrder;
    bool hasOrder;

public:
    TicketOffice(Cinema* c) {
        cinema = c;
        hasOrder = false;
    }

    void showMenu() {
        cout << "\nMenu" << endl;
        cout << "1. View all sessions" << endl;
        cout << "2. Buy tickets" << endl;
        cout << "3. Cancel order" << endl;
        cout << "4. Print tickets" << endl;
        cout << "5. Exit" << endl;
        cout << "Your choice: ";
    }

    bool acceptOrder(Date date, Time time, string filmName, int hallNumber,
        bool isVip, int seatCount) {

        int sessionIndex = cinema->findSession(date, time, filmName, hallNumber);
        if (sessionIndex == -1) {
            cout << "Error: Session not found" << endl;
            return false;
        }

        if (!cinema->canSellTickets(sessionIndex, isVip, seatCount)) {
            cout << "Error: Not enough free seats in the selected zone or session is not available for sale" << endl;
            return false;
        }

        vector<Seat> seats = cinema->reserveSeats(sessionIndex, isVip, seatCount);

        double pricePerTicket = cinema->getTicketPrice(sessionIndex, isVip);
        double totalPrice = pricePerTicket * seatCount;

        currentOrder.sessionIndex = sessionIndex;
        currentOrder.seats = seats;
        currentOrder.totalPrice = totalPrice;
        currentOrder.filmName = filmName;
        currentOrder.date = date;
        currentOrder.time = time;
        currentOrder.hallNumber = hallNumber;
        hasOrder = true;

        cout << "\nOrder successfully placed!" << endl;
        cout << "Number of seats: " << seatCount << endl;
        cout << "Zone: " << (isVip ? "VIP" : "Regular") << endl;
        cout << "Price per ticket: " << pricePerTicket << " rub." << endl;
        cout << "Total cost: " << totalPrice << " rub." << endl;
        return true;
    }

    void cancelOrder() {
        if (!hasOrder) {
            cout << "No active order to cancel" << endl;
            return;
        }

        cinema->freeSeats(currentOrder.sessionIndex, currentOrder.seats);
        hasOrder = false;
        cout << "Order successfully cancelled, seats have been freed" << endl;
    }

    void printTickets() {
        if (!hasOrder) {
            cout << "No active order to print tickets" << endl;
            return;
        }

        cout << "\nYour tickets" << endl;
        for (int i = 0; i < currentOrder.seats.size(); i++) {
            cout << "\nTicket #" << i + 1 << endl;
            cout << "  Film: " << currentOrder.filmName << endl;
            cout << "  Date: " << currentOrder.date.day << "."
                << currentOrder.date.month << "." << currentOrder.date.year << endl;
            cout << "  Time: " << currentOrder.time.hour << ":"
                << (currentOrder.time.minute < 10 ? "0" : "") << currentOrder.time.minute << endl;
            cout << "  Hall: " << currentOrder.hallNumber << endl;
            cout << "  Row: " << currentOrder.seats[i].row
                << ", Seat: " << currentOrder.seats[i].number << endl;

            double price = cinema->getTicketPrice(currentOrder.sessionIndex,
                currentOrder.seats[i].isVip);
            cout << "  Price: " << price << " rub." << endl;
            cout << "  Zone: " << (currentOrder.seats[i].isVip ? "VIP" : "Regular") << endl;
        }
        cout << "Total to pay: " << currentOrder.totalPrice << " rub." << endl;
    }

    bool hasActiveOrder() {
        return hasOrder;
    }
};

void addTestSessions(Cinema& cinema) {
    Date currentDate = getCurrentDate();

    Date d1 = currentDate;
    Time t1 = getCurrentTime();
    t1.hour++; 
    if (t1.hour >= 24) {
        t1.hour = 23;
        t1.minute = 0;
    }

    Date d2 = currentDate;
    d2.day++;
    Time t2 = { 15, 30 };

    Date d3 = currentDate;
    d3.day += 2;
    Time t3 = { 20, 0 };

    Date d4 = currentDate;
    d4.day += 10;
    Time t4 = { 18, 0 };

    cinema.addSession(d1, t1, "Avatar", 1, 8, 10, 300);
    cinema.addSession(d2, t2, "Batman", 1, 8, 10, 300);
    cinema.addSession(d3, t3, "Dune", 2, 10, 12, 350);
    cinema.addSession(d4, t4, "Inception", 3, 8, 10, 400);
}

int main() {
    Cinema cinema;
    TicketOffice office(&cinema);

    addTestSessions(cinema);

    int choice;

    do {
        office.showMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            cinema.showAllSessions();
            break;
        case 2: {
            Date date;
            Time time;
            string filmName;
            int hallNumber;
            int zoneType;
            bool isVip;
            int seatCount;

            cout << "\nTicket purchase" << endl;
            cout << "Note: Tickets can only be sold for sessions within 3 days and no later than 10 minutes after start" << endl;

            cout << "Enter session date (day month year): ";
            cin >> date.day >> date.month >> date.year;

            cout << "Enter session time (hour minutes): ";
            cin >> time.hour >> time.minute;

            cout << "Enter film name: ";
            cin.ignore();
            getline(cin, filmName);

            cout << "Enter hall number: ";
            cin >> hallNumber;

            cout << "Select zone (1 - Regular, 2 - VIP): ";
            cin >> zoneType;
            isVip = (zoneType == 2);

            cout << "Enter number of tickets: ";
            cin >> seatCount;

            if (seatCount <= 0) {
                cout << "Error. Number of tickets must be positive" << endl;
                break;
            }

            office.acceptOrder(date, time, filmName, hallNumber, isVip, seatCount);
            break;
        }
        case 3:
            office.cancelOrder();
            break;
        case 4:
            office.printTickets();
            break;
        case 5:
            cout << "Exit" << endl;
            break;
        default:
            cout << "Invalid choice! Please try again (1-5)" << endl;
        }

    } while (choice != 5);

    return 0;
}