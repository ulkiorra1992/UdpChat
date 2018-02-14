#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include "core_global.h"

#include <QString>
#include <QVariant>

class CORESHARED_EXPORT ProjectSettings
{
public:
    /**
     * @brief getInstance синглтон.
     * @return возвращает указатель на себя.
     */
    static ProjectSettings *getInstance();

private:
    ProjectSettings() {

    };
    virtual ~ProjectSettings() {

    };

    static ProjectSettings *instance_;
};

#endif // PROJECTSETTINGS_H
