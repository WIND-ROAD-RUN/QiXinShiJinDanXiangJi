#include <QMessageBox>
#include <QtWidgets/QApplication>

#include "Modules.hpp"
#include "QiXinShiJinDanXiangJi.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

	if (!Modules::check())
	{
		return 1;
	}

	Modules::getInstance().build();

	QiXinShiJinDanXiangJi w;

	Modules::getInstance().connect();
	Modules::getInstance().start();

#ifdef NDEBUG
	w.showFullScreen();
#else
	w.show();
#endif
    return a.exec();
}
