#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

const int MAX_COURSES = 100;
const int MAX_PREREQS = 10;
const int MAX_SEMESTERS = 10;
const int MAX_CODES_PER_SEMESTER = 20;

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
    char semesterCourses[MAX_SEMESTERS][MAX_CODES_PER_SEMESTER][10];
    int semesterCourseCounts[MAX_SEMESTERS];

public:
    CourseGraph() : courseCount(0) {
        for (int i = 0; i < MAX_SEMESTERS; ++i) {
            semesterCourseCounts[i] = 0;
        }
    }

    void addCourse(const char* code, const char* name, const char prereqCodes[][10], const char prereqNames[][100], int prereqCount, int semester) {
        strncpy(courseList[courseCount].code, code, sizeof(courseList[courseCount].code));
        strncpy(courseList[courseCount].name, name, sizeof(courseList[courseCount].name));
        courseList[courseCount].prereqCount = prereqCount;

        for (int i = 0; i < prereqCount; ++i) {
            strncpy(courseList[courseCount].prereqCodes[i], prereqCodes[i], sizeof(courseList[courseCount].prereqCodes[i]));
            strncpy(courseList[courseCount].prereqNames[i], prereqNames[i], sizeof(courseList[courseCount].prereqNames[i]));
        }

        strncpy(semesterCourses[semester - 1][semesterCourseCounts[semester - 1]], code, sizeof(semesterCourses[semester - 1][semesterCourseCounts[semester - 1]]));
        semesterCourseCounts[semester - 1]++;
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
            cout << string(level * 4, ' ') << "   ^f    Course not found: " << code << endl;
            return;
        }

        const Course& course = courseList[index];
        cout << string(level * 4, ' ') << "   ^f    " << course.code << " - " << course.name << endl;

        for (int i = 0; i < course.prereqCount; i++) {
            if (strcmp(course.prereqCodes[i], "NA") == 0) {
                cout << string((level + 1) * 4, ' ') << "   ^f    " << course.prereqNames[i] << endl;
                continue;
            }
            printPrerequisites(course.prereqCodes[i], level + 1);
        }
    }

    void loadCoursesFromFile(const char* filename, int semester) {
        ifstream file(filename);
        if (!file.is_open()) return;

        char line[1024];
        while (file.getline(line, sizeof(line))) {
            char code[10] = {}, name[100] = {};
            char prereqCodesPart[256] = {}, prereqNamesPart[512] = {};
            char parsedPrereqCodes[MAX_PREREQS][10];
            char parsedPrereqNames[MAX_PREREQS][100];
            int prereqCount = 0;

            int pos1 = -1, pos2 = -1, pos3 = -1;
            for (int i = 0, sepCount = 0; line[i] != '\0'; i++) {
                if (line[i] == '|') {
                    if (sepCount == 0) pos1 = i;
                    else if (sepCount == 1) pos2 = i;
                    else if (sepCount == 2) {
                        pos3 = i;
                        break;
                    }
                    sepCount++;
                }
            }
            if (pos1 == -1 || pos2 == -1 || pos3 == -1) continue;

            strncpy(code, line, min(pos1, (int)sizeof(code) - 1));
            strncpy(name, line + pos1 + 1, min(pos2 - pos1 - 1, (int)sizeof(name) - 1));
            strncpy(prereqCodesPart, line + pos2 + 1, min(pos3 - pos2 - 1, (int)sizeof(prereqCodesPart) - 1));
            strncpy(prereqNamesPart, line + pos3 + 1, min((int)strlen(line) - pos3 - 1, (int)sizeof(prereqNamesPart) - 1));

            prereqCount = 0;
            int start = 0, end = 0;
            while (prereqCodesPart[end] != '\0') {
                if (prereqCodesPart[end] == ',' || prereqCodesPart[end + 1] == '\0') {
                    int len = (prereqCodesPart[end] == ',' ? end : end + 1) - start;
                    if (len >= 10) len = 9;
                    strncpy(parsedPrereqCodes[prereqCount], prereqCodesPart + start, len);
                    parsedPrereqCodes[prereqCount][len] = '\0';
                    prereqCount++;
                    start = end + 1;
                    if (prereqCount >= MAX_PREREQS) break;
                }
                end++;
            }

            start = 0;
            end = 0;
            int count = 0;
            while (prereqNamesPart[end] != '\0') {
                if (prereqNamesPart[end] == ',' || prereqNamesPart[end + 1] == '\0') {
                    int len = (prereqNamesPart[end] == ',' ? end : end + 1) - start;
                    if (len >= 100) len = 99;
                    strncpy(parsedPrereqNames[count], prereqNamesPart + start, len);
                    parsedPrereqNames[count][len] = '\0';
                    count++;
                    start = end + 1;
                    if (count >= MAX_PREREQS) break;
                }
                end++;
            }

            addCourse(code, name, parsedPrereqCodes, parsedPrereqNames, prereqCount, semester);
        }

        file.close();
    }

    void displaySemesterCourses(int semester) {
        if (semester == 7 || semester == 10) {
            cout << "Semester " << semester << ": Project Work / Research\n";
            return;
        }

        cout << "Courses in Semester " << semester << ":\n";
        for (int i = 0; i < semesterCourseCounts[semester - 1]; ++i) {
            const char* code = semesterCourses[semester - 1][i];
            int index = findCourseIndex(code);
            if (index != -1) {
                cout << i + 1 << ". " << courseList[index].code << " - " << courseList[index].name << endl;
            }
        }
    }
};

int main() {
    CourseGraph graph;

    graph.loadCoursesFromFile("sem1.txt", 1);
    graph.loadCoursesFromFile("sem2.txt", 2);
    graph.loadCoursesFromFile("sem3.txt", 3);
    graph.loadCoursesFromFile("sem4.txt", 4);
    graph.loadCoursesFromFile("sem5.txt", 5);
    graph.loadCoursesFromFile("sem6.txt", 6);
    graph.loadCoursesFromFile("sem8.txt", 8);
    graph.loadCoursesFromFile("sem9.txt", 9);

    int semester;
    cout << "Enter semester number (1-10): ";
    cin >> semester;

    if (semester < 1 || semester > 10) {
        cout << "Invalid semester.\n";
        return 0;
    }

    graph.displaySemesterCourses(semester);

    if (semester == 7 || semester == 10) return 0;

    char code[10];
    cout << "Enter course code to print prerequisites: ";
    cin >> code;

    cout << "\nPrerequisite Graph:\n";
    graph.printPrerequisites(code);

    return 0;
}



