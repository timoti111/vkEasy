#pragma once

#include <exception>
#include <map>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
enum class Error {
    CreationInfoModifyAfterInitialization,
    RequirementsNotFulfilled,
    MultipleInitializations,
};

class Errorable {
    friend class Exception;

public:
    Errorable() = delete;
    Errorable(const std::string& objectName);

protected:
    const std::string& objectName();
    void error(Error error);
    std::string m_objectName;
};

class Exception : public std::exception {
public:
    Exception() = delete;
    Exception(Errorable* object, Error error) noexcept;

    Error error() const noexcept;
    const char* what() const noexcept;

private:
    Error m_error;
    Errorable* m_object;
    std::string m_errorText;
    static std::map<Error, std::string> m_errorTexts;
};
}
