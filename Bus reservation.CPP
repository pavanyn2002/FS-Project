#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

// Structure to represent a reservation
struct Reservation {
    int seatNumber;
    string passengerName;
    string source;
    string destination;
};

// Function to display the seat chart
void displaySeatChart() {
    ifstream inFile("seats.txt");
    if (!inFile) {
        cout << "Error: Could not open file." << endl;
        return;
    }

    string line;
    while (getline(inFile, line)) {
        cout << line << endl;
    }

    inFile.close();
}

// Function to create the primary index file
void createPrimaryIndex() {
    ifstream inFile("seats.txt");
    if (!inFile) {
        cout << "Error: Could not open file." << endl;
        return;
    }

    ofstream indexFile("index.txt");
    if (!indexFile) {
        cout << "Error: Could not create index file." << endl;
        inFile.close();
        return;
    }

    string line;
    int pos = 0;
    int seatNumber = 0;
    while (getline(inFile, line)) {
        if (line.find("Seat Number: ") != string::npos) {
            seatNumber = stoi(line.substr(13));
            indexFile << seatNumber << " " << pos << endl;
        }

        pos = static_cast<int>(inFile.tellg());
    }

    inFile.close();
    indexFile.close();

    cout << "Primary index created successfully." << endl;
}

// Function to search for a reservation by seat number using primary index
void searchReservationBySeat() {
    int seatNumber;
    cout << "Enter seat number to search: ";
    cin >> seatNumber;

    ifstream indexFile("index.txt");
    if (!indexFile) {
        cout << "Error: Could not open index file." << endl;
        return;
    }

    string line;
    vector<pair<int, int>> indexData;
    while (getline(indexFile, line)) {
        int seat, pos;
        sscanf(line.c_str(), "%d %d", &seat, &pos);
        indexData.push_back(make_pair(seat, pos));
    }

    indexFile.close();

    bool seatFound = false;
    for (const auto& index : indexData) {
        if (index.first == seatNumber) {
            ifstream inFile("seats.txt");
            if (!inFile) {
                cout << "Error: Could not open file." << endl;
                return;
            }

            inFile.seekg(index.second);

            string reservationLine;
            for (int i = 0; i < 4; ++i) {
                getline(inFile, reservationLine);
                cout << reservationLine << endl;
            }

            inFile.close();

            seatFound = true;
            break;
        }
    }

    if (!seatFound) {
        cout << "Seat not found." << endl;
    }
}

// Function to modify a reservation by seat number
void modifyReservationBySeat() {
    int seatNumber;
    cout << "Enter seat number to modify: ";
    cin >> seatNumber;

    ifstream inFile("seats.txt");
    if (!inFile) {
        cout << "Error: Could not open file." << endl;
        return;
    }

    ofstream tempFile("temp.txt");
    if (!tempFile) {
        cout << "Error: Could not create temporary file." << endl;
        inFile.close();
        return;
    }

    bool seatFound = false;
    string line;
    while (getline(inFile, line)) {
        if (line.find("Seat Number: " + to_string(seatNumber)) != string::npos) {
            seatFound = true;

            Reservation reservation;
            reservation.seatNumber = seatNumber;

            cout << "Enter modified passenger name: ";
            getline(cin >> ws, reservation.passengerName);

            cout << "Enter modified source: ";
            getline(cin, reservation.source);

            cout << "Enter modified destination: ";
            getline(cin, reservation.destination);

            tempFile << "Seat Number: " << reservation.seatNumber << endl;
            tempFile << "Passenger Name: " << reservation.passengerName << endl;
            tempFile << "Source: " << reservation.source << endl;
            tempFile << "Destination: " << reservation.destination << endl;
            tempFile << endl;
        } else {
            tempFile << line << endl;
        }
    }

    inFile.close();
    tempFile.close();

    if (!seatFound) {
        cout << "Seat not found." << endl;
        remove("temp.txt");
        return;
    }

    remove("seats.txt");
    rename("temp.txt", "seats.txt");

    cout << "Reservation modified successfully." << endl;

    // Update the primary index after modification
    createPrimaryIndex();
}

// Function to reserve a seat
void reserveSeat() {
    // Check if the maximum number of seats has been reached
    ifstream inFile("seats.txt");
    int seatCount = 0;
    string line;
    while (getline(inFile, line)) {
        if (line.find("Seat Number: ") != string::npos) {
            seatCount++;
        }
    }
    inFile.close();

    if (seatCount >= 20) {
        cout << "Maximum number of seats reached. Reservation cannot be made." << endl;
        return;
    }

    // Continue with the reservation process
    ofstream outFile("seats.txt", ios::app);
    if (!outFile) {
        cout << "Error: Could not open file." << endl;
        return;
    }

    Reservation reservation;

    cout << "Enter seat number: ";
    cin >> reservation.seatNumber;

    // Check if the seat is available
    ifstream inFile2("seats.txt");
    if (inFile2) {
        string line;
        while (getline(inFile2, line)) {
            if (line.find("Seat Number: " + to_string(reservation.seatNumber)) != string::npos) {
                cout << "Seat already reserved." << endl;
                outFile.close();
                inFile2.close();
                return;
            }
        }
        inFile2.close();
    }

    cout << "Enter passenger name: ";
    cin.ignore();
    getline(cin, reservation.passengerName);

    cout << "Enter source: ";
    getline(cin, reservation.source);

    cout << "Enter destination: ";
    getline(cin, reservation.destination);

    outFile << "Seat Number: " << reservation.seatNumber << endl;
    outFile << "Passenger Name: " << reservation.passengerName << endl;
    outFile << "Source: " << reservation.source << endl;
    outFile << "Destination: " << reservation.destination << endl;
    outFile << endl;

    outFile.close();

    cout << "Seat reserved successfully." << endl;

    // Update the primary index after reservation
    createPrimaryIndex();
}

// Function to cancel a seat
void cancelSeat() {
    int seatNumber;
    cout << "Enter seat number to cancel: ";
    cin >> seatNumber;

    ifstream inFile("seats.txt");
    if (!inFile) {
        cout << "Error: Could not open file." << endl;
        return;
    }

    ofstream tempFile("temp.txt");
    if (!tempFile) {
        cout << "Error: Could not create temporary file." << endl;
        inFile.close();
        return;
    }

    bool seatFound = false;
    string line;
    while (getline(inFile, line)) {
        if (line.find("Seat Number: " + to_string(seatNumber)) != string::npos) {
            seatFound = true;
            for (int i = 0; i < 4; ++i) {
                getline(inFile, line);
            }
        } else {
            tempFile << line << endl;
        }
    }

    inFile.close();
    tempFile.close();

    if (!seatFound) {
        cout << "Seat not found." << endl;
        remove("temp.txt");
        return;
    }

    remove("seats.txt");
    rename("temp.txt", "seats.txt");

    cout << "Seat canceled successfully." << endl;

    // Update the primary index after cancellation
    createPrimaryIndex();
}

int main() {
    createPrimaryIndex();

    while (true) {
        cout << "1. Reserve a seat" << endl;
        cout << "2. Cancel a seat" << endl;
        cout << "3. Modify a reservation" << endl;
        cout << "4. Display seat chart" << endl;
        cout << "5. Search reservation by seat number" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                reserveSeat();
                break;
            case 2:
                cancelSeat();
                break;
            case 3:
                modifyReservationBySeat();
                break;
            case 4:
                displaySeatChart();
                break;
            case 5:
                searchReservationBySeat();
                break;
            case 6:
                cout << "Exiting program." << endl;
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }

        cout << endl;
    }

    return 0;
}
