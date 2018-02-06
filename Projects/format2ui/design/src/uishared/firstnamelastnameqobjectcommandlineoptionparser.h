#include <QStringList>
#include <QTextStream>

class FirstNameLastNameQObjectCommandLineOptionParser
{
public:
    FirstNameLastNameQObjectCommandLineOptionParser();
    bool parseArgs();
    QString firstNameDefaultValueParsedFromProcessArg() const;
    QString lastNameDefaultValueParsedFromProcessArg() const;
    QStringList top5MoviesDefaultValueParsedFromProcessArg() const;
private:
    void showUsage();
    int parseOptionalArg(QStringList *argz, const QString &optionalArg, QString *out_String);
    bool parseOptionalArgAndHandleErrors(QStringList *argz, const QString &optionalArg, QString *out_String);

    QTextStream m_StdErr;

    QString m_FirstNameDefaultValueParsedFromProcessArg;
    QString m_LastNameDefaultValueParsedFromProcessArg;
};
