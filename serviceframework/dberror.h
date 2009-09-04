#ifndef DBERROR_H
#define DBERROR_H

#include <QString>

class DBError
{
    public:
        enum ErrorCode {
            NoError,
            DatabaseNotOpen = -2000,    //A connection with the database has not been opened
                                        //  database needs to be opened before any operations take place
            InvalidDatabaseConnection,  //The database connection does not have a valid driver
            LocationAlreadyRegistered,  //A service location has already been registered.
            IfaceImplAlreadyRegistered, //An interface implementation by a given service is already registered to that service
            NotFound,       
            SqlError,               //An Sql error occurred.
            IfaceIDNotExternal,     //InterfaceID does not refer to an external interface implementation
            CannotCreateDbDir,      //Directory to contain database could not be created(usu a permissions issue)
            CannotOpenServiceDb,     //service database cannot be opened(usually a permissions issue)
            ExternalIfaceIDFound,   //Notification for interfaceDefault() on a user scope database
                                    //  to indicate that a default refers to an interface implementation in the
                                    //  system scope database
            InvalidDescriptorScope, //Notification for setInterfaceDefault() on a system scope database
                                    //  to indicate that a user scope descriptor cannot be used
                                    //  with a system scope database.
            InvalidDatabaseFile,    //database file is corrupted or not a valid database
            NoWritePermissions,     //trying to perform a write operation without sufficient permissions
            UnknownError
        };
        DBError();
        void setError(ErrorCode error, const QString &errorText = QString());
        void setSQLError(const QString &errorText) {
            m_error = SqlError;
            m_text = errorText;
        }
        void setNotFoundError(const QString &errorText) {
            m_error = NotFound;
            m_text = errorText;
        }
        QString text() const { return m_text; }
        ErrorCode code() const { return m_error; }
    private:
        QString m_text;
        ErrorCode m_error;
};

#endif  //DBERROR_H
