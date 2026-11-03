/*
 * ------------------------------------------------------------
 *  Project   : Personal Weight Tracker
 *  Student   : RUDHR AHUJA
 *  Roll No.  : 25BCA20005
 *  Language  : C++ (C++11 or later)
 *
 *  Description:
 *  A console application that lets a user record their body
 *  weight over time, view history, edit/delete entries, see
 *  statistics, check BMI, set a goal and view a text-based
 *  progress chart. All data is saved to files so it is kept
 *  between runs (profile.txt and weights.csv).
 *
 *  Compile : g++ -std=c++11 -Wall -o WeightTracker PersonalWeightTracker.cpp
 *  Run     : ./WeightTracker      (Windows: WeightTracker.exe)
 * ------------------------------------------------------------
 */

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// ------------------------------------------------------------
//  Constants
// ------------------------------------------------------------
const string PROFILE_FILE = "profile.txt";
const string DATA_FILE    = "weights.csv";

const double MIN_WEIGHT = 10.0;   // kg
const double MAX_WEIGHT = 500.0;  // kg
const double MIN_HEIGHT = 50.0;   // cm
const double MAX_HEIGHT = 272.0;  // cm

// ------------------------------------------------------------
//  Data structures
// ------------------------------------------------------------
struct Entry {
    string date;     // YYYY-MM-DD
    double weight;   // kg
    string note;     // optional note (no commas / newlines)
};

struct Profile {
    string name;
    double heightCm;
    double goalWeight;
    bool   isSet;
    Profile() : name(""), heightCm(0), goalWeight(0), isSet(false) {}
};

// ------------------------------------------------------------
//  Utility helpers
// ------------------------------------------------------------
string trim(const string &s) {
    size_t start = 0, end = s.size();
    while (start < end && isspace(static_cast<unsigned char>(s[start]))) start++;
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(start, end - start);
}

string todayDate() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);
    return string(buf);
}

bool isLeapYear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

// Validates a date string of the form YYYY-MM-DD
bool isValidDate(const string &d) {
    if (d.size() != 10 || d[4] != '-' || d[7] != '-') return false;
    for (size_t i = 0; i < d.size(); i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(static_cast<unsigned char>(d[i]))) return false;
    }
    int y = atoi(d.substr(0, 4).c_str());
    int m = atoi(d.substr(5, 2).c_str());
    int day = atoi(d.substr(8, 2).c_str());
    if (y < 1900 || y > 2100 || m < 1 || m > 12 || day < 1) return false;
    int dim[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = dim[m - 1];
    if (m == 2 && isLeapYear(y)) maxDay = 29;
    return day <= maxDay;
}

void clearInputLine() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void pause() {
    cout << "\nPress Enter to continue...";
    string dummy;
    getline(cin, dummy);
}

// Reads a full line safely. Returns false if input stream has ended.
bool readLine(const string &prompt, string &out) {
    cout << prompt;
    if (!getline(cin, out)) return false;
    out = trim(out);
    return true;
}

// Reads a double within [lo, hi]; keeps asking until valid.
// Returns false only when input stream is closed.
bool readDouble(const string &prompt, double lo, double hi, double &value) {
    while (true) {
        string line;
        if (!readLine(prompt, line)) return false;
        if (line.empty()) {
            cout << "  Input cannot be empty.\n";
            continue;
        }
        char *endPtr = NULL;
        double v = strtod(line.c_str(), &endPtr);
        if (*endPtr != '\0') {
            cout << "  Please enter a valid number.\n";
            continue;
        }
        if (v < lo || v > hi) {
            cout << "  Value must be between " << lo << " and " << hi << ".\n";
            continue;
        }
        value = v;
        return true;
    }
}

// Reads an integer menu choice. Returns -1 on invalid / EOF handled by caller.
bool readInt(const string &prompt, int lo, int hi, int &value) {
    while (true) {
        string line;
        if (!readLine(prompt, line)) return false;
        if (line.empty()) continue;
        char *endPtr = NULL;
        long v = strtol(line.c_str(), &endPtr, 10);
        if (*endPtr != '\0') {
            cout << "  Please enter a whole number.\n";
            continue;
        }
        if (v < lo || v > hi) {
            cout << "  Choice must be between " << lo << " and " << hi << ".\n";
            continue;
        }
        value = static_cast<int>(v);
        return true;
    }
}

