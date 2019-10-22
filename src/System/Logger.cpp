#include "System/Logger.h"

using namespace Sourcehold::System;

static bool coloredOutput;

Logger::Logger()
{

}

Logger::~Logger()
{
}

std::ostream &Logger::log(LogType type, const std::string& subsystem)
{
    std::string msg = "";
    if(coloredOutput) {
        switch(type) {
        case LogType::ERROR:
            msg = "\033[1;31m[ " + subsystem + " ]\033[0m -> ";
            break;
        case LogType::WARNING:
            msg = "\033[1;33m[ " + subsystem + " ]\033[0m -> ";
            break;
        case LogType::MESSAGE:
            msg = "\033[1;34m[ " + subsystem + " ]\033[0m -> ";
            break;
        case LogType::SUCCESS:
            msg = "\033[1;32m[ " + subsystem + " ]\033[0m -> ";
            break;
        default:
            break;
        }
    }
    else {
        msg = "[ " + subsystem + " ] -> ";
    }
    return std::cout << msg;
}

std::ostream &Logger::error(Subsystem subsystem)
{
    return log(LogType::ERROR, SubsystemName(subsystem));
}

std::ostream &Logger::warning(Subsystem subsystem)
{
    return log(LogType::WARNING, SubsystemName(subsystem));
}

std::ostream &Logger::message(Subsystem subsystem)
{
    return log(LogType::MESSAGE, SubsystemName(subsystem));
}

std::ostream &Logger::success(Subsystem subsystem)
{
    return log(LogType::SUCCESS, SubsystemName(subsystem));
}

void Logger::SetColorOutput(bool a)
{
    coloredOutput = a;
}

bool Logger::GetColorOutput()
{
    return coloredOutput;
}

std::string Logger::SubsystemName(Subsystem sys)
{
    std::string s;

    switch(sys) {
    case AUDIO:
        s = "AUDIO";
        break;
    case EVENTS:
        s = "EVENTS";
        break;
    case GUI:
        s = "GUI";
        break;
    case PARSERS:
        s = "PARSERS";
        break;
    case RENDERING:
        s = "RENDERING";
        break;
    case GAME:
        s = "GAME";
        break;
    case SOURCEHOLD:
    default:
        s = "UNKNOWN";
        break;
    }

    return s;
}
