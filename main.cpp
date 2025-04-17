#include <iostream>
#include <fstream>
#include <sstream>
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
        strncpy(courseList[courseCount].code, code, sizeof(courseList[courseCount].code));
        strncpy(courseList[courseCount].name, name, sizeof(courseList[courseCount].name));
        courseList[courseCount].prereqCount = prereqCount;

        for (int i = 0; i < prereqCount; ++i) {
            strncpy(courseList[courseCount].prereqCodes[i], prereqCodes[i], sizeof(courseList[courseCount].prereqCodes[i]));
            strncpy(courseList[courseCount].prereqNames[i], prereqNames[i], sizeof(courseList[courseCount].prereqNames[i]));
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
                cout << string((level + 1) * 2, ' ') << "Prerequisite: " << course.prereqNames[i] << endl;
                continue;
            }
            cout << string((level + 1) * 2, ' ') << "Prerequisite: " << course.prereqCodes[i] << " - " << course.prereqNames[i] << endl;
            printPrerequisites(course.prereqCodes[i], level + 1);
        }
    }

    void loadCoursesFromFile(const char* filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            char code[10] = {}, name[100] = {};
            char prereqCodesPart[256] = {}, prereqNamesPart[512] = {};
            char parsedPrereqCodes[MAX_PREREQS][10];
            char parsedPrereqNames[MAX_PREREQS][100];
            int prereqCount = 0;

            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);
            size_t pos3 = line.find('|', pos2 + 1);

            if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) continue;

            int length1 = pos1;
            int length2 = pos2 - pos1 - 1;
            int length3 = pos3 - pos2 - 1;
            int length4 = line.length() - pos3 - 1;

            strncpy(code, &line[0], min(length1, (int)sizeof(code)-1));
            strncpy(name, &line[pos1 + 1], min(length2, (int)sizeof(name)-1));
            strncpy(prereqCodesPart, &line[pos2 + 1], min(length3, (int)sizeof(prereqCodesPart)-1));
            strncpy(prereqNamesPart, &line[pos3 + 1], min(length4, (int)sizeof(prereqNamesPart)-1));

            stringstream codeStream(prereqCodesPart);
            string token;
            prereqCount = 0;
            while (getline(codeStream, token, ',') && prereqCount < MAX_PREREQS) {
                strncpy(parsedPrereqCodes[prereqCount], token.c_str(), sizeof(parsedPrereqCodes[prereqCount]));
                prereqCount++;
            }

            stringstream nameStream(prereqNamesPart);
            for (int i = 0; i < prereqCount && getline(nameStream, token, ','); ++i) {
                strncpy(parsedPrereqNames[i], token.c_str(), sizeof(parsedPrereqNames[i]));
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