// Reads a valid date; empty input means "today".
bool readDate(const string &prompt, string &date) {
    while (true) {
        string line;
        if (!readLine(prompt, line)) return false;
        if (line.empty()) {
            date = todayDate();
            return true;
        }
        if (isValidDate(line)) {
            date = line;
            return true;
        }
        cout << "  Invalid date. Use format YYYY-MM-DD (e.g. 2026-10-30).\n";
    }
}

string fmt(double v, int prec = 1) {
    ostringstream oss;
    oss << fixed << setprecision(prec) << v;
    return oss.str();
}

// ------------------------------------------------------------
//  WeightTracker class
// ------------------------------------------------------------
class WeightTracker {
private:
    vector<Entry> entries;
    Profile profile;

    // ---- file handling ----
    void loadProfile() {
        ifstream in(PROFILE_FILE.c_str());
        if (!in) return;
        string name, h, g;
        if (getline(in, name) && getline(in, h) && getline(in, g)) {
            char *e1 = NULL, *e2 = NULL;
            double height = strtod(h.c_str(), &e1);
            double goal   = strtod(g.c_str(), &e2);
            if (*e1 == '\0' && *e2 == '\0' && !h.empty() && !g.empty() &&
                height >= MIN_HEIGHT && height <= MAX_HEIGHT &&
                goal >= MIN_WEIGHT && goal <= MAX_WEIGHT) {
                profile.name = name;
                profile.heightCm = height;
                profile.goalWeight = goal;
                profile.isSet = true;
            }
        }
    }

    void saveProfile() const {
        ofstream out(PROFILE_FILE.c_str());
        if (!out) {
            cout << "  Warning: could not save profile.\n";
            return;
        }
        out << profile.name << "\n" << profile.heightCm << "\n"
            << profile.goalWeight << "\n";
    }

    void loadEntries() {
        ifstream in(DATA_FILE.c_str());
        if (!in) return;
        string line;
        while (getline(in, line)) {
            if (trim(line).empty()) continue;
            size_t c1 = line.find(',');
            if (c1 == string::npos) continue;
            size_t c2 = line.find(',', c1 + 1);

            string date = trim(line.substr(0, c1));
            string wStr = (c2 == string::npos) ? line.substr(c1 + 1)
                                               : line.substr(c1 + 1, c2 - c1 - 1);
            string note = (c2 == string::npos) ? "" : trim(line.substr(c2 + 1));

            char *endPtr = NULL;
            double w = strtod(wStr.c_str(), &endPtr);
            if (!isValidDate(date) || endPtr == wStr.c_str() ||
                w < MIN_WEIGHT || w > MAX_WEIGHT)
                continue;  // skip corrupt line

            Entry e;
            e.date = date;
            e.weight = w;
            e.note = note;
            entries.push_back(e);
        }
        sortEntries();
    }

    void saveEntries() const {
        ofstream out(DATA_FILE.c_str());
        if (!out) {
            cout << "  Warning: could not save data to " << DATA_FILE << ".\n";
            return;
        }
        for (size_t i = 0; i < entries.size(); i++)
            out << entries[i].date << "," << entries[i].weight << ","
                << entries[i].note << "\n";
    }

    static bool byDate(const Entry &a, const Entry &b) { return a.date < b.date; }

    void sortEntries() { sort(entries.begin(), entries.end(), byDate); }

    int findByDate(const string &date) const {
        for (size_t i = 0; i < entries.size(); i++)
            if (entries[i].date == date) return static_cast<int>(i);
        return -1;
    }

    // Removes commas / newlines so the CSV format never breaks.
    static string cleanNote(string s) {
        for (size_t i = 0; i < s.size(); i++)
            if (s[i] == ',' || s[i] == '\n' || s[i] == '\r') s[i] = ' ';
        return s;
    }

