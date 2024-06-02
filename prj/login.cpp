#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;


void mainMenu();
void userLogin();
void adminLogin();
void registerUser();
void userMainMenu(const string& username);
void adminMainMenu();
void changePassword(const string& username);

// Utility Functions
vector<string> split(const string& s, char delimiter);
bool userExists(const string& username);
bool verifyUser(const string& username, const string& password);
bool verifyAdmin(const string& username, const string& password);
void saveUser(const string& username, const string& password);

class Book {
public:
    string title;
    string author;
    string status;

    Book(const string& t, const string& a, const string& s) : title(t), author(a), status(s) {}

    string toString() const {
        return title + ";" + author + ";" + status;
    }
};

class Admin {
private:
    vector<Book> books;

    void loadBooks();
    void saveBooks();

public:
    Admin() { loadBooks(); }
    ~Admin() { saveBooks(); }
    void viewBooks();
    void addBook();
    void acceptRequest();
    void deleteUser();
};

class User {
private:
    string username;
    vector<Book> books;

    void loadBooks();
    void saveRequest(const string& bookTitle);

public:
    User(const string& user) : username(user) { loadBooks(); }
    void viewBooks();
    void borrowBook();
    void viewRequests();
    void changePassword();
};

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool userExists(const string& username) {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        vector<string> user = split(line, ';');
        if (user[0] == username) {
            return true;
        }
    }
    return false;
}

bool verifyUser(const string& username, const string& password) {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        vector<string> user = split(line, ';');
        if (user[0] == username && user[1] == password) {
            return true;
        }
    }
    return false;
}

bool verifyAdmin(const string& username, const string& password) {
    ifstream file("admins.txt");
    string line;
    while (getline(file, line)) {
        vector<string> admin = split(line, ';');
        if (admin[0] == username && admin[1] == password) {
            return true;
        }
    }
    return false;
}

void saveUser(const string& username, const string& password) {
    ofstream file("users.txt", ios::app);
    file << username << ";" << password << endl;
}

void changePassword(const string& username, const string& newPassword) {
    ifstream file("users.txt");
    ofstream tempFile("temp.txt");
    string line;
    while (getline(file, line)) {
        vector<string> user = split(line, ';');
        if (user[0] == username) {
            tempFile << username << ";" << newPassword << endl;
        } else {
            tempFile << line << endl;
        }
    }
    file.close();
    tempFile.close();
    remove("users.txt");
    rename("temp.txt", "users.txt");
}

void Admin::loadBooks() {
    ifstream file("books.txt");
    string line;
    while (getline(file, line)) {
        vector<string> book = split(line, ';');
        books.push_back(Book(book[0], book[1], book[2]));
    }
}

void Admin::saveBooks() {
    ofstream file("books.txt");
    for (const auto& book : books) {
        file << book.toString() << endl;
    }
}

void Admin::viewBooks() {
    cout << "Book List:" << endl;
    for (const auto& book : books) {
        cout << "Title: " << book.title << ", Author: " << book.author << ", Status: " << book.status << endl;
    }
}

void Admin::addBook() {
    string title, author;
    cout << "Enter book title: ";
    cin.ignore();
    getline(cin, title);
    cout << "Enter book author: ";
    getline(cin, author);
    books.push_back(Book(title, author, "available"));
}

void Admin::acceptRequest() {
    ifstream file("requests.txt");
    ofstream tempFile("temp.txt");
    string line;
    cout << "Request List:" << endl;
    while (getline(file, line)) {
        vector<string> request = split(line, ';');
        cout << "User: " << request[0] << ", Book: " << request[1] << ", Status: " << request[2] << endl;
    }
    file.close();
    
    string user, bookTitle;
    cout << "Enter user name to approve request: ";
    cin >> user;
    cout << "Enter book title to approve: ";
    cin.ignore();
    getline(cin, bookTitle);

    file.open("requests.txt");
    while (getline(file, line)) {
        vector<string> request = split(line, ';');
        if (request[0] == user && request[1] == bookTitle && request[2] == "requested") {
            tempFile << user << ";" << bookTitle << ";approved" << endl;
        } else {
            tempFile << line << endl;
        }
    }
    file.close();
    tempFile.close();
    remove("requests.txt");
    rename("temp.txt", "requests.txt");

    for (auto& book : books) {
        if (book.title == bookTitle) {
            book.status = "borrowed";
            break;
        }
    }
}

