#include <vkEasy/Error.h>

using namespace VK_EASY_NAMESPACE;

Exception::Exception(Errorable* object, Error error) noexcept
    : std::exception()
    , m_error(error)
    , m_object(object)
{
    m_errorText = "Error: vk::easy::" + m_object->objectName() + " " + m_errorTexts[m_error];
}

Error Exception::error() const noexcept
{
    return m_error;
}

const char* Exception::what() const noexcept
{
    return m_errorText.c_str();
}

std::map<Error, std::string> Exception::m_errorTexts {
    { Error::CreationInfoModifyAfterInitialization, "Setting object info even though object is already initialized" },
    { Error::RequirementsNotFulfilled, "Specified requrements cannot be fulfilled" },
    { Error::MultipleInitializations, "Trying to initialize multiple times" },
    { Error::NotRecordingGraph, "record() called while recording graph recording is not started yet" },
    { Error::NotRecordingGraph, "run() called while recording graph" }
};

Errorable::Errorable(const std::string& objectName)
    : m_objectName(objectName)
{
}

const std::string& Errorable::objectName()
{
    return m_objectName;
}

void Errorable::error(Error error)
{
    throw Exception(this, error);
}
