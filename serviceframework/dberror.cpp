#include "dberror.h"

DBError::DBError()
{
    setError(NoError);
}

void DBError::setError(ErrorCode error, const QString &text)
{
    m_error = error;
    switch (error) {
        case (NoError):
            m_text = "No error";
            break;
        case(DatabaseNotOpen):
            m_text = "Database not open";
            break;
        case(InvalidDatabaseConnection):
            m_text = "Invalid database connection";
            break;
        case(ExternalIfaceIDFound):
            m_text = "External InterfaceID found";
            break;
        case(SqlError):
        case(NotFound):
        case(LocationAlreadyRegistered):
        case(IfaceImplAlreadyRegistered):
        case(CannotCreateDbDir):
        case(InvalidDescriptorScope):
        case(IfaceIDNotExternal):
        case(InvalidDatabaseFile):
        case(NoWritePermissions):
        case(CannotOpenServiceDb):
            m_text = text;
            break;
        default:
            m_text= "Unknown error";
            m_error = UnknownError;
    }
}