    static string bmiCategory(double bmi) {
        if (bmi < 18.5) return "Underweight";
        if (bmi < 25.0) return "Normal weight";
        if (bmi < 30.0) return "Overweight";
        return "Obese";
    }

    double bmiFor(double weightKg) const {
        double m = profile.heightCm / 100.0;
        return weightKg / (m * m);
    }

    bool requireEntries() const {
        if (entries.empty()) {
            cout << "\n  No entries yet. Add one from the main menu.\n";
            return false;
        }
        return true;
    }

public:
    WeightTracker() {
        loadProfile();
        loadEntries();
    }

    bool hasProfile() const { return profile.isSet; }
    const string &userName() const { return profile.name; }

    // ---- 1. Set up / update profile ----
    void setupProfile() {
        cout << "\n=== Profile & Goal Setup ===\n";
        string name;
        if (!readLine("Your name" +
                      string(profile.isSet ? " [" + profile.name + "]" : "") + ": ", name))
            return;
        if (name.empty()) name = profile.isSet ? profile.name : "User";

        double h, g;
        if (!readDouble("Height in cm (" + fmt(MIN_HEIGHT, 0) + "-" + fmt(MAX_HEIGHT, 0) + "): ",
                        MIN_HEIGHT, MAX_HEIGHT, h)) return;
        if (!readDouble("Goal weight in kg: ", MIN_WEIGHT, MAX_WEIGHT, g)) return;

        profile.name = name;
        profile.heightCm = h;
        profile.goalWeight = g;
        profile.isSet = true;
        saveProfile();
        cout << "\n  Profile saved successfully.\n";
    }

    // ---- 2. Add entry ----
    void addEntry() {
        cout << "\n=== Add Weight Entry ===\n";
        string date;
        if (!readDate("Date (YYYY-MM-DD, Enter = today): ", date)) return;

        int idx = findByDate(date);
        if (idx != -1) {
            cout << "  An entry for " << date << " already exists ("
                 << fmt(entries[idx].weight) << " kg).\n";
            string ans;
            if (!readLine("  Overwrite it? (y/n): ", ans)) return;
            if (ans.empty() || (tolower(ans[0]) != 'y')) {
                cout << "  Cancelled.\n";
                return;
            }
        }

        double w;
        if (!readDouble("Weight in kg: ", MIN_WEIGHT, MAX_WEIGHT, w)) return;
        string note;
        if (!readLine("Note (optional): ", note)) return;

        Entry e;
        e.date = date;
        e.weight = w;
        e.note = cleanNote(note);

        if (idx != -1) entries[idx] = e;
        else entries.push_back(e);

        sortEntries();
        saveEntries();
        cout << "\n  Entry saved.\n";
    }

    // ---- 3. View all entries ----
    void viewEntries() const {
        cout << "\n=== Weight History ===\n";
        if (!requireEntries()) return;

        cout << left << setw(5) << "No." << setw(13) << "Date"
             << setw(12) << "Weight(kg)" << setw(10) << "Change"
             << "Note\n";
        cout << string(60, '-') << "\n";

        for (size_t i = 0; i < entries.size(); i++) {
            string change = "-";
            if (i > 0) {
                double d = entries[i].weight - entries[i - 1].weight;
                change = (d > 0 ? "+" : "") + fmt(d);
            }
            cout << left << setw(5) << (i + 1) << setw(13) << entries[i].date
                 << setw(12) << fmt(entries[i].weight) << setw(10) << change
                 << entries[i].note << "\n";
        }
    }

    // ---- 4. Edit entry ----
    void editEntry() {
        cout << "\n=== Edit Entry ===\n";
        if (!requireEntries()) return;
        viewEntries();

        int n;
        if (!readInt("\nEntry number to edit (0 = cancel): ", 0,
                     static_cast<int>(entries.size()), n)) return;
        if (n == 0) return;

        Entry &e = entries[n - 1];
        double w;
        if (!readDouble("New weight in kg (current " + fmt(e.weight) + "): ",
                        MIN_WEIGHT, MAX_WEIGHT, w)) return;
        string note;
        if (!readLine("New note (Enter = keep current): ", note)) return;

        e.weight = w;
        if (!note.empty()) e.note = cleanNote(note);
        saveEntries();
        cout << "\n  Entry updated.\n";
    }

