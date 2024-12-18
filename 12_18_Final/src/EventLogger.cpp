#include "EventLogger.hpp"


ofstream logfile;

void event(const string& message, const string& name, int x, int y, char direction) {
    // Ensure the log file is only created once
    if (!logfile.is_open()) {
        time_t tnow = time(0);
        tm* tinfo = localtime(&tnow);

        char timestamp[9];
        strftime(timestamp, sizeof(timestamp), "%H_%M_%S", tinfo);

        string filename = "GameLog_" + string(timestamp) + ".txt";
        logfile.open(filename, ios::app);
    }

    // Get the current time for a timestamp
    time_t tnow = time(0);
    tm* tinfo = localtime(&tnow);
    char timestamp[9];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tinfo);

    // Write to the log file
    logfile << "[" << timestamp << "] " << name << ": ";
    if (!message.empty()) {
        logfile << message;
    }
    if (x != -1 && y != -1) {
        logfile << " (" << x << ", " << y << ")";
    }
    if (direction != ' ') {
        logfile << " Direction: " << direction;
    }
    logfile << endl;
}
