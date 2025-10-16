#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;

struct Topic {
    string name;
    bool completed;
    string completedDate;
};

struct Subject {
    string name;
    vector<Topic> topics;
};

class JEESyllabusTracker {
private:
    vector<Subject> subjects;
    int currentSubject = 0;
    int currentTopic = 0;
    int scrollOffset = 0;
    const string dataFile = "jee_progress.dat";

    void initializeSubjects() {
        subjects = {
            {"Physics", {
                {"Mechanics - Kinematics", false, ""},
                {"Mechanics - Laws of Motion", false, ""},
                {"Mechanics - Work, Energy & Power", false, ""},
                {"Mechanics - Rotational Motion", false, ""},
                {"Gravitation", false, ""},
                {"Properties of Matter", false, ""},
                {"Thermodynamics", false, ""},
                {"Kinetic Theory of Gases", false, ""},
                {"Oscillations", false, ""},
                {"Waves", false, ""},
                {"Electrostatics", false, ""},
                {"Current Electricity", false, ""},
                {"Magnetic Effects of Current", false, ""},
                {"Electromagnetic Induction", false, ""},
                {"Alternating Current", false, ""},
                {"Optics - Ray Optics", false, ""},
                {"Optics - Wave Optics", false, ""},
                {"Modern Physics - Dual Nature", false, ""},
                {"Modern Physics - Atoms & Nuclei", false, ""},
                {"Electronic Devices", false, ""}
            }},
            {"Chemistry", {
                {"Atomic Structure", false, ""},
                {"Chemical Bonding", false, ""},
                {"States of Matter", false, ""},
                {"Thermodynamics", false, ""},
                {"Chemical Equilibrium", false, ""},
                {"Ionic Equilibrium", false, ""},
                {"Redox Reactions", false, ""},
                {"Electrochemistry", false, ""},
                {"Chemical Kinetics", false, ""},
                {"Surface Chemistry", false, ""},
                {"Periodic Table", false, ""},
                {"Hydrogen & s-Block", false, ""},
                {"p-Block Elements", false, ""},
                {"d & f Block Elements", false, ""},
                {"Coordination Compounds", false, ""},
                {"Organic Chemistry Basics", false, ""},
                {"Hydrocarbons", false, ""},
                {"Organic Compounds - Haloalkanes", false, ""},
                {"Alcohols, Phenols & Ethers", false, ""},
                {"Aldehydes & Ketones", false, ""},
                {"Carboxylic Acids", false, ""},
                {"Amines", false, ""},
                {"Biomolecules", false, ""},
                {"Polymers", false, ""}
            }},
            {"Mathematics", {
                {"Sets & Relations", false, ""},
                {"Complex Numbers", false, ""},
                {"Quadratic Equations", false, ""},
                {"Sequences & Series", false, ""},
                {"Permutations & Combinations", false, ""},
                {"Binomial Theorem", false, ""},
                {"Matrices & Determinants", false, ""},
                {"Probability", false, ""},
                {"Functions", false, ""},
                {"Limits & Continuity", false, ""},
                {"Differentiation", false, ""},
                {"Applications of Derivatives", false, ""},
                {"Indefinite Integration", false, ""},
                {"Definite Integration", false, ""},
                {"Differential Equations", false, ""},
                {"Coordinate Geometry - Straight Lines", false, ""},
                {"Coordinate Geometry - Circles", false, ""},
                {"Coordinate Geometry - Parabola", false, ""},
                {"Coordinate Geometry - Ellipse", false, ""},
                {"Coordinate Geometry - Hyperbola", false, ""},
                {"3D Geometry", false, ""},
                {"Vectors", false, ""},
                {"Trigonometry", false, ""},
                {"Statistics", false, ""}
            }}
        };
    }

    string getCurrentDate() {
        time_t now = time(0);
        char buf[80];
        strftime(buf, sizeof(buf), "%d-%m-%Y", localtime(&now));
        return string(buf);
    }

    void saveProgress() {
        ofstream file(dataFile, ios::binary);
        if (!file) return;
        
        for (auto& subject : subjects) {
            for (auto& topic : subject.topics) {
                file << topic.completed << " ";
                file << topic.completedDate << "\n";
            }
        }
        file.close();
    }

    void loadProgress() {
        ifstream file(dataFile, ios::binary);
        if (!file) return;
        
        for (auto& subject : subjects) {
            for (auto& topic : subject.topics) {
                int comp;
                file >> comp;
                topic.completed = comp;
                file.ignore();
                getline(file, topic.completedDate);
            }
        }
        file.close();
    }

    void drawHeader() {
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(0, 0, "+==============================================================================+");
        mvprintw(1, 0, "|                            veni vidi vici                                    |");
        mvprintw(2, 0, "+==============================================================================+");
        attroff(COLOR_PAIR(1) | A_BOLD);
    }

    void drawStats() {
        int total = 0, completed = 0;
        for (auto& subject : subjects) {
            for (auto& topic : subject.topics) {
                total++;
                if (topic.completed) completed++;
            }
        }
        
        int percent = (total > 0) ? (completed * 100) / total : 0;
        
        attron(COLOR_PAIR(3));
        mvprintw(4, 2, "Overall Progress: %d/%d topics (%d%%)", completed, total, percent);
        
        // Progress bar
        mvprintw(5, 2, "[");
        int barWidth = 50;
        int filled = (barWidth * completed) / total;
        for (int i = 0; i < barWidth; i++) {
            if (i < filled) {
                attron(COLOR_PAIR(2));
                addch('#');
                attroff(COLOR_PAIR(2));
            } else {
                addch('-');
            }
        }
        addch(']');
        attroff(COLOR_PAIR(3));
    }