    // ---- 5. Delete entry ----
    void deleteEntry() {
        cout << "\n=== Delete Entry ===\n";
        if (!requireEntries()) return;
        viewEntries();

        int n;
        if (!readInt("\nEntry number to delete (0 = cancel): ", 0,
                     static_cast<int>(entries.size()), n)) return;
        if (n == 0) return;

        string ans;
        if (!readLine("Delete entry for " + entries[n - 1].date + "? (y/n): ", ans)) return;
        if (ans.empty() || tolower(ans[0]) != 'y') {
            cout << "  Cancelled.\n";
            return;
        }
        entries.erase(entries.begin() + (n - 1));
        saveEntries();
        cout << "\n  Entry deleted.\n";
    }

    // ---- 6. Statistics ----
    void showStatistics() const {
        cout << "\n=== Statistics ===\n";
        if (!requireEntries()) return;

        double sum = 0;
        double mn = entries[0].weight, mx = entries[0].weight;
        for (size_t i = 0; i < entries.size(); i++) {
            sum += entries[i].weight;
            mn = min(mn, entries[i].weight);
            mx = max(mx, entries[i].weight);
        }
        double avg = sum / entries.size();
        double start = entries.front().weight;
        double current = entries.back().weight;
        double total = current - start;

        cout << "Total entries   : " << entries.size() << "\n";
        cout << "First entry     : " << entries.front().date << "  ("
             << fmt(start) << " kg)\n";
        cout << "Latest entry    : " << entries.back().date << "  ("
             << fmt(current) << " kg)\n";
        cout << "Lowest weight   : " << fmt(mn) << " kg\n";
        cout << "Highest weight  : " << fmt(mx) << " kg\n";
        cout << "Average weight  : " << fmt(avg) << " kg\n";
        cout << "Total change    : " << (total > 0 ? "+" : "") << fmt(total) << " kg";
        if (total < 0) cout << "  (lost)";
        else if (total > 0) cout << "  (gained)";
        cout << "\n";

        if (entries.size() >= 2) {
            double last = entries.back().weight - entries[entries.size() - 2].weight;
            cout << "Since last entry: " << (last > 0 ? "+" : "") << fmt(last) << " kg\n";
        }

        if (profile.isSet) {
            double diff = current - profile.goalWeight;
            cout << "Goal weight     : " << fmt(profile.goalWeight) << " kg\n";
            if (diff > 0.05)       cout << "To reach goal   : lose " << fmt(diff) << " kg\n";
            else if (diff < -0.05) cout << "To reach goal   : gain " << fmt(-diff) << " kg\n";
            else                   cout << "Goal status     : Goal reached! Congratulations!\n";
        } else {
            cout << "(Set a goal in the Profile option to see goal progress.)\n";
        }
    }

    // ---- 7. BMI ----
    void showBMI() const {
        cout << "\n=== BMI Calculator ===\n";
        if (!profile.isSet) {
            cout << "  Please set up your profile (height) first.\n";
            return;
        }
        if (!requireEntries()) return;

        double w = entries.back().weight;
        double bmi = bmiFor(w);
        cout << "Height          : " << fmt(profile.heightCm) << " cm\n";
        cout << "Latest weight   : " << fmt(w) << " kg (" << entries.back().date << ")\n";
        cout << "BMI             : " << fmt(bmi) << "  -> " << bmiCategory(bmi) << "\n";

        double m = profile.heightCm / 100.0;
        cout << "Healthy range   : " << fmt(18.5 * m * m) << " kg - "
             << fmt(24.9 * m * m) << " kg for your height\n";
    }

