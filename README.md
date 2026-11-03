# 🏋️ Personal Weight Tracker

A console-based **Personal Weight Tracker** developed using C++. This application allows users to record and monitor their body weight over time, manage personal goals, calculate BMI, view statistics, and track progress through a text-based chart.

The application uses file handling to store user profiles and weight records permanently, ensuring that data remains available even after the program is closed.

---

## 👨‍💻 Project Information

| Detail | Information |
|---|---|
| Project Name | Personal Weight Tracker |
| Student Name | RUDHR AHUJA |
| Roll Number | 25BCA20005 |
| Programming Language | C++ |
| Standard | C++11 or Later |
| Project Type | Console-Based Application |

---

## ✨ Features

The application provides the following functionalities:

### 1. Profile & Goal Management
- Create and update a personal profile.
- Enter your name and height.
- Set a target weight.
- Automatically save profile information.

### 2. Add Weight Entries
- Record daily body weight.
- Select a custom date or use the current date.
- Add optional notes to each entry.
- Prevent duplicate entries for the same date unless the user chooses to overwrite them.

### 3. View Weight History
- Display all previously recorded weight entries.
- View dates, recorded weights, and optional notes.
- Track weight changes between consecutive entries.

### 4. Edit Weight Entries
- Update previously recorded weight values.
- Modify existing notes.
- Save changes automatically.

### 5. Delete Weight Entries
- Remove unwanted weight records.
- Confirmation before deleting an entry.

### 6. Weight Statistics
View useful statistics, including:
- Total number of entries.
- First recorded weight.
- Latest recorded weight.
- Lowest recorded weight.
- Highest recorded weight.
- Average weight.
- Total weight change.
- Progress towards the target weight.

### 7. BMI Calculator
- Calculate Body Mass Index (BMI).
- Uses the user's saved height and latest recorded weight.
- Displays a BMI category.
- Shows a reference weight range based on height.

*Note: BMI is a general screening measure and is not a complete assessment of individual health.*

### 8. Progress Chart
- Display a text-based weight progress chart.
- Visualize the last 15 weight entries.
- Compare weight values using proportional bars.

### 9. Export Report
- Generate a text-based weight report.
- Include profile information and weight history.
- Save the report as `weight_report.txt`.

---

## 🛠️ Technologies Used

| Technology | Purpose |
|---|---|
| C++ | Core programming language |
| C++ Standard Library | Data processing and utilities |
| File Handling | Permanent data storage |
| Vectors | Storing weight records |
| Structures | Organizing profile and entry data |
| Classes & Objects | Implementing object-oriented programming |
| CSV Files | Storing weight history |

### C++ Concepts Implemented
- Object-Oriented Programming (OOP)
- Classes and Objects
- Structures
- Functions
- Conditional Statements
- Loops
- Vectors
- File Handling
- Input Validation
- Sorting Algorithms
- String Manipulation

---

## 📁 Project Structure

```text
Personal-Weight-Tracker/
│
├── PersonalWeightTracker.cpp
│
├── profile.txt
│
├── weights.csv
│
├── weight_report.txt
│
└── README.md
```

**File Description:**

| File | Description |
|---|---|
| `PersonalWeightTracker.cpp` | Main C++ source code |
| `profile.txt` | Stores user profile and goal information |
| `weights.csv` | Stores weight entries in CSV format |
| `weight_report.txt` | Generated report containing weight history |
| `README.md` | Project documentation |

*Note: The data and report files are automatically created when required by the application.*

---

## ⚙️ Installation & Setup

### Prerequisites

You need:
- A C++ compiler (GCC or MinGW recommended).
- A terminal or command prompt.
- Support for C++11 or later.

### Step 1: Clone the Repository

```bash
git clone https://github.com/your-username/Personal-Weight-Tracker.git
```

### Step 2: Navigate to the Project Directory

```bash
cd Personal-Weight-Tracker
```

### Step 3: Compile the Program

Using GCC:

```bash
g++ -std=c++11 -Wall -o WeightTracker PersonalWeightTracker.cpp
```

### Step 4: Run the Application

**Windows:**

```bash
WeightTracker.exe
```

**Linux / macOS:**

```bash
./WeightTracker
```

---

## 🖥️ Application Menu

After launching the application, users are presented with the following menu:

```text
==========================================
        PERSONAL WEIGHT TRACKER
==========================================
 1. Set up / update profile & goal
 2. Add weight entry
 3. View all entries
 4. Edit an entry
 5. Delete an entry
 6. View statistics
 7. Check BMI
 8. Show weight chart
 9. Export report to file
 0. Exit
------------------------------------------
Enter your choice:
```

Select the desired option by entering its corresponding number.

---

## 💾 Data Storage

The application uses local file handling to preserve information between program executions.

**1. profile.txt**

Stores:
- User name
- Height in centimetres
- Goal weight in kilograms

**2. weights.csv**

Stores weight records using the following format:

```csv
Date,Weight,Note
2026-10-30,85.5,Initial entry
2026-10-31,85.2,Morning measurement
2026-11-01,84.9,Regular tracking
```

The application automatically loads saved records when it starts and updates the files whenever changes are made.

---

## 🔒 Input Validation

The application includes several validation mechanisms:

- Validates weight values within the permitted range.
- Validates height values.
- Checks date format and calendar validity.
- Prevents invalid menu selections.
- Handles empty inputs.
- Prevents duplicate date entries without confirmation.
- Skips invalid or corrupted weight records while loading data.
- Cleans commas and newline characters from notes to maintain CSV formatting.

---

## 🚀 Future Improvements

Potential improvements for future versions include:

- Graphical User Interface (GUI).
- Database integration using MySQL or SQLite.
- Monthly and weekly progress reports.
- Graphical weight visualization.
- Password-protected user profiles.
- Multiple user account support.
- Cloud backup and synchronization.
- Mobile application integration.

---

## 🎯 Project Objective

The primary objective of this project is to develop a simple and user-friendly weight management system using C++.

It demonstrates the practical implementation of programming fundamentals, object-oriented programming, data structures, file handling, and input validation in a real-world application.

---

## 👨‍🎓 Developed By

**RUDHR AHUJA**

BCA – AR/VR Development  
Roll Number: 25BCA20005

**Project:** Personal Weight Tracker

**Language:** C++

---

⭐ If you find this project useful, feel free to explore and improve it!
