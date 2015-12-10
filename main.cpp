#include <QApplication>
#include "ProvidenceKOSLookup.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Providence");
    QCoreApplication::setApplicationName("KOS Lookup");
    ProvidenceKOSLookup foo;
    return app.exec();
}