void Admin::deleteUser() {
    string username;
    cout << "Enter username to delete: ";
    cin >> username;

    ifstream file("users.txt");
    ofstream tempFile("temp.txt");
    string line;
    while (getline(file, line)) {
        vector<string> user = split(line, ';');
        if (user[0] != username) {
            tempFile << line << endl;
        }
    }
    file.close();
    tempFile.close();
    remove("users.txt");
    rename("temp.txt", "users.txt");
}

void User::loadBooks() {
    ifstream file("books.txt");
    string line;
    while (getline(file, line)) {
        vector<string> book = split(line, ';');
        books.push_back(Book(book[0], book[1], book[2]));
    }
}

void User::viewBooks() {
    cout << "Book List:" << endl;
    for (const auto& book : books) {
        cout << "Title: " << book.title << ", Author: " << book.author << ", Status: " << book.status << endl;
    }
}

void User::borrowBook() {
    string bookTitle;
    cout << "Enter book title to borrow: ";
    cin.ignore();
    getline(cin, bookTitle);

    for (auto& book : books) {
        if (book.title == bookTitle && book.status == "available") {
            book.status = "requested";
            saveRequest(bookTitle);
            cout << "Request sent." << endl;
            return;
        }
    }
    cout << "Book not available." << endl;
}

void User::saveRequest(const string& bookTitle) {
    ofstream file("requests.txt", ios::app);
    file << username << ";" << bookTitle << ";requested" << endl;
}

void User::viewRequests() {
    ifstream file("requests.txt");
    string line;
    cout << "Request List:" << endl;
    while (getline(file, line)) {
        vector<string> request = split(line, ';');
        if (request[0] == username) {
            cout << "Book: " << request[1] << ", Status: " << request[2] << endl;
        }
    }
}

void User::changePassword() {
    string newPassword;
    cout << "Enter new password: ";
    cin >> newPassword;
    ::changePassword(username, newPassword);
}

int main() {
    mainMenu();
    return 0;
}

void mainMenu() {
    int choice;
    do {
        cout << "Main Menu\n";
        cout << "1. User login\n";
        cout << "2. Admin login\n";
        cout << "3. Register\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            userLogin();
            break;
        case 2:
            adminLogin();
            break;
        case 3:
            registerUser();
            break;
        case 4:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice, try again.\n";
        }
    } while (choice != 4);
}

void userLogin() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (verifyUser(username, password)) {
        userMainMenu(username);
    } else {
        cout << "Invalid credentials.\n";
    }
}

void adminLogin() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (verifyAdmin(username, password)) {
        adminMainMenu();
    } else {
        cout << "Invalid credentials.\n";
    }
}

void registerUser() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (userExists(username)) {
        cout << "User already exists.\n";
    } else {
        saveUser(username, password);
        cout << "User registered successfully.\n";
    }
}

void userMainMenu(const string& username) {
    User user(username);
    int choice;
    do {
        cout << "User Main Menu\n";
        cout << "1. Book list\n";
        cout << "2. Borrow book\n";
        cout << "3. Request sent\n";
        cout << "4. Change password\n";
        cout << "5. Log out\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            user.viewBooks();
            break;
        case 2:
            user.borrowBook();
            break;
        case 3:
            user.viewRequests();
            break;
        case 4:
            user.changePassword();
            break;
        case 5:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice, try again.\n";
        }
    } while (choice != 5);
}

void adminMainMenu() {
    Admin admin;
    int choice;
    do {
        cout << "Admin Main Menu\n";
        cout << "1. Book list\n";
        cout << "2. Add book\n";
        cout << "3. Accept request\n";
        cout << "4. Delete user\n";
        cout << "5. Change password\n";
        cout << "6. Log out\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            admin.viewBooks();
            break;
        case 2:
            admin.addBook();
            break;
        case 3:
            admin.acceptRequest();
            break;
        case 4:
            admin.deleteUser();
            break;
        case 5:
            string username, newPassword;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter new password: ";
            cin >> newPassword;
            ::changePassword(username, newPassword);
            break;
        case 6:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice, try again.\n";
        }
    } while (choice != 6);
}