    void drawSubjects(bool inSubjectPanel) {
        // Draw panel border
        if (inSubjectPanel) {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(6, 1, "+--- SUBJECTS ---+");
            attroff(COLOR_PAIR(2) | A_BOLD);
        } else {
            attron(COLOR_PAIR(1));
            mvprintw(6, 1, "+--- SUBJECTS ---+");
            attroff(COLOR_PAIR(1));
        }
        
        for (size_t i = 0; i < subjects.size(); i++) {
            int completed = 0;
            for (auto& topic : subjects[i].topics) {
                if (topic.completed) completed++;
            }
            
            if (i == currentSubject) {
                attron(COLOR_PAIR(4) | A_REVERSE | A_BOLD);
            }
            
            mvprintw(8 + i, 4, "> %s (%d/%zu)", 
                     subjects[i].name.c_str(), 
                     completed, 
                     subjects[i].topics.size());
            
            if (i == currentSubject) {
                attroff(COLOR_PAIR(4) | A_REVERSE | A_BOLD);
            }
        }
    }

    void drawTopics(bool inSubjectPanel) {
        int startY = 7;
        int maxVisible = LINES - startY - 5;
        
        // Draw panel border
        if (!inSubjectPanel) {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(6, 34, "+--- TOPICS ---+");
            attroff(COLOR_PAIR(2) | A_BOLD);
        } else {
            attron(COLOR_PAIR(1));
            mvprintw(6, 34, "+--- TOPICS ---+");
            attroff(COLOR_PAIR(1));
        }
        
        auto& topics = subjects[currentSubject].topics;
        
        // Adjust scroll offset
        if (currentTopic < scrollOffset) {
            scrollOffset = currentTopic;
        } else if (currentTopic >= scrollOffset + maxVisible) {
            scrollOffset = currentTopic - maxVisible + 1;
        }
        
        for (size_t i = scrollOffset; i < topics.size() && i < scrollOffset + maxVisible; i++) {
            int y = startY + 1 + (i - scrollOffset);
            
            if (i == currentTopic) {
                attron(COLOR_PAIR(4) | A_REVERSE);
            }
            
            string status = topics[i].completed ? "[X]" : "[ ]";
            mvprintw(y, 37, "%s %s", status.c_str(), topics[i].name.c_str());
            
            if (topics[i].completed && !topics[i].completedDate.empty()) {
                attron(COLOR_PAIR(3));
                mvprintw(y, 75, "(%s)", topics[i].completedDate.c_str());
                attroff(COLOR_PAIR(3));
            }
            
            if (i == currentTopic) {
                attroff(COLOR_PAIR(4) | A_REVERSE);
            }
        }
    }

    void drawFooter() {
        attron(COLOR_PAIR(1));
        mvprintw(LINES - 3, 0, "--------------------------------------------------------------------------------");
        mvprintw(LINES - 2, 2, "UP/DOWN: Navigate | TAB: Switch Panel | SPACE: Toggle | S: Save | Q: Quit");
        attroff(COLOR_PAIR(1));
    }

public:
    JEESyllabusTracker() {
        initializeSubjects();
        loadProgress();
    }

    void run() {
        initscr();
        start_color();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);
        
        // Color pairs
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_WHITE, COLOR_BLACK);
        
        bool inSubjectPanel = true;
        bool running = true;
        
        while (running) {
            clear();
            drawHeader();
            drawStats();
            drawSubjects(inSubjectPanel);
            drawTopics(inSubjectPanel);
            drawFooter();
            refresh();
            
            int ch = getch();
            
            switch (ch) {
                case KEY_UP:
                    if (inSubjectPanel) {
                        if (currentSubject > 0) currentSubject--;
                    } else {
                        if (currentTopic > 0) currentTopic--;
                    }
                    break;
                    
                case KEY_DOWN:
                    if (inSubjectPanel) {
                        if (currentSubject < subjects.size() - 1) currentSubject++;
                    } else {
                        if (currentTopic < subjects[currentSubject].topics.size() - 1) 
                            currentTopic++;
                    }
                    break;
                    
                case '\t':
                    inSubjectPanel = !inSubjectPanel;
                    if (inSubjectPanel) {
                        currentTopic = 0;
                        scrollOffset = 0;
                    }
                    break;
                    
                case ' ':
                    if (!inSubjectPanel) {
                        auto& topic = subjects[currentSubject].topics[currentTopic];
                        topic.completed = !topic.completed;
                        if (topic.completed) {
                            topic.completedDate = getCurrentDate();
                        } else {
                            topic.completedDate = "";
                        }
                        saveProgress();
                    }
                    break;
                    
                case 's':
                case 'S':
                    saveProgress();
                    mvprintw(LINES - 1, 2, "Progress saved!");
                    refresh();
                    napms(1000);
                    break;
                    
                case 'q':
                case 'Q':
                    running = false;
                    break;
            }
        }
        
        endwin();
    }
};

int main() {
    JEESyllabusTracker tracker;
    tracker.run();
    return 0;
}
