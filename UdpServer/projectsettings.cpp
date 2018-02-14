#include "projectsettings.h"

ProjectSettings *ProjectSettings::instance_ = 0;

ProjectSettings *ProjectSettings::getInstance()
{
    if (!instance_) {
        instance_ = new ProjectSettings();
    }
    return instance_;
}

