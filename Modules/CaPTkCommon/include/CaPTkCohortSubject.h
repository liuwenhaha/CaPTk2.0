#ifndef CaPTkCohortSubject_h
#define CaPTkCohortSubject_h

// The following header file is generated by CMake and thus it's located in
// the build directory. It provides an export macro for classes and functions
// that you want to be part of the public interface of your module.
#include <MitkCaPTkCommonExports.h>

namespace captk
{
class CohortStudy;
}

#include <QString>
#include <QList>
#include <QSharedPointer>

namespace captk
{
class MITKCAPTKCOMMON_EXPORT CohortSubject
{
public:
    CohortSubject();

    ~CohortSubject();

    /* Getters */

    QString GetName();

    QList<QSharedPointer<CohortStudy>> GetStudies();

    /* Setters */

    void SetName(QString name);

    void SetStudies(QList<QSharedPointer<CohortStudy>> studies);

private:
    QString m_Name;
    QList<QSharedPointer<CohortStudy>> m_Studies;
};
}

#endif // ! CaPTkCohortSubject_h