    // ---- 8. Progress chart ----
    void showChart() const {
        cout << "\n=== Weight Chart (last 15 entries) ===\n";
        if (!requireEntries()) return;

        size_t startIdx = entries.size() > 15 ? entries.size() - 15 : 0;
        double mn = entries[startIdx].weight, mx = mn;
        for (size_t i = startIdx; i < entries.size(); i++) {
            mn = min(mn, entries[i].weight);
            mx = max(mx, entries[i].weight);
        }
        double range = mx - mn;
        const int WIDTH = 40;

        for (size_t i = startIdx; i < entries.size(); i++) {
            int bars = (range < 0.0001)
                           ? WIDTH / 2
                           : 1 + static_cast<int>((entries[i].weight - mn) / range * (WIDTH - 1));
            cout << entries[i].date << " | " << string(bars, '#') << " "
                 << fmt(entries[i].weight) << " kg\n";
        }
        cout << "\n(Bars are scaled between " << fmt(mn) << " kg and " << fmt(mx) << " kg)\n";
    }

    // ---- 9. Export report ----
    void exportReport() const {
        cout << "\n=== Export Report ===\n";
        if (!requireEntries()) return;

        const string fname = "weight_report.txt";
        ofstream out(fname.c_str());
        if (!out) {
            cout << "  Could not create " << fname << ".\n";
            return;
        }
        out << "PERSONAL WEIGHT TRACKER - REPORT\n";
        out << "Generated on: " << todayDate() << "\n";
        if (profile.isSet)
            out << "Name: " << profile.name << "   Height: " << fmt(profile.heightCm)
                << " cm   Goal: " << fmt(profile.goalWeight) << " kg\n";
        out << string(50, '=') << "\n";
        out << left << setw(13) << "Date" << setw(12) << "Weight(kg)" << "Note\n";
        out << string(50, '-') << "\n";
        for (size_t i = 0; i < entries.size(); i++)
            out << left << setw(13) << entries[i].date << setw(12)
                << fmt(entries[i].weight) << entries[i].note << "\n";
        out << string(50, '=') << "\n";
        out << "Total entries: " << entries.size() << "\n";
        out << "Start: " << fmt(entries.front().weight) << " kg   Latest: "
            << fmt(entries.back().weight) << " kg   Change: "
            << fmt(entries.back().weight - entries.front().weight) << " kg\n";
        cout << "  Report saved to " << fname << "\n";
    }
};

// ------------------------------------------------------------
//  Menu
// ------------------------------------------------------------
void showMenu(const WeightTracker &app) {
    cout << "\n==========================================\n";
    cout << "        PERSONAL WEIGHT TRACKER\n";
    if (app.hasProfile()) cout << "        Welcome, " << app.userName() << "!\n";
    cout << "==========================================\n";
    cout << " 1. Set up / update profile & goal\n";
    cout << " 2. Add weight entry\n";
    cout << " 3. View all entries\n";
    cout << " 4. Edit an entry\n";
    cout << " 5. Delete an entry\n";
    cout << " 6. View statistics\n";
    cout << " 7. Check BMI\n";
    cout << " 8. Show weight chart\n";
    cout << " 9. Export report to file\n";
    cout << " 0. Exit\n";
    cout << "------------------------------------------\n";
}

int main() {
    WeightTracker app;

    if (!app.hasProfile()) {
        cout << "Welcome! Let's set up your profile first.\n";
        app.setupProfile();
    }

    while (true) {
        showMenu(app);
        int choice;
        if (!readInt("Enter your choice: ", 0, 9, choice)) break;  // EOF

        switch (choice) {
            case 1: app.setupProfile(); break;
            case 2: app.addEntry();     break;
            case 3: app.viewEntries();  break;
            case 4: app.editEntry();    break;
            case 5: app.deleteEntry();  break;
            case 6: app.showStatistics(); break;
            case 7: app.showBMI();      break;
            case 8: app.showChart();    break;
            case 9: app.exportReport(); break;
            case 0:
                cout << "\nThank you for using Personal Weight Tracker. Stay healthy!\n";
                return 0;
        }
        if (cin) pause();
        else break;
    }
    return 0;
}
