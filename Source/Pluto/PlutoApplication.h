#ifndef __PLUTO_APPLICATION_H__
#define __PLUTO_APPLICATION_H__

#include <QApplication>

class PlutoKernelProxy;
class PlutoApplication : public QApplication
{
    Q_OBJECT

public:
    explicit PlutoApplication(int argc, char** argv);
    ~PlutoApplication();

    void init();

    PlutoKernelProxy* kernel() const;

    // Singleton access
    static PlutoApplication& instance();

public slots:
    void start_kernel();

private:
    PlutoKernelProxy* _kernel;
};

#endif // __PLUTO_APPLICATION_H__
