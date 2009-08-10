#include <QMainWindow>
#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    QMainWindow mainwindow;
    mainwindow.show();

    app.exec();
}
