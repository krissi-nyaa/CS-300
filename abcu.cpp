//============================================================================
// Name        : abcu.cpp
// Author      : Krissi Yan
// Version     : -0.05a
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Super Awesome Final Project!!
//============================================================================

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>

#define DEBUG false

struct Course {
        std::string title;
        std::string courseId;
        std::vector<std::string> prereqs;
};

class CourseVector {
    public:
        CourseVector();
        ~CourseVector() {};

    public:
        Course findCourse(std::string courseId);
        void removeCourse(std::string courseId);
        void addCourse(Course course);
        void addCourse(std::string title, std::string courseId, std::vector<std::string> prereqs);
        void printCourses();
        static void printCourse(Course c);
        void sortCourses();    
    private:
        std::vector<Course*> courses;
        bool isSorted;

};

CourseVector::CourseVector() {
    // Clear courses ... cuz why not?
    courses.clear();

    // Set sorted to false
    isSorted = false;
}

void CourseVector::printCourse(Course c) {
    // Don't print if empty
    if( c.courseId == "" || c.courseId == "-1")
        return;

    std::cout << "Course ID: " << c.courseId << " | Course Title: " << c.title << " | Prereqs: ";
    if(c.prereqs.size() < 1)
        std::cout << "None";
    else {
        for(int idy = 0; idy < c.prereqs.size(); idy++) {
            std::cout << c.prereqs.at(idy);
            if(idy < c.prereqs.size() - 1)
                std::cout << ", ";
        }
    }
}

// Not really sure what I've made here :)
// It's close to a BST ordered search, but runs using a vector, window, and with skew to adjust for odd/even sets
// (thus runtime of O(log_2(n) + 1))
Course CourseVector::findCourse(std::string courseId) {
    // Don't search if there's nothing loaded
    if(courses.size() < 1) {
        std::cout << "No courses loaded!";
        return Course();
    }

    if(!isSorted)
        sortCourses();

    bool hasSkewed = false;
    int window = std::ceil(courses.size() / 4.0);
    int idx = std::ceil((courses.size() - 1.0) / 2.0);
    Course t_course = *courses.at(idx);

    if(DEBUG)
        std::cout << "Size is " << courses.size() << std::endl;
        
    while(t_course.courseId != courseId && idx != 0 && idx != courses.size() - 1 && window != 0) {
        if(DEBUG)
            std::cout << "Searching at: " << idx << " window is " << window << std::endl;
        
        // Similar to a BST search, move left or right based on comparison
        if(t_course.courseId < courseId)
            idx += window;
        else
            idx -= window;

        // Set the next course for evaluation
        t_course = *courses.at(idx);
        
        // Window is halved
        std::ceil(window /= 2);

        // Account for skew left or right only once ( cheaper than Roof()+If statment )
        if(window < 1 && !hasSkewed) {
            if(DEBUG)
                std::cout << "Allowing Skew\n";
            window += 1;
            hasSkewed = true;
        }
    }

    // If we have found the course, return it
    if(t_course.courseId == courseId) {
        if(DEBUG)
            std::cout << "Found at " << idx << std::endl;
        return t_course;
    }
    // Otherwise return a blank course with errors in it
    else {
        std::cout << "Course not found" << std::endl;
        t_course.courseId = "-1";
        t_course.title = "Not found";
        return t_course;
    }

}

// Insert a course to the vector
void CourseVector::addCourse(std::string title, std::string courseId, std::vector<std::string> prereqs) {
    Course* t_course = new Course();
    t_course->courseId = courseId;
    t_course->title = title;
    t_course->prereqs = prereqs;

    courses.push_back(t_course);
    isSorted = false;
}

// Selection-sort the courses in alphanumeric order
void CourseVector::sortCourses() {
    // Don't sort if there's nothing loaded
    if(courses.size() < 1)
        return;

    // Iterate through all courses
    for(int idx = 0; idx < courses.size() - 1; idx++) {
        // Set the minimum to the first item
        int minIdx = idx;
        for(int idy = idx+1; idy < courses.size(); idy++) {
            // If we find a smaller item, set that to the smallest
            if(courses.at(idy)->courseId < courses.at(minIdx)->courseId)
                minIdx = idy;
        }
        // Swap that item in if something smaller was found
        if(minIdx != idx)
            std::swap(courses.at(minIdx),courses.at(idx));
    }

    isSorted = true;
}

void CourseVector::printCourses() {
    // Don't print if there's nothing loaded
    if(courses.size() < 1) {
        std::cout << "No courses loaded!";
        return;
    }

    // If the courses haven't been sorted or have been mushed with, sort them
    if(!isSorted)
        sortCourses();
    for(int idx = 0; idx < courses.size(); idx++) {
        std::cout << "Course ID: " << courses.at(idx)->courseId << " | Course Title: " << courses.at(idx)->title << " | Prereqs: ";
        if(courses.at(idx)->prereqs.size() < 1)
            std::cout << "None";
        else {
            for(int idy = 0; idy < courses.at(idx)->prereqs.size(); idy++) {
                std::cout << courses.at(idx)->prereqs.at(idy);
                if(idy < courses.at(idx)->prereqs.size() - 1)
                    std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
}

void parseInput(CourseVector* cv, std::string filename) {

    // Open the file
    std::ifstream inputFile(filename);

    // Check if the file exists
    if (inputFile.is_open()) {
        char c = 0x00;
        std::vector<std::string> values;
        std::string value = "";
        std::vector<std::string> t_vec;

        // Super awesome and creative (slow) way to compress parsing down to a few lines :D
        // Read the file char by char
        while (inputFile.get(c)) {
            switch(c) {
                case ',':
                    // If there's a value, store it, otherwise it was empty
                    if(value != "")
                        values.push_back(value);
                    value = "";
                    break;
                case '\r':
                    // Protection against empty lines
                    if(values.size() < 2) {
                        break;
                    }
                    // If there's a value, store it, otherwise it was empty
                    if(value != "")
                        values.push_back(value);
                    value = "";
                    t_vec.clear();
                    // If there's prereqs, parse them into a vector
                    if( values.size() > 2 ) {
                        for(int idx = 2; idx < values.size(); idx++)
                            t_vec.push_back(values.at(idx));
                    }
                    // Add the course
                    cv->addCourse(values.at(1), values.at(0), t_vec);
                    values.clear();
                    break;
                case '\n':
                    // only use \r as our signal (the encoding seen in the example file from the announcement)
                    break;
                default:
                    value += c;
            }
        }

        std::cout << "\nInput file loaded!\n";
    } else {
        std::cerr << "Error: File does not exist." << std::endl;
    }
}

void printMenu() {
    std::cout << "\n\t1 - Load input file\n";
    std::cout << "\t2 - Print loaded data\n";
    std::cout << "\t3 - Search for a course\n";
    std::cout << "\t9 - Exit program\n";
    std::cout << "Choice: ";
}

int main() {
    // File path and name
    CourseVector* cv = new CourseVector();

    std::cout << "Welcome to the ABCU Course Infomaticorium System v0.02a!" << std::endl;

    char choice = 0x00;
    std::string t_var = "";
    while( choice != '9' ) {
        printMenu();
        std::cin >> choice;
        switch(choice) {
            case '1':
                std::cout << "\nInput filename: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin >> t_var;
                parseInput(cv, t_var);
                break;
            case '2':
                cv->printCourses();
                break;
            case '3':
                std::cout << "\nCourse ID: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin >> t_var;
                std::cout << std::endl;
                CourseVector::printCourse(cv->findCourse(t_var));
        }
    }
        
    return 0;
}