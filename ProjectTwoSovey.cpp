//============================================================================
// Name        : ProjectTwoSovey.cpp
// Author      : Gage Sovey
// Version     : 1.0
// Description : Project Two Done Utilizing Hash Table
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string>
#include <vector> 
#include <time.h>
#include <sstream> 
#include <fstream>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold bid information
struct Course {
    string courseId; // unique identifier
    string title;
    vector<string> preReq;
    Course() {
        title = "";
    }
};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold bids
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a bid
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a bid and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    void Remove(string courseId);
    Course Search(string courseId);
    size_t Size();
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // FIXME (1): Initialize the structures used to hold bids

    // Initalize node structure by resizing tableSize
    nodes.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    // resize nodes size
    tableSize = size;
    nodes.resize(tableSize);
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // FIXME (2): Implement logic to free storage when class is destroyed

    // erase nodes beginning
    nodes.erase(nodes.begin(), nodes.end());
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    // FIXME (3): Implement logic to calculate a hash value
    // return key tableSize
    return key % tableSize;
}

/**
 * Insert a bid
 *
 * @param bid The bid to insert
 */
void HashTable::Insert(Course course) {
    std::hash<std::string> hashFunction;
    unsigned key = hashFunction(course.courseId) % tableSize;  // Generate the hash key

    Node* node = &nodes.at(key);  // Get the node at the hashed key

    // Debugging output to verify insertion
    // cout << "Inserting course: " << course.courseId << " with key: " << key << endl;

    // If the node is not initialized (key == UINT_MAX), insert the course here
    if (node->key == UINT_MAX) {
        node->key = key;
        node->course = course;
        node->next = nullptr;
    }
    else {
        // Handle collisions: traverse the linked list and insert at the end
        while (node->next != nullptr) {
            node = node->next;
        }
        // Create a new node at the end of the list
        node->next = new Node(course, key);
    }
}


/**
 * Print all bids
 */
void HashTable::PrintAll() {
    vector<Course> courseList;  // Vector to store all courses

    // Collect all courses from the hash table
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        if (it->key != UINT_MAX) {
            // Add the course from the current node to the list
            courseList.push_back(it->course);

            // Traverse the linked list if there are collisions
            Node* node = it->next;
            while (node != nullptr) {
                courseList.push_back(node->course);
                node = node->next;
            }
        }
    }

    // Sort the courses alphanumerically by courseId
    std::sort(courseList.begin(), courseList.end(), [](const Course& a, const Course& b) {
        return a.courseId < b.courseId;
        });

    // Print the sorted list of courses
    for (const auto& course : courseList) {
        cout << "Course: " << course.courseId << ": " << course.title << endl;
    }
}



/**
 * Remove a bid
 *
 * @param bidId The bid id to search for
 */
void HashTable::Remove(string courseId) {
    unsigned courseIdKey = this->hash(std::atoi(courseId.c_str()));
    Node* index = &nodes.at(courseIdKey);

    // if there is only a node here, then erase it
    if (index->next == nullptr) {
        nodes.erase(nodes.begin() + courseIdKey);
        index->key = UINT_MAX;
        return;
    }


    // if index is the head node, we need to move the chain to the index node
    if (index->course.courseId == courseId) {
        Node* temp = index->next;
        nodes.at(courseIdKey) = *temp;
        delete temp;

        return;
    }

    // if there are multiple bids at this bucket, then remove the correct one
    Node* curr = index->next;

    // walk the list until we find the correct bid
    while (curr != nullptr) {
        if (courseId == curr->course.courseId) {

            Node* temp = curr;
            index->next = temp->next;

            delete temp;
            temp = nullptr;
            return;
        }
        else {
            index = index->next;
            curr = curr->next;
        }
    }

}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Course HashTable::Search(string courseId) {
    std::hash<std::string> hashFunction;
    unsigned key = hashFunction(courseId) % tableSize;  // Generate the hash key and ensure it is within bounds

    Node* node = &(nodes.at(key));  // Get the node corresponding to the hash key

    // Traverse the chain of nodes (in case of collisions)
    while (node != nullptr) {
        // Debugging: print the current node courseId and key
        // cout << "Checking node with courseId: " << node->course.courseId << " and key: " << node->key << endl;

        // Compare the courseId with the one in the node
        if (node->key != UINT_MAX && node->course.courseId == courseId) {
            return node->course;  // Course found
        }
        node = node->next;  // Move to the next node in the chain (if any)
    }

    // If we reach here, the course was not found
    return Course();  // Return an empty course object
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayCourse(Course course, HashTable* hashTable) {
    cout << course.courseId << ": " << course.title << endl;
    cout << "Pre - Requisites:" << endl;

    if (course.preReq.size() > 0) {
        for (int i = 0; i < course.preReq.size(); i++) {
            // Search the hash table for the prerequisite course
            Course preReqCourse = hashTable->Search(course.preReq[i]);

            // Print the prerequisite course's ID and title
            if (!preReqCourse.courseId.empty()) {
                cout << "Pre-Requisite #" << (i + 1) << " - " << preReqCourse.courseId
                    << ": " << preReqCourse.title << endl;
            }
            else {
                // If the prerequisite course is not found
                cout << "Pre-Requisite #" << (i + 1) << " - " << course.preReq[i] << " (not found)" << endl;
            }
        }
    }
    else {
        cout << "No pre-requisites needed for this course." << endl;
    }
}

// Function to load CSV data into a HashTable
void loadCSVData(const string& filename, HashTable* hashTable) {
    ifstream file(filename); // Open the file

    if (!file.is_open()) {
        cerr << "Error: Could not open the file!" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);  // Create a stringstream for the line
        string cell;

        Course course;  // Create a new Course object

        // Get the Course ID
        getline(ss, course.courseId, ',');

        // Get the Course Title
        getline(ss, course.title, ',');

        // Get the prerequisites (columns 3 and 4, if present)
        while (getline(ss, cell, ',')) {
            if (!cell.empty()) {
                course.preReq.push_back(cell);  // Add the prerequisite to the vector
            }
        }

        // Insert the course into the hash table
        hashTable->Insert(course);
    }

    file.close();  // Close the file
}


/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {


    string csvPath, courseKey;

    // Define a hash table to hold all the bids
    HashTable* courseTable;

    Course course;
    courseTable = new HashTable();

    int choice = 0;
    while (choice != 9) {
        cout << endl;
        cout << "Menu:" << endl;
        cout << "  1. Load Data Structure." << endl;
        cout << "  2. Print Course List." << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        // Clear the input buffer after reading an integer with cin
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {

        case 1:

            // Initialize a timer variable before loading bids


            // Complete the method call to load the bids
            cout << "What is the file name?" << endl;
            getline(cin, csvPath);
            cout << "Attempting to open file: " << csvPath << endl;  // Debugging print statement
            loadCSVData(csvPath, courseTable);

            break;

        case 2:
            courseTable->PrintAll();
            break;

        case 3:
            cout << "What course do you want to know about? ";
            cin >> std::ws;  // Ignore leading whitespace
            getline(cin, courseKey);
            course = courseTable->Search(courseKey);



            if (!course.courseId.empty()) {
                displayCourse(course, courseTable);
            }
            else {
                cout << "Course not found." << endl;
            }
            break;

        }

       
    }
    cout << "Good bye." << endl;

        return 0;
}



