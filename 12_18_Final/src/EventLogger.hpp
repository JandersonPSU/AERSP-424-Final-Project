#ifndef EVENTLOGGER_HPP
#define EVENTLOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

// Global function to log events
void event(const string& message, const string& name = "Console", int x = -1, int y = -1, char direction = ' ');

#endif
