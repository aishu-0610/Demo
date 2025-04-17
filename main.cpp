#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

const int MAX_COURSES = 100;
const int MAX_PREREQS = 10;

struct Course {
    char code[10];
    char name[100];
    char prereqCodes[MAX_PREREQS][10];
    char prereqNames[MAX_PREREQS][100];
    int prereqCount;
};

class CourseGraph {
private:
    Course courseList[MAX_COURSES];
    int courseCount;

public:
    CourseGraph() : courseCount(0) {}

    void addCourse(const char* code, const char* name, const char prereqCodes[][10], const char prereqNames[][100], int prereqCount) {
        strcpy(courseList[courseCount].code, code);
        strcpy(courseList[courseCount].name, name);
        courseList[courseCount].prereqCount = prereqCount;
        for (int i = 0; i < prereqCount; ++i) {
            strcpy(courseList[courseCount].prereqCodes[i], prereqCodes[i]);
            strcpy(courseList[courseCount].prereqNames[i], prereqNames[i]);
        }
        courseCount++;
    }

    int findCourseIndex(const char* code) {
        for (int i = 0; i < courseCount; ++i) {
            if (strcmp(courseList[i].code, code) == 0) {
                return i;
            }
        }
        return -1;
    }

    void printPrerequisites(const char* code, int level = 0) {
        if (strcmp(code, "NA") == 0) return;
        int index = findCourseIndex(code);
        if (index == -1) {
            cout << "Course not found: " << code << endl;
            return;
        }
        const Course& course = courseList[index];
        cout << string(level * 2, ' ') << "Course: " << course.code << " - " << course.name << endl;
        for (int i = 0; i < course.prereqCount; i++) {
            if (strcmp(course.prereqCodes[i], "NA") == 0) {
                cout << string(level * 2, ' ') << "Prerequisite: " << course.prereqNames[i] << endl;
                continue;
            }
            cout << string(level * 2, ' ') << "Prerequisite: " << course.prereqCodes[i] << " - " << course.prereqNames[i] << endl;
            printPrerequisites(course.prereqCodes[i], level + 1);
        }
    }

    void loadCoursesFromFile(const char* filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        char line[512];
        while (file.getline(line, sizeof(line))) {
            char code[10], name[100], prereqCodes[256], prereqNames[256];
            sscanf(line, "%[^|]|%[^|]|%[^|]|%[^
]", code, name, prereqCodes, prereqNames);
            char parsedPrereqCodes[MAX_PREREQS][10];
            char parsedPrereqNames[MAX_PREREQS][100];
            int prereqCount = 0;
            char* token = strtok(prereqCodes, ",");
            while (token != nullptr && prereqCount < MAX_PREREQS) {
                strcpy(parsedPrereqCodes[prereqCount], token);
                prereqCount++;
                token = strtok(nullptr, ",");
            }
            token = strtok(prereqNames, ",");
            for (int i = 0; i < prereqCount && token != nullptr; ++i) {
                strcpy(parsedPrereqNames[i], token);
                token = strtok(nullptr, ",");
            }
            addCourse(code, name, parsedPrereqCodes, parsedPrereqNames, prereqCount);
        }
        file.close();
    }
};

int main() {
    CourseGraph graph;
    graph.loadCoursesFromFile("sem1.txt");
    graph.loadCoursesFromFile("sem2.txt");
    graph.loadCoursesFromFile("sem3.txt");
    graph.loadCoursesFromFile("sem4.txt");
    graph.loadCoursesFromFile("sem5.txt");
    graph.loadCoursesFromFile("sem6.txt");
    graph.loadCoursesFromFile("sem8.txt");
    graph.loadCoursesFromFile("sem9.txt");
    char courseCode[10];
    cout << "Enter course code to print prerequisites graph: ";
    cin >> courseCode;
    cout << "Prerequisite Graph (Directed):" << endl;
    graph.printPrerequisites(courseCode);
    return 0;
}

